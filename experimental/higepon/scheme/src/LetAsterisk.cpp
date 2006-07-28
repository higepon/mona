#include "LetAsterisk.h"

using namespace monash;

LetAsterisk::LetAsterisk(Objects* body, Variables* variables, Objects* values) : body_(body), variables_(variables), values_(values)
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
    Variables* variables = new Variables;
    variables->push_back(*variablesIt);
    Objects* values = new Objects;
    values->push_back(*valuesIt);
    return new Let(expandInternal(++variablesIt, ++valuesIt), variables, values);
}

Objects* LetAsterisk::expandInternal(Variables::iterator variablesIt, Objects::iterator valuesIt)
{
    if (variablesIt == variables_->end() || valuesIt == values_->end())
    {
        return body_;
    }
    Variables* variables = new Variables;
    variables->push_back(*variablesIt);
    Objects* values = new Objects;
    values->push_back(*valuesIt);
    Let* let = new Let(expandInternal(++variablesIt, ++valuesIt), variables, values);
    Objects* body = new Objects;
    body->push_back(let);
    return body;
}

Object* LetAsterisk::eval(Environment* env)
{
    Object* application = expand();
    return application->eval(env);
}
