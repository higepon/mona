// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_POINTER_H__
#define __SYSTEM_POINTER_H__

#define _P ::System::Pointer

#ifdef DEBUG
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#define __CHECK_NULL if (this->pointer == NULL) { \
	::printf("ERROR: [Pointer] null reference exception\n"); ::exit(1); }
#else
#define __CHECK_NULL
#endif

#include <gui/System/Object.h>

namespace System
{
	template <class T> struct Pointer
	{
	private:
		T* pointer;
		int* refCount;
		bool hasRefCount;
	
		inline void Initialize()
		{
			this->pointer  = 0 /*NULL*/;
			this->refCount = 0 /*NULL*/;
			this->hasRefCount = false;
		}
	
	public:
		Pointer()
		{
			this->Initialize();
		}
	
		Pointer(T* pointer, bool isManaged)
		{
			this->Initialize();
			this->Set(pointer, isManaged);
		}
	
		Pointer(const Pointer<T>& pointer)
		{
			this->Initialize();
			this->Set(pointer);
		}
		
		Pointer(Object* pointer)
		{
			this->Initialize();
			this->Set(pointer);
		}
	
		~Pointer()
		{
			this->Unset();
		}
		
		void Set(Object* pointer)
		{
			this->Unset();
			
			this->pointer = (T*)pointer;
			if (this->pointer == 0 /*NULL*/) return;
			
			this->refCount = pointer->get_RefCountPtr();
			(*this->refCount)++;
		}
	
		void Set(T* pointer, bool isManaged)
		{
			this->Unset();
	
			this->pointer = pointer;
			if (this->pointer != 0 /*NULL*/ && isManaged)
			{
				this->refCount = new int(1);
				this->hasRefCount = true;
			}
		}
	
		void Set(const Pointer<T>& pointer)
		{
			this->Unset();
	
			this->pointer  = pointer.pointer;
			this->refCount = pointer.refCount;
			this->hasRefCount = pointer.hasRefCount;
			if (this->refCount != 0 /*NULL*/) (*this->refCount)++;
		}
	
		void Clear()
		{
			this->Initialize();
		}
	
		void Unset()
		{
			if (this->refCount != 0 /*NULL*/)
			{
				(*this->refCount)--;
				if (*this->refCount < 1)
				{
					if (this->hasRefCount) delete this->refCount;
					this->refCount = 0 /*NULL*/;
					delete this->pointer;
				}
				else
				{
					this->refCount = 0 /*NULL*/;
				}
				this->pointer = 0 /*NULL*/;
			}
		}
	
		inline T* operator ->() { __CHECK_NULL return this->pointer; }
		inline T* get() { return this->pointer; }
		inline int get_RefCount() const { return this->refCount != 0 /*NULL*/ ? *this->refCount : -1; }
	
		inline bool operator ==(T* arg) const { return this->pointer == arg; }
		inline bool operator !=(T* arg) const { return this->pointer != arg; }
		inline bool operator ==(const Pointer<T>& arg) const { return this->pointer == arg.pointer; }
		inline bool operator !=(const Pointer<T>& arg) const { return this->pointer != arg.pointer; }
	
		inline Pointer<T>& operator =(Object* pointer)
		{
			this->Set(pointer);
			return *this;
		}
	
		inline Pointer<T>& operator =(const Pointer<T>& pointer)
		{
			this->Set(pointer);
			return *this;
		}
	};
}

#endif  // __SYSTEM_POINTER_H__
