#ifndef __FILE_SERVER_H__
#define __FILE_SERVER_H__

#include <monapi/cmemoryinfo.h>
#include <monapi/CString.h>

#define SVR "File Server"

int ChangeDrive(int drive);
int GetCurrentDrive();
const char* GetCurrentDirectory();
int ChangeDirectory(const MonAPI::CString& dir);
MonAPI::CString getParentDirectory(const MonAPI::CString& dir);
MonAPI::CString mergeDirectory(const MonAPI::CString& dir1, const MonAPI::CString& dir2);

dword Open(const MonAPI::CString& path);
bool Seek(dword id, dword position, dword flag);
monapi_cmemoryinfo* Read(dword id, dword size);
dword GetFileSize(dword id);
bool Close(dword id);

extern monapi_cmemoryinfo* ReadFile(const char* file, bool prompt = false);
extern monapi_cmemoryinfo* ReadDirectory(const char* path, bool prompt = false);
extern void Initialize(bool bootFromCD);

#endif  // __FILE_SERVER_H__
