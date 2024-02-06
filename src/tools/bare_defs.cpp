#include <stddef.h>

extern "C" size_t strlen(const char *pStr)
{
    size_t r = 0;
    while(*pStr++) ++r;
    return r;
}
