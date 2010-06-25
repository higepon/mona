#ifndef _MONA_USERLIB_LIST_
#define _MONA_USERLIB_LIST_

#include <sys/types.h>

/*----------------------------------------------------------------------
    List Interface
----------------------------------------------------------------------*/
template <class T> class List
{
public:
    virtual ~List() {}

public:
    virtual void add(T element)                 = 0;
    virtual bool removeAt(int index, T* found = NULL) = 0;
    virtual bool remove(T element)              = 0;
    virtual T get(int index) const              = 0;
    virtual bool isEmpty() const                = 0;
    virtual int size() const                    = 0;
    virtual bool hasElement(T element) const    = 0;
};

#endif
