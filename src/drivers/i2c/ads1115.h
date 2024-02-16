#ifndef ADS1115_H_
#define ADS1115_H_

#include "../../rpi_bare/rpi_i2c_bare.h"
#include "../../rpi_bare/rpi_timers_bare.h"

template<class I2C>
class ADS1115
{
    using Device = I2C::template Device<true>;
    using Timer = rpi::timers::Sys<typename I2C::RPiType>;
    enum class Reg: uint8_t
    {
        Conversion  = 0b00,
        Config      = 0b01,
        LoThreshold = 0b10,
        HiThreshold = 0b11
    };
public:
    enum class ErrorCode: uint8_t
    {
        I2Ce = 1,
        ConfigSingle = 2,
        Conversion = 3,
        WriteReg = 4,
        ChangeReg = 5,
        ReadReg = 6,
        ConfigContinuous = 7,
        UnexpectedMode = 8,
    };

    struct Error
    {
        rpi::i2c::Error i2c_error;
        ErrorCode code;
    };

    template<std::integral T>
    using IntResult = std::expected<T, Error>;
    using GenericResult = std::expected<void, Error>;
    using FloatResult = std::expected<float, Error>;

    template<ErrorCode mainError>
    static inline Error i2c_error_to_this_error(rpi::i2c::Error &&e) { return {std::move(e), mainError}; }

    struct Config
    {
        enum class ComparatorMode: uint16_t
        {
            Traditional = 0,
            Window = 1
        };
        enum class Polarity: uint16_t
        {
            Low = 0,
            High = 1
        };
        enum class Comparator: uint16_t
        {
            AssertAfter1 = 0,
            AssertAfter2 = 1,
            AssertAfter4 = 2,
            Disable = 3,
        };
        enum class Rate: uint16_t
        {
            Sps8   = 0,
            Sps16  = 1,
            Sps32  = 2,
            Sps64  = 3,
            Sps128 = 4,
            Sps250 = 5,
            Sps475 = 6,
            Sps860 = 7,
        };
        enum class Mode: uint16_t
        {
            Continuous = 0,
            SingleShot = 1
        };
        enum class PGA: uint16_t
        {
            FSR_6_144 = 0,
            FSR_4_096 = 1,
            FSR_2_048 = 2,
            FSR_1_024 = 3,
            FSR_0_512 = 4,
            FSR_0_256_0 = 5,
            FSR_0_256_1 = 6,
            FSR_0_256_2 = 7,
        };
        enum class Mux: uint16_t
        {
            AIN_0_1 = 0,
            AIN_0_3 = 1,
            AIN_1_3 = 2,
            AIN_2_3 = 3,
            AIN_0_G = 4,
            AIN_1_G = 5,
            AIN_2_G = 6,
            AIN_3_G = 7,
        };

        Config() = default;
        Config(uint16_t v):m_dw(v) {}
        Config(const Config &rhs) = default;

        Config& operator=(const Config &rhs) = default;

        uint32_t get_wait_time_us() const
        {
            uint32_t sps;
            switch(m_bits.data_rate)
            {
                case Config::Rate::Sps8:   sps = 8; break;
                case Config::Rate::Sps16:  sps = 16; break;
                case Config::Rate::Sps32:  sps = 32; break;
                case Config::Rate::Sps64:  sps = 64; break;
                case Config::Rate::Sps128: sps = 128; break;
                case Config::Rate::Sps250: sps = 250; break;
                case Config::Rate::Sps475: sps = 475; break;
                case Config::Rate::Sps860: sps = 860; break;
            }
            //+10%
            return (1'100'000) / sps;
        }

        void get_full_scale_range(float &from, float &to) const
        {
            switch(m_bits.pga)
            {
                case PGA::FSR_6_144: from = -6.144f; to = 6.144f; break;
                case PGA::FSR_4_096: from = -4.096f; to = 4.096f; break;
                case PGA::FSR_2_048: from = -2.048f; to = 2.048f; break;
                case PGA::FSR_1_024: from = -1.024f; to = 1.024f; break;
                case PGA::FSR_0_512: from = -0.512f; to = 0.512f; break;
                default: from = -0.256; to = 0.256; break;
            }
        }

        static inline constexpr PGA get_pga_for_voltage(float v)
        {
            if (v < 0) v = -v;
            if (v > 4.096f) return PGA::FSR_6_144;
            if (v > 2.048f) return PGA::FSR_4_096;
            if (v > 1.024f) return PGA::FSR_2_048;
            if (v > 0.512f) return PGA::FSR_1_024;
            if (v > 0.256f) return PGA::FSR_0_512;
            return PGA::FSR_0_256_0;
        }

        union
        {
            struct
            {
                Comparator     comp_queue          : 2 = Comparator::Disable;
                uint16_t       latching_comparator : 1 = 0;
                Polarity       comparator_polarity : 1 = Polarity::Low;
                ComparatorMode comparator_mode     : 1 = ComparatorMode::Traditional;
                Rate           data_rate           : 3 = Rate::Sps128;
                Mode           mode                : 1 = Mode::SingleShot;
                PGA            pga                 : 3 = PGA::FSR_2_048;
                Mux            mux                 : 3 = Mux::AIN_0_1;
                uint16_t       conversion          : 1 = 1;
            }m_bits{};
            uint16_t m_dw;
        };
    };

    enum class Address: uint8_t
    {
        GND = 0x48,
        VDD = 0x49,
        SDA = 0x4A,
        SCL = 0x4B,
    };

    ADS1115(Address addr = Address::GND):m_Device((uint8_t)addr) 
    {
        m_SampleWaitTime = m_Config.get_wait_time_us();
        m_Config.get_full_scale_range(m_MinRange, m_MaxRange);
    }
    void set_addr(Address addr) { m_Device.set_addr((uint8_t)addr); }
    Address get_addr() const { return (Address)m_Device.get_addr(); }

    bool exists() const { return m_Device.exists(); }

    Config::Rate get_conversion_rate() const
    {
        return m_Config.m_bits.data_rate;
    }

    GenericResult set_conversion_rate(Config::Rate r)
    {
        m_Config.m_bits.data_rate = r;
        if (m_Config.m_bits.conversion && m_Config.m_bits.mode == Config::Mode::Continuous)
        {
            //continuous running atm
            //start again with new rate
            return update_config();
        }
        return {};
    }

    Config::PGA get_measure_range_mode() const
    {
        return m_Config.m_bits.pga;
    }

    GenericResult set_measure_range_mode(Config::PGA p)
    {
        m_Config.m_bits.pga = p;
        if (m_Config.m_bits.conversion && m_Config.m_bits.mode == Config::Mode::Continuous)
        {
            //continuous running atm
            //start again with new rate
            return update_config();
        }
        return {};
    }

    IntResult<int16_t> get_hi_threshold() const
    {
        return read_register<int16_t, Reg::HiThreshold>();
    }

    IntResult<int16_t> set_hi_threshold(int16_t v) const
    {
        return write_to_register(m_Device.communicate(), Reg::HiThreshold, v);
    }

    IntResult<int16_t> get_lo_threshold() const
    {
        return read_register<int16_t, Reg::LoThreshold>();
    }

    IntResult<int16_t> set_lo_threshold(int16_t v) const
    {
        return write_to_register(m_Device.communicate(), Reg::LoThreshold, v);
    }

    IntResult<uint16_t> get_config() const
    {
        return read_register<uint16_t, Reg::Config>();
    }

    GenericResult run_continuous()
    {
        m_Config.m_bits.conversion = 1;
        m_Config.m_bits.mode = Config::Mode::Continuous;
        return update_config();
    }

    GenericResult stop_continuous()
    {
        if (m_Config.m_bits.mode != Config::Mode::Continuous)
            return std::unexpected(Error{ErrorCode::UnexpectedMode});
        m_Config.m_bits.conversion = 0;
        m_Config.m_bits.mode = Config::Mode::Continuous;
        return update_config();
    }

    IntResult<int16_t> read_now_raw() const
    {
        return read_register<int16_t, Reg::Conversion, ErrorCode::Conversion>();
    }

    FloatResult read_now() const
    {
        return read_now_raw().and_then([&](int16_t iv){
            float res;
            if (iv < 0)
                res = -((m_MinRange * iv) / int16_t(0x8000));
            else
                res = (m_MaxRange * iv) / int16_t(0x7fff);
            return FloatResult(res);
        });
    }

    IntResult<int16_t> read_single_raw()
    {
        m_Config.m_bits.conversion = 1;
        m_Config.m_bits.mode = Config::Mode::SingleShot;
        if (auto r = update_config(); !r)
            return std::unexpected(r.error());

        Timer::delay_microseconds(m_SampleWaitTime);

        return read_register<int16_t, Reg::Conversion, ErrorCode::Conversion>();
    }

    FloatResult read_single()
    {
        return read_single_raw().and_then([&](int16_t iv){
            float res;
            if (iv < 0)
                res = -((m_MinRange * iv) / int16_t(0x8000));
            else
                res = (m_MaxRange * iv) / int16_t(0x7fff);
            return FloatResult(res);
        });
    }
private:
    GenericResult update_config()
    {
        auto c = m_Device.communicate();
        if (auto r = write_to_register(c, Reg::Config, m_Config); !r)
            return std::unexpected(r.error());
        return {};
    }

    template<typename T, Reg reg, ErrorCode errCode = ErrorCode::ReadReg>
    IntResult<T> read_register() const
    {
        return rpi::tools::convert_expected<Error>(
                m_Device
                    .communicate()
                    .template read_from_reg<T>(uint8_t(reg))
                    , i2c_error_to_this_error<errCode>);
    }

    GenericResult write_to_register(Device::Channel &c, Reg r, uint16_t v) const
    { 
        if (auto res = rpi::tools::convert_expected<Error>(
                m_Device
                    .communicate()
                    .write_to_reg_u16(uint8_t(r), v)
                    , i2c_error_to_this_error<ErrorCode::WriteReg>); !res)
            return std::unexpected(res.error());
        return {};
    }
    GenericResult write_to_register(Device::Channel &c, Reg r, Config cfg) const { return write_to_register(c, r, cfg.m_dw); }

    Device m_Device;
    Config m_Config;
    uint32_t m_SampleWaitTime;
    float m_MaxRange;
    float m_MinRange;
};
#endif
