/*!
    \file  Map.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

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
