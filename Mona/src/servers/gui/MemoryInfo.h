// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_MEMORYINFO_H__
#define __GUISERVER_MEMORYINFO_H__

#include <monapi.h>
using namespace MonAPI;

class MemoryInfo
{
public:
	dword Handle, Size;
	int Width, Height;
	byte* Data;
	
	MemoryInfo();
	MemoryInfo(dword handle, dword size);
	
	bool Create(dword size, bool prompt = false);
	bool Map();
	void Dispose();
};

#endif  // __GUISERVER_MEMORYINFO_H__
