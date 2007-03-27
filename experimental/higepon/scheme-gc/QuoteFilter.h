#ifndef __QUOTE_FILTER_H__
#define __QUOTE_FILTER_H__

#include "util/String.h"

namespace monash {

class QuoteFilter
{
public:
    QuoteFilter();
    virtual ~QuoteFilter();
    ::util::String filter(const ::util::String& text);

protected:
    char getChar();
    void unGetChar();
    ::util::String getQuoteString(uint32_t paren);
    ::util::String input_;
    uint32_t postion_;
};

}; // namespace monash

#endif // __QUOTE_FILTER_H__
