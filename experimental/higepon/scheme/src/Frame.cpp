#include "Frame.h"

using namespace monash;

Frame::Frame()
{
}

Frame::Frame(Variables* variables, Objects* values)
{
    int size = variables->size() > values->size() ? variables->size() : values->size();

    for (int i = 0; i < size; i++)
    {
        map_.insert(std::pair<Variable*, Object*>(variables->at(i), values->at(i)));
    }
}

Frame::~Frame()
{
}

Object* Frame::lookup(Variable* variable)
{
    FrameMap::iterator it = map_.find(variable);
    if (it == map_.end()) return NULL;
    return (*it).second;
}

void Frame::insert(Variable* variable, Object* value)
{
    map_.insert(std::pair<Variable*, Object*>(variable, value));
}

void Frame::remove(Variable* variable)
{
    map_.erase(variable);
}

