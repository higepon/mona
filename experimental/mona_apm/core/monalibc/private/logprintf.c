/*!
 * \brief logprintf
 *
 * \author Higepon
 * \param format specifies how subsequent arguments
 * \return  none
 *
 * */
#include <stdio.h>
#include <stdarg.h>
#include <monapi/syscall.h>

int syscall_log_print(const char* msg) {
    int result;
    SYSCALL_1(SYSTEM_CALL_LOG_PRINT, result, msg);
    return result;
}


void logprintf(const char* format, ...) {

    char str[512];
    str[0] = '\0';
    va_list args;
    int result;

    va_start(args, format);
    result = vsprintf(str, format, args);
    va_end(args);
    if(result > 512) {
        /* over flow */
        syscall_log_print("logprintf:overflow");
    }

    syscall_log_print(str);
}

