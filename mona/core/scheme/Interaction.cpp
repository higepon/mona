/*!
    \file   Interaction.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "Interaction.h"

using namespace util;
using namespace monash;

// todo
extern Object* scheme_eval_string(const String& input, Environment* env, bool out /* = false */);

Interaction::Interaction(Environment* env) : env_(env)
{
    reset();
}

Interaction::~Interaction()
{
}

void Interaction::reset()
{
    input_ = "";
}

bool Interaction::onInput(const String& line)
{
    input_ += line;
    int leftParenCount  = input_.countChar('(');
    int rightParenCount = input_.countChar(')');
    if (input_ == "(\n")
    {
        SCHEME_WRITE(stdout, "\n");
        showPrompt();
        return false;
    }
    else if (leftParenCount - rightParenCount == 1)
    {
        // append ')' automatically
#ifdef MONA
        input_.chop();
        input_ += ")\n";
        SCHEME_WRITE(stdout, ")\n");
#else
        return false;
#endif
    }
    else if (leftParenCount != rightParenCount)
    {
        return false;
    }

    SCHEME_WRITE(stdout, "\n");
//    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    scheme_eval_string(input_, env_, true); // eval => print
//    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    return true;
}

void Interaction::showPrompt()
{
    reset();
    Object* o = scheme_eval_string("(mona-prompt-string)", env_, false);
    if (!o->isSString())
    {
        RAISE_ERROR(o->lineno(), "(mona-prompt-string) not defined");
        return;
    }

    SString* s = (SString*)o;

    SCHEME_WRITE(stdout, "%s", s->value().data());
    return;
}
