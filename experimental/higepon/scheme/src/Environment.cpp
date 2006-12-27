
#include "Environment.h"

using namespace monash;
using namespace std;

Environment::Environment()
{
    frames_ = new Frames();
    ASSERT(frames_);
    Frame* f = new Frame();
    ASSERT(f);
    frames_->push_back(f);
}

Environment::~Environment()
{
//    delete frames_;
}

Environment* Environment::clone()
{
    Environment* env = new Environment();ASSERT(env);
    Frames* target = env->frames();
    for (Frames::iterator it = frames_->begin(); it != frames_->end(); ++it)
    {
        target->push_back(*it);
    }
    return env;
}

void Environment::setVaribale(Variable* variable, Object* value)
{
    for (Frames::iterator it = frames_->begin(); it != frames_->end(); ++it)
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
//     Frames::iterator firstFrame = frames_->begin();
//     printf("define %s \n",  variable->name().c_str());
//     (*firstFrame)->insert(variable, value); insert or overwrite
    Frame* lastFrame = frames_->at(frames_->size() -1);
    lastFrame->insert(variable, value); // insert or overwrite
    return;
}

void Environment::extend(Variables* variables, Objects* objects)
{
    Frame* f = new Frame(variables, objects);
    ASSERT(f);
    frames_->push_back(f);
}

Object* Environment::lookupVariableValue(Variable* variable)
{
//     for (Frames::iterator frame = frames_->begin(); frame != frames_->end(); frame++)
//     {
//         Object* found = (*frame)->lookup(variable);
//         if (NULL != found) return found;
//     }
    int size = frames_->size();
//    printf("size=%d\n",frames_->size());
    for (int i = size - 1; i >= 0; i--)
    {
        Frame* frame = frames_->at(i);
        Object* found = frame->lookup(variable);
        if (NULL != found) {
//            printf("%s found %d type=%d\n", variable->toString().c_str(), i, found->type());
            return found;
        }
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
