#include <monapi/get_digits.h>

uint32_t MonAPI::get_digits(uint32_t n)
{
    uint32_t number = n;
    uint32_t digits = 0;
    for (digits = 1; 1; digits++)
    {
        number /= 10;
        if (number == 0) break;
    }
    return digits;
}

