//
//   Gerated by script. Don't edit this file
//

#include <monapi.h>
#include <sys/error.h>

using namespace MonAPI;

const char* monapi_error_string(int error)
{
    switch(error) {
    case M_OK:
        return  "ok";
    case M_BAD_MUTEX_ID:
        return "bad mutex id";
    case M_TIMED_OUT:
        return "timed out";
    case M_BUSY:
        return "resource busy";
    case M_BAD_CONDITION_ID:
        return "bad condition id";
    case M_BAD_THREAD_ID:
        return "bad thread id";
    case M_BAD_SEMAPHORE_ID:
        return "bad semaphore id";
    case M_NAME_NOT_FOUND:
        return "name not found";
    case M_BAD_MEMORY_MAP_ID:
        return "bad memory map id";
    case M_MESSAGE_OVERFLOW:
        return "message queue overflow";
    case M_MEMORY_MAP_ERROR:
        return "memory map failed";
    case M_BAD_INDEX:
        return "bad index";
    case M_MESSAGE_NOT_FOUND:
        return "message is not found";
    case M_RELEASED:
        return "object reference is released, but accessed";
    case M_BAD_TIMER_ID:
        return "bad timer id";
    case M_BAD_STREAM:
        return "bad stream";
    case M_BAD_ADDRESS:
        return "bad access";
    case M_NO_MEMORY:
        return "no memory";
    case M_FILE_NOT_FOUND:
        return "file not found";
    case M_NO_SPACE:
        return "no space";
    case M_READ_ERROR:
        return "read error";
    case M_WRITE_ERROR:
        return "write error";
    case M_BAD_OFFSET:
        return "bad offset";
    case M_BAD_FILE_ID:
        return "bad file id";
    case M_NOT_SUPPORTED:
        return "not supported";
    case M_UNKNOWN:
        return "unknown error";
    case M_BAD_ARG:
        return "bad arguments";
    case M_OBJECT_NOT_FOUND:
        return "object not found";
    default:
        return "BUG: unknown error";
    }
}