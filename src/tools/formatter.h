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
        static bool format_to(Dest &&dst, std::string_view const& fmtStr, T &&v)
        {
            uint8_t t[16]={'0'};
            uint8_t d = v == 0 ? 1 : 0;

            while(v)
            {
                t[d++] = '0' + (v % 10);
                v /= 10;
            }
            for(uint8_t i = 0; i < d; ++i)
                dst(t[d - i - 1]);
            return true;
        }
    };

    template<>
    struct formatter_t<char>
    {
        template<FormatDestination Dest>
        static bool format_to(Dest &&dst, std::string_view const& fmtStr, char c) { dst(c); return true; }
    };

    template<>
    struct formatter_t<const char*>
    {
        template<FormatDestination Dest>
        static bool format_to(Dest &&dst, std::string_view const& fmtStr, const char *pStr) { dst(pStr); return true; }
    };

    template<>
    struct formatter_t<std::string_view>
    {
        template<FormatDestination Dest>
        static bool format_to(Dest &&dst, std::string_view const& fmtStr, std::string_view sv) { dst(sv); return true; }
    };

    template<FormatDestination Dest, class... Args>
    size_t format_to(Dest &&dst, std::string_view f, Args &&...args)
    {
        return f.size();
    }

    template<size_t I, FormatDestination Dest, class T, class... Rest>
    bool format_nth_arg(size_t i, std::string_view const& fmtStr, Dest &&dst, T &&arg, Rest &&...args)
    {
        //error checking?
        if (I == i)
            return formatter_t<T>::format_to(std::forward<Dest>(dst), fmtStr, std::forward<T>(arg));

        if constexpr (sizeof...(Rest) > 0)
            return format_nth_arg<I+1>(i, fmtStr, std::forward<Dest>(dst), std::forward<Rest>(args)...);
        else
            return false;
    }

    template<FormatDestination Dest, class... Args>
    std::expected<size_t, FormatError> format_to(Dest &&dst, const char *pStr, Args &&...args)
    {
        auto pBegin = pStr;
        char prev = 0, c;
        size_t arg = 0;
        while((c = *pStr))
        {
            if (c == '{')
            {
                if (prev == '\\')
                {
                    dst(std::string_view(pBegin, pStr - 1));
                    pBegin = pStr;
                }
                else
                {
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


                    if (targ < sizeof...(args))
                    {
                        if (*pStr == ':') ++pStr;
                        auto pFmtBegin = pStr;
                        while(*pStr && *pStr != '}')
                            ++pStr;
                        if (*pStr != '}')
                            return std::unexpected(FormatError::InvalidFormatString);
                        std::string_view fmtStr(pFmtBegin, pStr++);
                        if (!format_nth_arg<0>(targ, fmtStr, std::forward<Dest>(dst), std::forward<Args>(args)...))
                            return std::unexpected(FormatError::CouldNotFormat);
                        pBegin = pStr;
                    }
                    else
                        return std::unexpected{explicitNumber ? FormatError::InvalidFormatArgumentNumber : FormatError::NotEnoughFormatArguments};
                }
            }
            prev = *pStr++;
        }
        return 0;
    }
}

#endif
