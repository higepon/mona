#include "Frame.h"

using namespace monash;
using namespace monash::util;

Frame::Frame()
{
}

Frame::Frame(Variables* variables, Objects* values)
{
    int size = variables->size() > values->size() ? variables->size() : values->size();
    for (int i = 0; i < size; i++)
    {
        map_.put(variables->get(i)->name().data(), values->get(i));
    }
}

Frame::~Frame()
{
}

Object* Frame::lookup(Variable* variable)
{
    return map_.get(variable->name().data());
}

void Frame::insert(Variable* variable, Object* value)
{
    Object* found = map_.get(variable->name().data());
    if (found != NULL)
    {
        map_.remove(variable->name().data());
    }
    map_.put(variable->name().data(), value);
}

void Frame::remove(Variable* variable)
{
    map_.remove(variable->name().data());
}

::util::String Frame::toString()
{
    ::util::String result = "";
// todo fix me
//     for (FrameMap::const_iterator it = map_.begin(); it != map_.end(); ++it)
//     {
//         result += (*it).first + ":";
//         result += (*it).second->toString() + "\n";
//     }
    return result;
}
