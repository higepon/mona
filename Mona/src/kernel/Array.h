#ifndef _MONA_ARRAY_
#define _MONA_ARRAY_

#include "sys/types.h"

#define FOREACH(type, iterator, array) \
        if ((array).GetLength() > 0) \
                for ({int __##iterator = 0; type iterator;} \
                        __##iterator < (array).GetLength() && (&(iterator = (array)[__##iterator]) || true); __##iterator++)

/*----------------------------------------------------------------------
    Array
----------------------------------------------------------------------*/
template <class T> class Array
{
  public:
    Array(dword length) : length(length), alloc(true)
    {
        this->array = new T[length];
    }

    Array(T* array, dword length) : array(array), length(length), alloc(false)
    {
    }

    virtual ~Array()
    {
        if (this->alloc)
        {
            delete[] this->array;
        }
    }

  public:
    T& operator [](dword index)
    {
#if 1
        if (index < 0 || index >= length)
        {
            g_console->printf("array index outof range %d\n", index);
            panic("array");
        }
#endif
        return this->array[index];
    }

    int GetLength() const
    {
        return this->length;
    }

  private:
    T* array;
    dword length;
    bool alloc;
};

#endif
