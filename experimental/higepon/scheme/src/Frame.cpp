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
        map_.insert(std::pair<string, Object*>(variables->at(i)->name(), values->at(i)));
    }
}

Frame::~Frame()
{
}

Object* Frame::lookup(Variable* variable)
{
    FrameMap::iterator it = map_.find(variable->name());
    if (it == map_.end()) return NULL;
    return (*it).second;
}

void Frame::insert(Variable* variable, Object* value)
{
    map_.insert(std::pair<string, Object*>(variable->name(), value));
}

void Frame::remove(Variable* variable)
{
    map_.erase(variable->name());
}

std::string Frame::toString()
{
    string result = "";
    for (FrameMap::iterator it = map_.begin(); it != map_.end(); it++)
    {
        result += (*it).first + ":";
        result += (*it).second->toString() + "\n";
    }
    return result;
}
