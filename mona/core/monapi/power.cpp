#include <monapi/power.h>

uint32_t MonAPI::power(uint32_t x, uint32_t y)
{
    if (y == 0) return 1;
    uint32_t ret = x;
    for (int i = 0; i < (int)(y - 1); i++)
    {
        ret *= x;
    }
    return ret;
}
