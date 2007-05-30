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

void Interaction::onInput(const String& line)
{
    input_ += line;
    int leftParenCount  = input_.countChar('(');
    int rightParenCount = input_.countChar(')');
    if (input_ == "(\n")
    {
        SCHEME_WRITE(stdout, "\n");
        showPrompt();
        return ;
    }
    else if (leftParenCount - rightParenCount == 1)
    {
        // append ')' automatically
#ifdef MONA
        input_.chop();
        input_ += ")\n";
        SCHEME_WRITE(stdout, ")\n");
#else
        return;
#endif
    }
    else if (leftParenCount != rightParenCount)
    {
        return;
    }

    // あとで Scheme / eval /print に書き換える
    SCHEME_WRITE(stdout, "\n");
    scheme_eval_string(input_, env_, true); // eval => print
    showPrompt();
    return;
}

void Interaction::showPrompt()
{
    reset();

    // 後で書き換える
    Object* o = scheme_eval_string("(mona-prompt-string)", env_, false);
    if (!o->isSString())
    {
        RAISE_ERROR(o->lineno(), "(mona-prompt-string) not defined");
        return;
    }

    SString* s = (SString*)o;

    // あとで書き換える
    SCHEME_WRITE(stdout, "%s", s->value().data());
    return;
}
