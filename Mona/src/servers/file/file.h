#ifndef __FILE_SERVER_FILE_H__
#define __FILE_SERVER_FILE_H__

#include <monapi/cmemoryinfo.h>

extern monapi_cmemoryinfo* ReadFile(const char* file, bool prompt = false);
extern monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt = false);

#endif  // __FILE_SERVER_FILE_H__
