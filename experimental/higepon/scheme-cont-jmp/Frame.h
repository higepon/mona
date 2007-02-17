#ifndef __FRAME_H__
#define __FRAME_H__

//#include <stlport/hash_map>
#include <map>
#include "util/HashMap.h"
#include "Variable.h"

namespace monash {

class Frame
{
public:
    Frame();
    Frame(Variables* variables, Objects* values);
    virtual ~Frame();

    Object* lookup(Variable* variable);
    void insert(Variable* variable, Object* value);
    void remove(Variable* variable);
    std::string toString();
protected:
    typedef util::HashMap<Object*> FrameMap;
    FrameMap map_;
};

}; // namespace monash

#endif // __FRAME_H__
