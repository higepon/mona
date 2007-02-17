#include "Frame.h"

using namespace monash;
using namespace std;

Frame::Frame()
{
}

Frame::Frame(Variables* variables, Objects* values)
{
    int size = variables->size() > values->size() ? variables->size() : values->size();
    for (int i = 0; i < size; i++)
    {
        map_.put(variables->get(i)->name().c_str(), values->get(i));
    }
}

Frame::~Frame()
{
}

Object* Frame::lookup(Variable* variable)
{
    return map_.get(variable->name().c_str());
}

void Frame::insert(Variable* variable, Object* value)
{
    Object* found = map_.get(variable->name().c_str());
    if (found != NULL)
    {
        map_.remove(variable->name().c_str());
    }
    map_.put(variable->name().c_str(), value);
}

void Frame::remove(Variable* variable)
{
    map_.remove(variable->name().c_str());
}

std::string Frame::toString()
{
    string result = "";
// todo fix me
//     for (FrameMap::const_iterator it = map_.begin(); it != map_.end(); ++it)
//     {
//         result += (*it).first + ":";
//         result += (*it).second->toString() + "\n";
//     }
    return result;
}
