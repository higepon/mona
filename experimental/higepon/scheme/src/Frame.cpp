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
        map_.insert(std::pair<Variable*, Object*>(variables->at(i), values->at(i)));
    }
}

Frame::~Frame()
{
}

Object* Frame::lookup(Variable* variable)
{
    printf("%s %s:%d i wanna %s\n", __func__, __FILE__, __LINE__, variable->text.c_str());fflush(stdout);// debug
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

std::string Frame::toString()
{
    string result = "";
    for (FrameMap::iterator it = map_.begin(); it != map_.end(); it++)
    {
        result += (*it).first->toString() + ":";
        result += (*it).second->toString() + "\n";
    }
    return result;
}
