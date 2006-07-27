#include "StringToNumber.h"

using namespace monash;

StringToNumber::StringToNumber()
{
}

StringToNumber::~StringToNumber()
{
}

std::string StringToNumber::toString()
{
    return "procedure:number?";
}

Object* StringToNumber::eval(Environment* env)
{
    printf("don't eval me");
    return NULL;
}

Object* StringToNumber::apply(Objects* arguments)
{
    if (arguments->size() != 1)
    {
        printf("string->number got error");
        return NULL;
    }
    Object* o = arguments->at(0);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (o->type() != Object::STRING) return new Number(0);
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    String* str = (String*)o;
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    std::string text = str->value();
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    for (int i = 0; i < text.length(); i++)
    {
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        if (!isdigit(text[i]))
        {
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
            return new Number(0);
        }
    }
    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    return new Number(atoi(text.c_str()));
}
