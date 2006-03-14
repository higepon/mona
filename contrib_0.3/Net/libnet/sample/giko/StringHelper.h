#ifndef __STRINGHELPER_H__
#define __STRINGHELPER_H__

#include <string>
#include <vector>

class StringHelper
{
private:
    StringHelper();
    virtual ~StringHelper();

public:
    static std::vector<std::string> split(const std::string& text);
    static std::string join(const std::string& sep, std::vector<std::string> strings);
    static std::string removeHTMLTags(const std::string& text);
    static std::string removeDoubleBreakLines(const std::string& text);
};

#endif // __STRINGHELPER_H__
