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
//     for (int i = 0; i < variables_->size(); i++)
//     {
//         printf("v = %s\n", variables_->get(i)->toString().c_str());
//     }
//     for (int i = 0; i < values_->size(); i++)
//     {
//         printf("v = %s\n", values_->get(i)->toString().c_str());
//     }

    Variables* variables = new Variables;ASSERT(variables);
    variables->add(variables_->get(0));
    Objects* values = new Objects;ASSERT(values);
    values->add(values_->get(0));
    Object* let = new Let(expandInternal(1, 1), variables, values); ASSERT(let); return let;
}

Objects* LetAsterisk::expandInternal(int variablesIndex, int valuesIndex)
{
//    printf("%d == %d, %d == %d \n", variablesIndex, variables_->size() - 1, valuesIndex, values_->size() - 1);
    if (variablesIndex == variables_->size() || valuesIndex == values_->size())
    {
        return body_;
    }
    Variables* variables = new Variables;ASSERT(variables);
    variables->add(variables_->get(variablesIndex));
    Objects* values = new Objects;ASSERT(values);
    values->add(values_->get(valuesIndex));
//    printf("(%d, %d) %s %s\n", variablesIndex, valuesIndex, variables_->get(variablesIndex)->toString().c_str(),values_->get(valuesIndex)->toString().c_str());
    Let* let = new Let(expandInternal(variablesIndex + 1, valuesIndex + 1), variables, values);ASSERT(let);
    Objects* body = new Objects;ASSERT(body);
    body->add(let);
    return body;
}

Object* LetAsterisk::eval(Environment* env)
{
    Object* application = expand();
    return application->eval(env);
}
