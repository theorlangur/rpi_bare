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
            switch(e.code)
            {
                case rpi::i2c::ErrorCode::Err:
                    dst(std::string_view("NACK;B:"));
                    break;
                case rpi::i2c::ErrorCode::Timeout:
                    dst(std::string_view("TO;B:"));
                    break;
            }
            if (auto r = formatter_t<decltype(e.transferred)>::format_to(std::forward<Dest>(dst), fmtStr, e.transferred); !r)
                return r;
            else
                res += *r;
            return res;
        }
    };
}

#endif
