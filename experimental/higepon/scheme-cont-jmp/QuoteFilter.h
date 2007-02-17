#ifndef __QUOTE_FILTER_H__
#define __QUOTE_FILTER_H__

#include <string>

namespace monash {

class QuoteFilter
{
public:
    QuoteFilter();
    virtual ~QuoteFilter();
    ::util::String filter(const ::util::String& text);

protected:
    char getChar();
    ::util::String getQuoteString(uint32_t paren);
    ::util::String& replace(::util::String& str, const ::util::String& t, const ::util::String& s);
    ::util::String input_;
    uint32_t postion_;
};

}; // namespace monash

#endif // __QUOTE_FILTER_H__
