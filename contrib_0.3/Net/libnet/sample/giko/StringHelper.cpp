#include "StringHelper.h"

using namespace std;

StringHelper::StringHelper()
{
}

StringHelper::~StringHelper()
{
}

vector<string> StringHelper::split(const string& text)
{
    vector<string> result;
    unsigned int start = 0;
    for (;;)
    {
        unsigned int found = text.find("\n", start);
        if (found == string::npos) break;
        if (found == start)
        {
            start++;
            continue;
        }
        string line = text.substr(start, found - start);
        start = found + 1;
        result.push_back(line);
    }
    return result;
}

string StringHelper::join(const string& sep, vector<string> strings)
{
    string result;
    for (unsigned int i = 0; i < strings.size(); i++)
    {
        result += strings[i] + sep;
    }
    return result;
}

string StringHelper::removeHTMLTags(const string& text)
{
    unsigned int start = 0;
    string result = text;
    for (;;)
    {
        unsigned int tagStart = result.find('<', start);
        if (tagStart == string::npos) break;
        unsigned int  tagEnd   = result.find('>', tagStart);
        if (tagEnd == string::npos) break;
        result.erase(tagStart, tagEnd - tagStart + 1);
        start = tagStart;
    }
    return result;
}

std::string StringHelper::removeDoubleBreakLines(const std::string& text)
{
    unsigned int start = 0;
    string result = text;
    for (;;)
    {
        unsigned int found = result.find("\n\n");
        if (found == string::npos) break;
        result.replace(found, 2, "\n");
        start = found;
    }
    start = 0;
    for (;;)
    {
        unsigned int found = result.find("  ");
        if (found == string::npos) break;
        result.replace(found, 2, " ");
        start = found;
    }
    start = 0;
    for (;;)
    {
        unsigned int found = result.find(" \n");
        if (found == string::npos) break;
        result.erase(found, 2);
        start = found;
    }
    return result;
}
