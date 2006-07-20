#include <monalibc/stdarg.h>

/*!
 * \brief sscanf
 *
 * \param s      buf printed characters
 * \param format specifies how subsequent arguments
 * \return  the number of characters printed (including '\\0' end of string)
 */
int sscanf(const char *s, const char *format, ...) {
    int result;
    va_list args;

    va_start(args, format);
    result = vsscanf(s, format, args);
    va_end(args);

    return result;
}

