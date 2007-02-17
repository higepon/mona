#include "QuoteFilter.h"

using namespace monash;
using namespace monash::util;

QuoteFilter::QuoteFilter()
{
}

QuoteFilter::~QuoteFilter()
{
}

::util::String QuoteFilter::filter(const ::util::String& text)
{
    postion_ = 0;
    input_ = text;
    input_ = replace(input_, "#(", "(vector "); // ugly
    ::util::String ret;
    char c;
    for (;;)
    {
        c = getChar();
        if (c == EOF)
        {
            break;
        }
        else if (c == ';')
        {
            ret += c;
            for (;;)
            {
                c = getChar();
                ret += c;
                if (c == '\n') break;
            }
        }
        else if (c == '\"')
        {
            ret += c;
            for (;;)
            {
                c = getChar();
                ret += c;
                if (c == '\"') break;
            }
        }
        else if (c == '\'')
        {
            ::util::String quoteString = getQuoteString(0);
            ret += "(quote " + quoteString + ")";
        }
        else
        {
            ret += c;
        }
    }
    return ret;
}

::util::String QuoteFilter::getQuoteString(uint32_t paren)
{
    ::util::String ret;
    char c;
    for (;;)
    {
        c = getChar();
        if (c == EOF)
        {
            break;
        }
        else if (c == '(')
        {
            ret += c;
            ret += getQuoteString(paren + 1);
        }
        else if (c == ')')
        {
            ret += c;
            break;
        }
        else if (isspace(c) && paren == 0)
        {
            break;
        }
        else if (isspace(c) && paren != 0)
        {
            ret += c;
        }
        else
        {
            ret += c;
        }
    }
    return ret;
}

char QuoteFilter::getChar()
{
    if (input_.size() <= postion_) return EOF;
    int c = input_[postion_];
    postion_++;
    return c;

}

::util::String& QuoteFilter::replace(::util::String& str, const ::util::String& t, const ::util::String& s)
{
    ::util::String::size_type pos;
    ::util::String::size_type current = 0;

    while ((pos = str.find(t, current)) != ::util::String::npos)
    {
        str.replace(pos, t.size(), s);
        current = pos + s.size();
    }
    return str;
}
