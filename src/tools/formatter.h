#ifndef FORMATTER_H_
#define FORMATTER_H_
#include <string_view>
#include <expected>

extern "C" size_t strlen(const char *pStr);

namespace tools
{
    enum class FormatError
    {
        InvalidFormatString,
        InvalidFormatArgumentNumber,
        NotEnoughFormatArguments,
        CouldNotFormat,
    };

    template<typename Dest>
    concept FormatDestination = requires(Dest a)
    {
        {a('0')};//being able to 'output' a single character
        {a(std::string_view{})};//being able to 'output' a string
    };

    template<class T>
    struct formatter_t;

    template<std::floating_point F>
    struct FloatingPointFormatTraits;

    template<>
    struct FloatingPointFormatTraits<float>
    {
        constexpr static float posExpThreshold=1e7;
        constexpr static float negExpThreshold=1e-5;
    };

    template<>
    struct FloatingPointFormatTraits<double>
    {
        constexpr static double posExpThreshold=1e7;
        constexpr static double negExpThreshold=1e-5;
    };


    template<std::floating_point T>
    struct formatter_t<T>
    {
        static int16_t normalizeFloat(T &value)
        {
            using FP = FloatingPointFormatTraits<T>;
            int16_t exponent = 0;

            if (value >= FP::posExpThreshold) {
                if (value >= 1e256) {
                    value /= 1e256;
                    exponent += 256;
                }
                if (value >= 1e128) {
                    value /= 1e128;
                    exponent += 128;
                }
                if (value >= 1e64) {
                    value /= 1e64;
                    exponent += 64;
                }
                if (value >= 1e32) {
                    value /= 1e32;
                    exponent += 32;
                }
                if (value >= 1e16) {
                    value /= 1e16;
                    exponent += 16;
                }
                if (value >= 1e8) {
                    value /= 1e8;
                    exponent += 8;
                }
                if (value >= 1e4) {
                    value /= 1e4;
                    exponent += 4;
                }
                if (value >= 1e2) {
                    value /= 1e2;
                    exponent += 2;
                }
                if (value >= 1e1) {
                    value /= 1e1;
                    exponent += 1;
                }
            }else if (value > 0 && value <= FP::negExpThreshold) {
                if (value < 1e-255) {
                    value *= 1e256;
                    exponent -= 256;
                }
                if (value < 1e-127) {
                    value *= 1e128;
                    exponent -= 128;
                }
                if (value < 1e-63) {
                    value *= 1e64;
                    exponent -= 64;
                }
                if (value < 1e-31) {
                    value *= 1e32;
                    exponent -= 32;
                }
                if (value < 1e-15) {
                    value *= 1e16;
                    exponent -= 16;
                }
                if (value < 1e-7) {
                    value *= 1e8;
                    exponent -= 8;
                }
                if (value < 1e-3) {
                    value *= 1e4;
                    exponent -= 4;
                }
                if (value < 1e-1) {
                    value *= 1e2;
                    exponent -= 2;
                }
                if (value < 1e0) {
                    value *= 1e1;
                    exponent -= 1;
                }
            }

            return exponent;
        }

        static void splitFloat(T value, uint32_t &integralPart,
                uint32_t &decimalPart, int16_t &exponent) {
            exponent = normalizeFloat(value);

            integralPart = (uint32_t)value;
            double remainder = value - integralPart;

            remainder *= 1e9;
            decimalPart = (uint32_t)remainder;  

            // rounding
            remainder -= decimalPart;
            if (remainder >= 0.5) {
                decimalPart++;
                if (decimalPart >= 1000000000) {
                    decimalPart = 0;
                    integralPart++;
                    if (exponent != 0 && integralPart >= 10) {
                        exponent++;
                        integralPart = 1;
                    }
                }
            }
        }

        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, T v)
        {
            bool neg = v < 0;
            if (neg)
                v = -v;

            uint32_t intPart, decPart;
            int16_t exp;
            splitFloat(v, intPart, decPart, exp);
            constexpr size_t n = 16;
            char intStr[n];
            intStr[15] = '0';
            uint8_t d = intPart == 0 ? 1 : 0;

            while(intPart)
            {
                intStr[n - ++d] = '0' + (intPart % 10);
                intPart /= 10;
            }
            if (neg)
                intStr[n - ++d] = '-';
            //printing integral part
            dst(std::string_view(intStr + n - d, d));
            //printing decimal part
            uint8_t r = d;
            d = 0;
            while(decPart)
            {
                auto digit = decPart % 10;
                if (digit || d)
                    intStr[n - ++d] = '0' + digit;
                decPart /= 10;
            }
            if (d)
                intStr[n - ++d] = '.';
            dst(std::string_view(intStr + n - d, d));
            r += d;

            //printing exponent
            if (exp)
            {
                neg = exp < 0;
                if (neg)
                    exp = -exp;

                d = 0;
                while(exp)
                {
                    intStr[n - ++d] = '0' + (exp % 10);
                    exp /= 10;
                }
                if (neg)
                    intStr[n - ++d] = '-';
                intStr[n - ++d] = 'e';
                r += d;
                dst(std::string_view(intStr + n - d, d));
            }
            return r;
        }
    };

    template<size_t N>
    struct formatter_t<uint8_t[N]>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, uint8_t(&v)[N])
        {
            for(uint8_t b : v)
            {
                uint8_t hi = (b >> 4) & 0xf;
                uint8_t lo = b & 0xf;
                if (hi < 10)
                    dst('0' + hi);
                else
                    dst('a' + hi);
                if (lo < 10)
                    dst('0' + lo);
                else
                    dst('a' + lo);
                dst(' ');
            }
            return N * 3;
        }
    };

    template<std::integral T>
    struct formatter_t<T>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, T v)
        {
            if (!fmtStr.empty())
            {
                constexpr T mask = 0x0f;
                constexpr const uint8_t n = sizeof(T) * 2;
                if (fmtStr[0] == 'x')
                {
                    dst("0x");
                    for(uint8_t i = 0; i < n; ++i)
                    {
                        uint8_t digit = (v >> (n - i - 1) * 4) & mask;
                        char c = digit < 10 ? ('0' + digit) : ('a' + digit - 10);
                        dst(c);
                    }
                }else
                {
                    dst("0x");
                    for(uint8_t i = 0; i < n; ++i)
                    {
                        uint8_t digit = (v >> (n - i - 1) * 4) & mask;
                        char c = digit < 10 ? ('0' + digit) : ('A' + digit - 10);
                        dst(c);
                    }
                }
                return n + 2;
            }
            bool neg;
            if constexpr (std::is_signed_v<T>)
            {
                neg = v < 0;
                if (neg)
                    v = -v;
            }else
                neg = false;

            char t[16];
            constexpr uint8_t n = std::size(t);
            t[15] = '0';
            uint8_t d = v == 0 ? 1 : 0;

            while(v)
            {
                t[n - ++d] = '0' + (v % 10);
                v /= 10;
            }

            if constexpr (std::is_signed_v<T>)
            {
                if (neg)
                    t[n - ++d] = '-';
            }

            dst(std::string_view(t + n - d, d));
            return d;
        }
    };

    template<class T> requires (std::is_scoped_enum_v<T>)
    struct formatter_t<T>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, T const&v)
        {
            return formatter_t<std::underlying_type_t<T>>::format_to(std::forward<Dest>(dst), fmtStr, std::underlying_type_t<T>(v));
        }
    };

    template<class Value, class Error>
    struct formatter_t<std::expected<Value,Error>>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, std::expected<Value,Error> const& v) 
        { 
            if (v)
                return formatter_t<Value>::format_to(std::forward<Dest>(dst), fmtStr, *v); 
            else
            {
                dst(std::string_view("E:"));
                auto r = formatter_t<Error>::format_to(std::forward<Dest>(dst), fmtStr, v.error());
                if (!r)
                    return r;
                return *r + 4; 
            }
        }
    };

    template<>
    struct formatter_t<char>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, char c) { dst(c); return 1; }
    };

    template<>
    struct formatter_t<const char*>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, const char *pStr) { dst(pStr); return strlen(pStr); }
    };

    template<>
    struct formatter_t<std::string_view>
    {
        template<FormatDestination Dest>
        static std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view const& fmtStr, std::string_view const& sv) { dst(sv); return sv.size(); }
    };

    template<size_t I, FormatDestination Dest, class T, class... Rest>
    std::expected<size_t, FormatError> format_nth_arg(size_t i, std::string_view const& fmtStr, Dest &&dst, T &&arg, Rest &&...args)
    {
        //error checking?
        if (I == i)
            return formatter_t<std::remove_cvref_t<T>>::format_to(std::forward<Dest>(dst), fmtStr, std::forward<T>(arg));

        if constexpr (sizeof...(Rest) > 0)
            return format_nth_arg<I+1>(i, fmtStr, std::forward<Dest>(dst), std::forward<Rest>(args)...);
        else
            return std::unexpected(FormatError::NotEnoughFormatArguments);
    }

    template<FormatDestination Dest, class... Args>
    std::expected<size_t, FormatError> format_to(Dest &&dst, std::string_view f, Args &&...args)
    {
        size_t res = 0;
        std::string_view::const_iterator pBegin = f.begin();
        char prev = 0, c;
        size_t arg = 0;
        auto  e = f.end();
        auto b = f.begin();
        while(b != e)
        {
            char c = *b;
            if (c == '{')
            {
                if (prev == '\\')
                {
                    auto sv = std::string_view(pBegin, b - 1);
                    dst(sv);
                    res += sv.size();
                    pBegin = b;
                }
                else
                {
                    res += b - pBegin;
                    dst(std::string_view(pBegin, b++));
                    //parse format argument
                    bool explicitNumber;
                    size_t targ;
                    if (*b >= '0' && *b <='9')
                    {
                        explicitNumber = true;
                        targ = 0;
                        do
                        {
                            targ = (targ * 10) + (*b++ - '0');
                        }
                        while(*b >= '0' && *b <='9');
                    }else
                    {
                        targ = arg++;
                        explicitNumber = false;
                    }


                    if (targ < sizeof...(args))
                    {
                        if (*b == ':') ++b;
                        auto pFmtBegin = b;
                        while(*b && *b != '}')
                            ++b;
                        if (*b != '}')
                            return std::unexpected(FormatError::InvalidFormatString);
                        std::string_view fmtStr(pFmtBegin, b++);
                        if (auto r = format_nth_arg<0>(targ, fmtStr, std::forward<Dest>(dst), std::forward<Args>(args)...); !r)
                            return r;
                        else
                            res += *r;
                        pBegin = b;
                    }
                    else
                        return std::unexpected{explicitNumber ? FormatError::InvalidFormatArgumentNumber : FormatError::NotEnoughFormatArguments};
                }
            }
            prev = *b++;
        }
        if (pBegin != e)
        {
            dst(std::string_view(pBegin, e));
            res += e - pBegin;
        }
        return res;
    }

    template<FormatDestination Dest, class... Args>
    std::expected<size_t, FormatError> format_to(Dest &&dst, const char *pStr, Args &&...args)
    {
        size_t res = 0;
        auto pBegin = pStr;
        char prev = 0, c;
        size_t arg = 0;
        while((c = *pStr))
        {
            if (c == '{')
            {
                if (prev == '\\')
                {
                    res += pStr - pBegin - 1;
                    dst(std::string_view(pBegin, pStr - 1));
                    pBegin = pStr;
                }
                else
                {
                    if constexpr (sizeof...(args) == 0)
                        return std::unexpected{FormatError::NotEnoughFormatArguments};

                    res += pStr - pBegin;
                    dst(std::string_view(pBegin, pStr++));
                    //parse format argument
                    bool explicitNumber;
                    size_t targ;
                    if (*pStr >= '0' && *pStr <='9')
                    {
                        explicitNumber = true;
                        targ = 0;
                        do
                        {
                            targ = (targ * 10) + (*pStr++ - '0');
                        }
                        while(*pStr >= '0' && *pStr <='9');
                    }else
                    {
                        targ = arg++;
                        explicitNumber = false;
                    }


                    if constexpr (sizeof...(args) > 0)
                    {
                        if (targ < sizeof...(args))
                        {
                            if (*pStr == ':') ++pStr;
                            auto pFmtBegin = pStr;
                            while(*pStr && *pStr != '}')
                                ++pStr;
                            if (*pStr != '}')
                                return std::unexpected(FormatError::InvalidFormatString);
                            std::string_view fmtStr(pFmtBegin, pStr);
                            if (auto r = format_nth_arg<0>(targ, fmtStr, std::forward<Dest>(dst), std::forward<Args>(args)...); !r)
                                return r;
                            else
                                res += *r;
                            pBegin = pStr + 1;
                        }
                        else
                            return std::unexpected{explicitNumber ? FormatError::InvalidFormatArgumentNumber : FormatError::NotEnoughFormatArguments};
                    }
                }
            }
            prev = *pStr++;
        }
        if (pBegin != pStr)
        {
            dst(std::string_view(pBegin, pStr));
            res += pStr - pBegin;
        }
        return res;
    }
}

#endif
