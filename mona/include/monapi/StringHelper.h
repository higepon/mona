/*!
    \file  StringHelper.h
    \brief StringHelper

    Copyright (c) 2006 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2006/11/24 update:$Date$
*/

#ifndef _MONAPI_STRING_HELPER_
#define _MONAPI_STRING_HELPER_

// don't include from monapi.h

#include <string>
#include <vector>

namespace MonAPI {

typedef std::vector<std::string> Strings;
/*----------------------------------------------------------------------
    StringHelper
----------------------------------------------------------------------*/
class StringHelper
{
public:
    static bool endsWith(const std::string& text, const std::string& str)
    {
        std::string::size_type foundPosition = text.rfind(str);
        if (foundPosition == std::string::npos) return false;
        return (text.length()  - str.length()) == foundPosition;
    }

    static Strings split(const std::string&sep, const std::string& text)
    {
        Strings result;
        std::string::size_type sepLength = sep.length();
        unsigned int start = 0;
        for (;;)
        {
            unsigned int found = text.find(sep, start);
            if (found == std::string::npos)
            {
                std::string line = text.substr(start);
                if (line != "") result.push_back(line);
                break;
            }
            if (found == start)
            {
                start++;
                continue;
            }
            std::string line = text.substr(start, found - start);
            start = found + sepLength;
            result.push_back(line);
        }
        return result;
    }

    static std::string join(const std::string& sep, Strings strings)
    {
        std::string result;
        std::string::size_type size = strings.size();
        for (unsigned int i = 0; i < size; i++)
        {
            result += strings[i];
            if (i != size - 1)
            {
                result += sep;
            }
        }
        return result;
    }

private:
    StringHelper() {}
    virtual ~StringHelper() {}
};

} // namespace MonAPI

#endif
