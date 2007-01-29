#include "Lambda.h"

using namespace monash;
using namespace std;

Lambda::Lambda(Objects* body, Variables* parameters, uint32_t lineno) : body_(body), parameters_(parameters), lineno_(lineno)
{
}

Lambda::~Lambda()
{
}

std::string Lambda::toString()
{
    return "lambda : " + body_->at(0)->toString();
}

int Lambda::type() const
{
    return Object::LAMBDA;
}

Object* Lambda::eval(Environment* env)
{
    Object* procedure = new Procedure(this, env); ASSERT(procedure); return procedure;
}

bool Lambda::eqv() const
{
    return false;
}

bool Lambda::eq() const
{
    return false;
}

Object* Lambda::getContinuation(Object* calledPoint, Environment* env)
{
    Objects::iterator target = find(body_->begin(), body_->end(), calledPoint);
    if (target == body_->end())
    {
        RAISE_ERROR(lineno(), "can't get continuation!");
    }

    Objects* continuationBody = new Objects;
    Variable* returnValue = new Variable("returnValue");
    continuationBody->push_back(returnValue);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    for (Objects::iterator p = target + 1; p != body_->end(); ++p)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        continuationBody->push_back(*p);
    }
    Variables* arguments = new Variables;
    arguments->push_back(returnValue);

    Lambda* thisCont = new Lambda(continuationBody, arguments, lineno());
    Lambda* ret = NULL;
#if 1
    if (parent != NULL)
    {
        // todo
        if (parent->isApplication())
        {
            Application* app = (Application*)parent;
            Object* c = app->getContinuation(this, env);
            if (c->isLambda())
            {
                printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
                Lambda* l = (Lambda*)c;
                MacroFilter mf;
                Translator t;
                l->body()->insert(l->body()->begin(), thisCont->eval(env));
                ret = l;
            }
        }
    }
#endif
    return ret ? ret : thisCont;
}
