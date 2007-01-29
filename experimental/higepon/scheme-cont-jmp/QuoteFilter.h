#ifndef __QUOTE_FILTER_H__
#define __QUOTE_FILTER_H__

#include <string>

namespace monash {

class QuoteFilter
{
public:
    QuoteFilter();
    virtual ~QuoteFilter();
    std::string filter(const std::string& text);

protected:
    char getChar();
    std::string getQuoteString(std::string::size_type paren);
    std::string& replace(std::string& str, const std::string& t, const std::string& s);
    std::string input_;
    std::string::size_type postion_;
};

}; // namespace monash

#endif // __QUOTE_FILTER_H__
