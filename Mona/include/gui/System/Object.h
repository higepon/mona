// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_OBJECT_H__
#define __SYSTEM_OBJECT_H__

#include <gui/System/String.h>

namespace System
{
	class Object
	{
	private:
		int refCount;
	
	public:
		virtual String get_TypeName() { return "System.Object"; }
		virtual String ToString() { return this->get_TypeName(); }
		Object() : refCount(0) {}
		virtual ~Object() {}
		inline int get_RefCount() { return this->refCount; }
		inline int* get_RefCountPtr() { return &this->refCount; }
	};
}

#endif  // __SYSTEM_OBJECT_H__
