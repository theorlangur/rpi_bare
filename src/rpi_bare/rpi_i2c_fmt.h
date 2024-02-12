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
            if (auto r = tools::format_to(std::forward<Dest>(dst), "{:X}", e.status.raw()); !r)
                return r;
            char err_bits[8] = {0};
            int err_n = 0;
            if (e.status.err_ack) err_bits[err_n++] = 'N';
            if (e.status.clkt_stretch_timeout) err_bits[err_n++] = 'C';
            err_bits[err_n++] = ';';
            err_bits[err_n++] = 'B';
            err_bits[err_n++] = ':';
            dst(std::string_view(err_bits, err_n));
            if (auto r = formatter_t<decltype(e.transferred)>::format_to(std::forward<Dest>(dst), fmtStr, e.transferred); !r)
                return r;
            else
                res += *r;
            return res;
        }
    };
}

#endif
