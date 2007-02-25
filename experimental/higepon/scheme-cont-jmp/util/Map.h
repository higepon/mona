#ifndef __UTIL_MAP_H__
#define __UTIL_MAP_H__

namespace util {

/*----------------------------------------------------------------------
    Map Interface
----------------------------------------------------------------------*/
#ifdef USE_BOEHM_GC
template <class T> class Map : public gc_cleanup
#else
template <class T> class Map
#endif
{
public:
    virtual ~Map() {}

public:
    virtual bool containsKey(const char* key) = 0;
    virtual T get(const char* key)            = 0;
    virtual T put(const char* key, T value)   = 0;
    virtual T remove(const char* key)         = 0;
    virtual int size()                  const = 0;
};
};

#endif
