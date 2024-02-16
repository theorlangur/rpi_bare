#ifndef RPI_I2C_FMT_H_
#define RPI_I2C_FMT_H_

#include "rpi_i2c_bare.h"
#include "../tools/formatter.h"

namespace tools
{
    //forward
    template<class T> struct formatter_t;

    template<>
    struct formatter_t<rpi::i2c::Error>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, rpi::i2c::Error e)
        {
            size_t res = 0;
            char err_bits[8] = {0};
            int err_n = 0;
            switch(e.code)
            {
                case rpi::i2c::ErrorCode::Err:
                    err_bits[err_n++] = 'N';
                    break;
                case rpi::i2c::ErrorCode::Timeout:
                    err_bits[err_n++] = 'C';
                    break;
            }
            err_bits[err_n++] = ';';
            dst(std::string_view(err_bits, err_n));
            return err_n;
        }
    };
}

#endif
