// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include "GUIServer.h"
#include "file.h"

using namespace MonAPI;

MemoryInfo* ReadFile(const char* file, bool prompt /*= false*/)
{
	if (prompt) printf("%s: Reading %s....", SVR, file);
	FileInputStream fis(file);
	if (fis.open() != 0)
	{
		if (prompt) printf("ERROR\n");
		return NULL;
	}
	
	MemoryInfo* ret = new MemoryInfo();
	if (!ret->Create(fis.getFileSize(), prompt))
	{
		delete ret;
		return NULL;
	}
	
	fis.read(ret->Data, ret->Size);
	fis.close();
	if (prompt) printf("OK\n");
	return ret;
}
