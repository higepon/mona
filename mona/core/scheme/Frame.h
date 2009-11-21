/*!
    \file  Frame.h
    \brief 

    Copyright (c) 2006-2007 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef __FRAME_H__
#define __FRAME_H__

//#include <stlport/hash_map>
#include "util/HashMap.h"
#include "Variable.h"

namespace monash {

#ifdef USE_BOEHM_GC
class Frame : public gc_cleanup
//class Frame : public gc
#else
class Frame
#endif
{
public:
    Frame();
    Frame(Variables* variables, Objects* values);
    virtual ~Frame();

    Object* lookup(Variable* variable);
    void insert(Variable* variable, Object* value);
    void remove(Variable* variable);
    ::util::String toString();
protected:
    typedef ::util::HashMap<Object*> FrameMap;
    FrameMap map_;
};

}; // namespace monash

#endif // __FRAME_H__
