// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_COLLECTIONS_ARRAYLIST_H__
#define __SYSTEM_COLLECTIONS_ARRAYLIST_H__

#ifdef DEBUG
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#endif

#include <gui/System/Object.h>

#define FOREACH_AL(type, iterator, arrayList) \
	if ((arrayList)->get_Count() > 0) {type iterator;\
		for (int __##iterator = 0; \
			__##iterator < (arrayList)->get_Count() && (&(iterator = (arrayList)->get_Item(__##iterator)) || true); __##iterator++)
#ifndef END_FOREACH_AL
#define END_FOREACH_AL }
#endif

namespace System { namespace Collections
{
	template <class T> class ArrayList : public System::Object
	{
	protected:
		static const int DefaultCapacity = 16;
		T* pointer;
		int count, capacity;
	
	private:
		inline void Initialize()
		{
			this->pointer  = new T[ArrayList::DefaultCapacity];
			this->count    = 0;
			this->capacity = ArrayList::DefaultCapacity;
		}
	
	public:
		virtual String get_TypeName() { return "System.Collections.ArrayList"; }
		
		ArrayList()
		{
			this->Initialize();
		}
	
		virtual ~ArrayList()
		{
			delete [] this->pointer;
		}
	
		void Clear()
		{
			delete [] this->pointer;
			this->Initialize();
		}
		
		virtual void Add(T item)
		{
			if (this->count == this->capacity)
			{
				this->capacity <<= 1;
				T* ptr = new T[this->capacity];
				for (int i = 0; i < this->count; i++) ptr[i] = this->pointer[i];
				delete [] this->pointer;
				this->pointer = ptr;
			}
			this->pointer[this->count] = item;
			this->count++;
		}
		
		virtual void RemoveAt(int index)
		{
			this->count--;
			for (int i = index; i < this->count; i++)
			{
				this->pointer[i] = this->pointer[i + 1];
			}
			this->pointer[this->count] = *(new T());
		}
		
		virtual void Remove(T item)
		{
			int idx = this->IndexOf(item);
			if (idx < 0) return;
			
			this->RemoveAt(idx);
		}
		
		virtual int IndexOf(T item)
		{
			for (int i = 0; i < this->count; i++)
			{
				if (this->pointer[i] == item) return i;
			}
			return -1;
		}
	
		inline T* get() { return this->pointer; }
		inline int get_Count() const { return this->count; }
		inline int get_Capacity() const { return this->capacity; }
	
		inline T& get_Item(int index)
		{
	#ifdef DEBUG
			if (index < 0 || this->count - 1 < index)
			{
				printf("ERROR: [ArrayList] out of range (%d/%d)\n", index, this->count);
				exit(1);
			}
	#endif
			return this->pointer[index];
		}
	};
}}

#endif  // __SYSTEM_COLLECTIONS_ARRAYLIST_H__
