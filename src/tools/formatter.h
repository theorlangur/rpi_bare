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
                    dst("0X");
                    for(uint8_t i = 0; i < n; ++i)
                    {
                        uint8_t digit = (v >> (n - i - 1) * 4) & mask;
                        char c = digit < 10 ? ('0' + digit) : ('A' + digit - 10);
                        dst(c);
                    }
                }
                return n + 2;
            }

            char t[16];
            constexpr uint8_t n = std::size(t);
            t[15] = '0';
            uint8_t d = v == 0 ? 1 : 0;

            while(v)
            {
                t[n - ++d] = '0' + (v % 10);
                v /= 10;
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
                dst(std::string_view("Err:"));
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
