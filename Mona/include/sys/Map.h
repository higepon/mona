#ifndef _MONA_MAP_
#define _MONA_MAP_

#include <sys/types.h>

/*----------------------------------------------------------------------
    Map Interface
----------------------------------------------------------------------*/
template <class T> class Map {
public:
    virtual ~Map() {}

public:
    virtual bool containsKey(const char* key) = 0;
    virtual T get(const char* key)            = 0;
    virtual T put(const char* key, T value)   = 0;
    virtual T remove(const char* key)         = 0;
    virtual int size()                  const = 0;
};

#endif
