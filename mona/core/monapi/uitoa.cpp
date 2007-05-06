#include <monapi/uitoa.h>

using namespace MonAPI;

char* MonAPI::uitoa(char* buf, uint32_t n)
{
    static const char digit[] = {"0123456789"};
    uint32_t number = n;

    uint32_t digits = get_digits(n);
    for (int i = digits; i >= 1; i--)
    {
        uint32_t tmp = (uint32_t)((number / power(10, i - 1)));
        buf[digits - i] = digit[tmp];
        number -= tmp * power(10, i - 1);
    }
    buf[digits] = '\0';
    return buf;
}
