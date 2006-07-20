#include "Environment.h"

using namespace monash;
using namespace std;

Environment::Environment()
{
    frames_ = new Frames();
    frames_->push_back(new Frame());
}

Environment::~Environment()
{
    delete frames_;
}

void Environment::setVaribale(Variable* variable, Object* value)
{
    for (Frames::iterator it = frames_->begin(); it != frames_->end(); it++)
    {
        if ((*it)->lookup(variable) != NULL)
        {
            (*it)->insert(variable, value);
            return;
        }
    }
    printf("unbound variable\n");
    return;
}

void Environment::defineVariable(Variable* variable, Object* value)
{
    Frames::iterator firstFrame = frames_->begin();
    (*firstFrame)->insert(variable, value); // insert or overwrite
    return;
}

void Environment::extend(Variables* variables, Objects* objects)
{
    frames_->push_back(new Frame(variables, objects));
}

Object* Environment::lookupVariableValue(Variable* variable)
{
    for (Frames::iterator frame = frames_->begin(); frame != frames_->end(); frame++)
    {
        Object* found = (*frame)->lookup(variable);
        if (NULL != found) return found;
    }

    printf("unbound variable %s \n", variable->toString().c_str());
    return NULL;
}

std::string Environment::toString()
{
    string result = "";
    for (Frames::iterator frame = frames_->begin(); frame != frames_->end(); frame++)
    {
        result += (*frame)->toString() + "\n\n";
    }
    return result;
}
