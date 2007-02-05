
#include "Environment.h"

using namespace monash;
using namespace std;

Environment::Environment(MacroFilter& filter, Translator& translator, uint32_t lineno /* = 0 */) : filter_(filter), translator_(translator), lineno_(lineno)
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
    Environment* env = new Environment(filter_, translator_);ASSERT(env);
    Frames* target = env->frames();
    for (Frames::const_iterator it = frames_->begin(); it != frames_->end(); ++it)
    {
        target->push_back(*it);
    }
    return env;
}

void Environment::setVaribale(Variable* variable, Object* value)
{
    if (variable->name() == "puga") printf("set! %s\n", value->toString().c_str());
    for (Frames::const_reverse_iterator p = frames_->rbegin(); p != frames_->rend(); ++p)
    {
        if ((*p)->lookup(variable) != NULL)
        {
            (*p)->insert(variable, value);
            return;
        }
    }
    RAISE_ERROR(variable->lineno(), "set! unbound variable [%s]", variable->toString().c_str());
    return;
}

void Environment::defineVariable(Variable* variable, Object* value)
{
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
    for (Frames::const_reverse_iterator p = frames_->rbegin(); p != frames_->rend(); ++p)
    {
        Object* found = (*p)->lookup(variable);
        if (NULL != found) {
            return found;
        }
    }

    RAISE_ERROR(variable->lineno(), "unbound variable [%s]", variable->toString().c_str());
    return NULL;
}

std::string Environment::toString()
{
    string result = "";
    for (Frames::const_reverse_iterator p = frames_->rbegin(); p != frames_->rend(); ++p)
    {
        result += "****************\n";
        result += (*p)->toString() + "\n\n";
    }
    return result;
}

int Environment::type() const
{
    return Object::ENVIRONMENT;
}

Object* Environment::eval(Environment* env)
{
    return this;
}
