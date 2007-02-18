#include "SExp.h"

using namespace util;
using namespace monash;

SExp* SExp::clone()
{
    SExp* sexp = new SExp(type);
    sexp->lineno = lineno;
    if (isSExps())
    {
        for (int i = 0; i < sexps.size(); i++)
        {
            sexp->sexps.add(sexps[i]->clone());
        }
    }
    else if(isNumber())
    {
        sexp->value = value;
    }
    else
    {
        sexp->text = text;
    }
    return sexp;
}

String SExp::typeToString()
{
    char buffer[256];
    switch(type)
    {
    case NUMBER:
        sprintf(buffer, "NUMBER[%d]\n", value);
        break;
    case SYMBOL:
        sprintf(buffer, "SYMBOL[%s]\n", text.data());
        break;
    case STRING:
        sprintf(buffer, "STRING[\"%s\"]\n", text.data());
        break;
    case QUOTE:
        sprintf(buffer, "QUOTE[\'%s]\n", text.data());
        break;
    case SEXPS:
        sprintf(buffer, "SEXPS\n");
        break;
    }
    return String(buffer);
}

String SExp::typeToRawString()
{
    char buffer[256];
    buffer[0] = '\0';

    switch(type)
    {
    case NUMBER:
        sprintf(buffer, "%d", value);
        break;
    case SYMBOL:
        sprintf(buffer, "%s", text.data());
        break;
    case STRING:
        sprintf(buffer, "\"%s\"", text.data());
        break;
    case CHAR:
        sprintf(buffer, "%s", text.data());
        break;
    case QUOTE:
        sprintf(buffer, "\'%s", text.data());
        break;
//     case SEXPS:
//         sprintf(buffer, "");
//         break;
    }
    return String(buffer);
}

void SExp::extractBindings(SExp* m, SExp* n, BindMap& bindMap)
{
    if (m->isSymbol())
    {
        BindObject* b = new BindObject;
        b->sexp = n;
        bindMap.put(m->text.data(), b);
        return;
    }
    else if (m->isSExps() && n->isSExps())
    {
        int nLength = n->sexps.size();
        int mLength = m->sexps.size();
        for (int i = 0; i < m->sexps.size(); ++i)
        {
            if (i == nLength) return;
            SExp* mm = m->sexps[i];
            SExp* nn = n->sexps[i];
            if (mLength != nLength && mm->isMatchAllKeyword())
            {
                BindObject* b = new BindObject;;
                for (int j = i; j < nLength; ++j)
                {
                    b->sexps.add(n->sexps[j]);
                }
                bindMap.put(mm->text.data(), b);
                return;
            }
            else
            {
               extractBindings(mm, nn, bindMap);
           }
        }
        return;
    }
    else
    {
        RAISE_ERROR(m->lineno, "macro exception \n%s\n%s", m->toString().data(), n->toString().data());
        return;
    }
}

void SExp::print(int depth /* = 0 */)
{
    printf(toString().data());
}

void SExp::toStringInternal(uint32_t depth, String& s)
{
    for (uint32_t i = 0; i < depth; i++)
    {
        s += " ";
    }
    s += typeToString();
    depth++;
    for (int i = 0; i < sexps.size(); i++)
    {
        sexps[i]->toStringInternal(depth, s);
    }
}

String SExp::toString()
{
    String ret;
    toStringInternal(0, ret);
    return ret;
}

String SExp::toSExpString()
{
    String ret;
    toSExpStringInternal(ret);
    return ret;
}

void SExp::toSExpStringInternal(String& s)
{
    if (isSExps())
    {
        s += "(";
        for (int i = 0; i < sexps.size(); i++)
        {
            sexps[i]->toSExpStringInternal(s);
            if (i != sexps.size() - 1) s += " ";
        }
        s += ")";
    }
    else
    {
        s += typeToRawString();
    }

}

bool SExp::equals(SExp* sexp)
{
    return equalsInternal(this, sexp);
}

bool SExp::equalsInternal(SExp* m, SExp* n)
{
    if (m->type != n->type) return false;
    if (m->isSExps())
    {
        if (m->sexps.size() != n->sexps.size()) return false;
        for (int i = 0; i < m->sexps.size(); i++)
        {
            if (!equalsInternal(m->sexps[i], n->sexps[i])) return false;
        }
        return true;
    }

    switch(m->type)
    {
    case NUMBER:
        return m->value == n->value;
    case SYMBOL:
    case STRING:
    case QUOTE:
        return m->text == n->text;
    }
    return false;
}

SExp* SExp::fromString(const String& text)
{
    Tokenizer tokenizer(text);
    Parser parser(&tokenizer);
    return parser.parse();
}

int SExp::foreachSExp(SExp* root, bool (SExp::*match)() const, int (SExp::*func)(SExp* root, SExp* sexp))
{
    int ret = 0;
    if (root->isSExps())
    {
        // don't use iterator here, sexps.size() will be changed by func
        for (int i = 0; i < root->sexps.size(); i++)
        {
            SExp* sexp = root->sexps[i];
            if ((sexp->*match)())
            {
                ret += (this->*func)(root, sexp);
            }
            ret += foreachSExp(sexp, match, func);
        }
    }
    return ret;
}

int SExp::foreachSExps(SExp* root, int (SExp::*f)(SExp* root, SExp* sexp))
{
    return foreachSExp(root, &SExp::isSExps, f);
}

#if 0
int SExp::execLoadSyntax(SExp* root, SExp* sexp)
{
    if (sexp->sexps.size() == 0) return 0;
    SExp* left = sexp->sexps[0];
    if (!left->isSymbol()
        || left->text != "load"
        || sexp->sexps.size() != 2
        || sexp->sexps[1]->type != SExp::STRING) return 0;

    String path  = sexp->sexps[1]->text;
    string input = load(path.data());
    if (input == "") return 0;
    input = "(begin " + input + ")";
    int i;
    for (i = 0; i < root->sexps.size(); ++i)
    {
        if (root->sexps[i] == sexp) break;
    }
    SExp* n = SExp::fromString(input);
    root->sexps[i] = n;
    return 1;
}

int SExp::execLoadSyntaxes()
{
    while (foreachSExps(this, &SExp::execLoadSyntax));
    return 0;
}
#endif
