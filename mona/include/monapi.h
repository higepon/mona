#ifndef _MONAPI_
#define _MONAPI_

#include <sys/types.h>
#include <sys/error.h>
#include <monapi/Observer.h>
#include <monapi/Date.h>
#include <monapi/Screen.h>
#include <monapi/Server.h>
#include <monapi/System.h>
#include <monapi/Message.h>
#include <monapi/BufferReceiver.h>
#include <monapi/messages.h>
#include <monapi/MonaApplication.h>
#include <monapi/MemoryMap.h>
#include <monapi/Thread.h>
#include <monapi/Mutex.h>
#include <monapi/Condition.h>
#include <monapi/string.h>
#include <monapi/MemoryManager.h>
#include <monapi/Color.h>
#include <monapi/Random.h>
#include <monapi/syscall.h>
#include <monapi/Keys.h>
#include <monapi/Stream.h>
#include <monapi/Assert.h>
#include <monapi/DebuggerService.h>
#include <monapi/Encoding.h>
#include <monapi/scoped_ptr.h>
#include <monapi/SharedMemory.h>

#define monapi_fatal(...) {PsInfo psinfo; MonAPI::System::getProcessInfo(&psinfo); _logprintf("Fatal (%s:%s:%d) : ", psinfo.name, __FILE__, __LINE__), _logprintf(__VA_ARGS__), _printf("Fatal (%s:%s:%d) :", psinfo.name, __FILE__, __LINE__), _printf(__VA_ARGS__), exit(-1); }
#define monapi_warn(...) { PsInfo psinfo; MonAPI::System::getProcessInfo(&psinfo); _logprintf("Warning (%s:%s:%d) : ", psinfo.name, __FILE__, __LINE__), _logprintf(__VA_ARGS__), _logprintf("\n"); }

extern "C" __attribute__((constructor)) void monapi_initialize();


#endif

