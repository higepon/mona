#ifndef __UTIL_MAP_H__
#define __UTIL_MAP_H__

namespace monash {
namespace util {

/*----------------------------------------------------------------------
    Map Interface
----------------------------------------------------------------------*/
template <class T> class Map
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
};

#endif
