#include "LetAsterisk.h"

using namespace monash;

LetAsterisk::LetAsterisk(Objects* body, Variables* variables, Objects* values, uint32_t lineno) : body_(body)
                                                                                                , variables_(variables)
                                                                                                , values_(values)
                                                                                                , lineno_(lineno)
{
}

LetAsterisk::~LetAsterisk()
{
}

std::string LetAsterisk::toString()
{
    return std::string("let");
}

int LetAsterisk::type() const
{
    return Object::LET_ASTERISK;
}

Object* LetAsterisk::expand()
{
    Variables::iterator variablesIt = variables_->begin();
    Objects::iterator valuesIt = values_->begin();
    Variables* variables = new Variables;ASSERT(variables);
    variables->push_back(*variablesIt);
    Objects* values = new Objects;ASSERT(values);
    values->push_back(*valuesIt);
    Object* let = new Let(expandInternal(++variablesIt, ++valuesIt), variables, values); ASSERT(let); return let;
}

Objects* LetAsterisk::expandInternal(Variables::iterator variablesIt, Objects::iterator valuesIt)
{
    if (variablesIt == variables_->end() || valuesIt == values_->end())
    {
        return body_;
    }
    Variables* variables = new Variables;ASSERT(variables);
    variables->push_back(*variablesIt);
    Objects* values = new Objects;ASSERT(values);
    values->push_back(*valuesIt);
    Let* let = new Let(expandInternal(++variablesIt, ++valuesIt), variables, values);ASSERT(let);
    Objects* body = new Objects;ASSERT(body);
    body->push_back(let);
    return body;
}

Object* LetAsterisk::eval(Environment* env)
{
    Object* application = expand();
    return application->eval(env);
}
