// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include "GUIServer.h"
#include "MemoryInfo.h"
using namespace MonAPI;

MemoryInfo::MemoryInfo() : Handle(0), Size(0), Width(0), Height(0), Data(NULL)
{
}

bool MemoryInfo::Create(int size, bool prompt /*= false*/)
{
	this->Handle = MemoryMap::create(size);
	if (this->Handle == 0)
	{
		if (prompt) printf("ERROR\n");
		printf("%s: MemoryMap create error = %x\n", SVR, MemoryMap::getLastError());
		return false;
	}
	
	this->Data = MemoryMap::map(this->Handle);
	if (this->Data == NULL)
	{
		if (prompt) printf("ERROR\n");
		printf("%s: map error\n", SVR);
		MemoryMap::unmap(this->Handle);
		this->Handle = 0;
		return false;
	}
	
	this->Size = size;
	return true;
}

void MemoryInfo::Dispose()
{
	MemoryMap::unmap(this->Handle);
}
