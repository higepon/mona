// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_MEMORYINFO_H__
#define __GUISERVER_MEMORYINFO_H__

#include <monapi.h>
using namespace MonAPI;

class MemoryInfo
{
public:
	dword Handle;
	int Size, Width, Height;
	byte* Data;
	
	MemoryInfo();
	bool Create(int size, bool prompt = false);
	void Dispose();
};

#endif  // __GUISERVER_MEMORYINFO_H__
