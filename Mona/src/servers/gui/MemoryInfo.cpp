// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include "GUIServer.h"
#include "MemoryInfo.h"
using namespace MonAPI;

MemoryInfo::MemoryInfo() : Handle(0), Size(0), Width(0), Height(0), Data(NULL)
{
}

MemoryInfo::MemoryInfo(dword handle, dword size) : Handle(handle), Size(size), Width(0), Height(0)
{
}

bool MemoryInfo::Create(dword size, bool prompt /*= false*/)
{
	this->Handle = MemoryMap::create(size);
	if (this->Handle == 0)
	{
		if (prompt) printf("ERROR\n");
		printf("%s: MemoryMap create error = %x\n", SVR, MemoryMap::getLastError());
		return false;
	}
	
	if (!this->Map())
	{
		printf("ERROR\n");
		return false;
	}
	
	this->Size = size;
	return true;
}

bool MemoryInfo::Map()
{
	this->Data = MemoryMap::map(this->Handle);
	if (this->Data != NULL) return true;
	
	printf("%s: map error\n", SVR);
	MemoryMap::unmap(this->Handle);
	this->Handle = 0;
	this->Size   = 0;
	return false;
}

void MemoryInfo::Dispose()
{
	MemoryMap::unmap(this->Handle);
}
