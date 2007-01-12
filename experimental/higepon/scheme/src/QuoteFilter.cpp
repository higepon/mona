#include "QuoteFilter.h"

using namespace monash;
using namespace std;

QuoteFilter::QuoteFilter()
{
}

QuoteFilter::~QuoteFilter()
{
}

string QuoteFilter::filter(const string& text)
{
    postion_ = 0;
    input_ = text;
    input_ = replace(input_, "#(", "(vector "); // ugly
    string ret;
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
            string quoteString = getQuoteString(0);
            ret += "(quote " + quoteString + ")";
        }
        else
        {
            ret += c;
        }
    }
    return ret;
}

string QuoteFilter::getQuoteString(string::size_type paren)
{
    string ret;
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

string& QuoteFilter::replace(string& str, const string& t, const string& s)
{
    string::size_type pos;
    string::size_type current = 0;

    while ((pos = str.find(t, current)) != string::npos)
    {
        str.replace(pos, t.size(), s);
        current = pos + s.size();
    }
    return str;
}
