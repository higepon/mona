// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_ARRAY_H__
#define __SYSTEM_ARRAY_H__

#define _A ::System::Array

#ifdef DEBUG
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#define STACK_ARRAY(type, name, length) type __##name[length]; _A<type> name(__##name, length, false)
#else
#define STACK_ARRAY(type, name, length) type name[length]
#endif

#define FOREACH(type, iterator, array) \
	if ((array).get_Length() > 0) { type iterator; \
		for (int __##iterator = 0; \
			__##iterator < (array).get_Length() && (&(iterator = (array)[__##iterator]) || true); __##iterator++)
#ifndef END_FOREACH
#define END_FOREACH }
#endif

namespace System
{
	template <class T> struct Array
	{
	protected:
		T* pointer;
		int length;
		int* refCount;

	private:
		inline void Initialize()
		{
			this->pointer  = 0 /*NULL*/;
			this->length   = 0;
			this->refCount = 0 /*NULL*/;
		}

	public:
		Array()
		{
			this->Initialize();
		}

		Array(int length)
		{
			this->Initialize();
			this->Alloc(length);
		}

		Array(T* pointer, int length, bool isManaged = true)
		{
			this->Initialize();
			this->Set(pointer, length, isManaged);
		}

		Array(const Array<T>& array)
		{
			this->Initialize();
			this->Set(array);
		}

		virtual ~Array()
		{
			this->Unset();
		}

		void Alloc(int length)
		{
			this->Unset();
			this->Set(new T[length], length);
		}

		void Set(T* pointer, int length, bool isManaged = true)
		{
			this->Unset();

			this->pointer = pointer;
			this->length = length;
#ifdef DEBUG
			if (this->pointer == 0 /*NULL*/ && this->length > 0)
			{
				::printf("ERROR: [Array] out of memory\n");
				::exit(1);
			}
#endif

			if (this->pointer != 0 /*NULL*/ && isManaged) this->refCount = new int(1);
		}

		void Set(const Array<T>& array)
		{
			this->Unset();

			this->pointer = array.pointer;
			this->length = array.length;
#ifdef DEBUG
			if (this->pointer == 0 /*NULL*/ && this->length > 0)
			{
				::printf("ERROR: [Array] out of memory\n");
				::exit(1);
			}
#endif

			this->refCount  = array.refCount;
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
					delete this->refCount;
					this->refCount = 0 /*NULL*/;
					delete [] this->pointer;
				}
				else
				{
					this->refCount = 0 /*NULL*/;
				}
				this->pointer = 0 /*NULL*/;
				this->length = 0;
			}
		}

		inline T* get() { return this->pointer; }
		inline int get_Length() const { return this->length; }
		inline int get_RefCount() const { return this->refCount != 0 /*NULL*/ ? *this->refCount : -1; }

		inline bool operator ==(T* arg) const { return this->pointer == arg; }
		inline bool operator !=(T* arg) const { return this->pointer != arg; }
		inline bool operator ==(const Array<T>& arg) const { return this->pointer == arg.pointer; }
		inline bool operator !=(const Array<T>& arg) const { return this->pointer != arg.pointer; }

		inline Array<T>& operator =(const Array<T>& pointer)
		{
			this->Set(pointer);
			return *this;
		}

		inline T& operator [](int index)
		{
#ifdef DEBUG
			if (this->pointer == 0 /*NULL*/)
			{
				::printf("ERROR: [Array] null reference exception\n");
				::exit(1);
			}
			else if (index < 0 || this->length - 1 < index)
			{
				::printf("ERROR: [Array] out of range (%d/%d)\n", index, this->length);
				::exit(1);
			}
#endif
			return this->pointer[index];
		}

		inline T GetValue(int index) const
		{
#ifdef DEBUG
			if (this->pointer == 0 /*NULL*/)
			{
				::printf("ERROR: [Array] null reference exception\n");
				::exit(1);
			}
			else if (index < 0 || this->length - 1 < index)
			{
				::printf("ERROR: [Array] out of range (%d/%d)\n", index, this->length);
				::exit(1);
			}
#endif
			return this->pointer[index];
		}
	};
}

#endif  // __SYSTEM_ARRAY_H__
