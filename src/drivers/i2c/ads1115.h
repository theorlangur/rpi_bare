#ifndef ADS1115_H_
#define ADS1115_H_

#include "../../rpi_bare/rpi_i2c_bare.h"
#include "../../rpi_bare/rpi_timers_bare.h"

template<class I2C>
class ADS1115
{
    using Timer = rpi::timers::Sys<typename I2C::RPiType>;
    enum class Reg: uint8_t
    {
        Conversion  = 0b00,
        Config      = 0b01,
        LoThreshold = 0b10,
        HiThreshold = 0b11
    };
public:
    enum class Error: uint8_t
    {
        I2Ce = 1,
        Config = 2,
        Conversion = 3,
        WriteReg = 4,
        ChangeReg = 5,
    };
    template<std::integral T>
    using IntResult = std::expected<T, Error/*typename I2C::Error*/>;
    using GenericResult = std::expected<bool, Error/*typename I2C::Error*/>;
    using FloatResult = std::expected<float, Error/*typename I2C::Error*/>;

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
            return (1'000'000 + sps - 1) / sps;
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

    IntResult<int16_t> read_single_raw()
    {
        auto c = m_Device.communicate();
        Config cfg = m_Config;
        cfg.m_bits.conversion = 1;
        cfg.m_bits.mode = Config::Mode::SingleShot;
        if (auto r = write_to_register(c, Reg::Config, cfg); !r)
            return std::unexpected(r.error());

        Timer::delay_microseconds(m_SampleWaitTime);
        if (auto r = change_register(c, Reg::Conversion); !r)
            return std::unexpected(r.error());

        auto read_res = c.template read<int16_t>();
        if (!read_res)
            return std::unexpected(Error::Conversion);
        return *read_res;
        /*return c.template read<int16_t>()
                .transform_error([](auto){ return IntResult<int16_t>(std::unexpect_t{}, Error::Conversion); })
                .and_then([](auto v){ return IntResult<int16_t>(v);}); 
                */
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

    GenericResult change_register(I2C::Device::Channel &c, Reg r) 
    { 
        if (auto res = c.write((uint8_t)r))
            return true;
        return std::unexpected(Error::ChangeReg);
        /*return c.write((uint8_t)r)
            .transform_error([](auto e){ return GenericResult(std::unexpect_t{}, Error::ChangeReg); })
            .and_then([](auto){ return GenericResult(true);}); 
            */
    }
    GenericResult write_to_register(I2C::Device::Channel &c, Reg r, uint16_t v) 
    { 
        uint8_t buf[3];
        buf[0] = (uint8_t)r;
        buf[1] = uint8_t(v >> 8);
        buf[2] = uint8_t(v & 0xff);
        if (auto res = c.write(buf, sizeof(buf)))
            return true;

        return std::unexpected(Error::WriteReg);
        /*return c.write(buf, sizeof(buf))
            .transform_error([](auto e){ return GenericResult(std::unexpect_t{}, Error::WriteReg); })
            .and_then([](auto){ return GenericResult(true);}); 
            */
    }
    GenericResult write_to_register(I2C::Device::Channel &c, Reg r, Config cfg) { return write_to_register(c, r, cfg.m_dw); }

    I2C::Device m_Device;
    Config m_Config;
    uint32_t m_SampleWaitTime;
    float m_MaxRange;
    float m_MinRange;
};

#endif
