#include <string>
#include <fstream>
#include "MacroMatchTest.h"
#include <Macro.h>

CPPUNIT_TEST_SUITE_REGISTRATION(MacroMatchTest);

using namespace monash;
using namespace std;

typedef vector<string> Strings;
typedef vector<Strings*> YAML;

void loadYAML(const string& path, YAML& yaml)
{
    ifstream ifs(path.c_str());
    string line;

    uint32_t index = -1;
    while (getline(ifs, line))
    {
        if (line[0] == '#')
        {
            continue;
        }
        else if (line[0] == '-')
        {
            Strings* strings = new Strings;;
            index++;
            yaml.push_back(strings);

        }
        else if (line[1] == '-')
        {
            Strings* strings = yaml.at(index);
            if (NULL == strings)
            {
                fprintf(stderr, "unknown yaml type %s\n", path.c_str());
                exit(-1);
            }
            strings->push_back(line.substr(3, line.size()));
        }
    }
}

void MacroMatchTest::setUp()
{
}

void MacroMatchTest::tearDown()
{
}

#define MACRO_MATCH(macroName, macro, target, matchOrNot)                                     \
{                                                                                             \
    Node* m = Macro::toNode(macro);                                                           \
    Node* t = Macro::toNode(target);                                                          \
    string msg(macro);                                                                        \
    msg += matchOrNot ? " matches " : " not match ";                                          \
    msg += target;                                                                            \
    CPPUNIT_ASSERT_MESSAGE(msg.c_str(), Macro::match(macroName, m, t) == matchOrNot);         \
}

#define _Y(macroName, macro, target) MACRO_MATCH(macroName, macro, target, true)
#define _N(macroName, macro, target) MACRO_MATCH(macroName, macro, target, false)

void MacroMatchTest::testNormalMatch()
{
    YAML yaml;
    loadYAML("match_ok.yml", yaml);

    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        Strings* s = (*it);
        if (s->size() != 3)
        {
            fprintf(stderr, "bad yaml!\n");
        }
        _Y(s->at(0).c_str(), s->at(1).c_str(), s->at(2).c_str());
    }
}

void MacroMatchTest::testNotMatch()
{
//     _Y("and", "(_ a b ...)", "(and a b c)");
//     _N("and", "(_ a)"      , "(and a b)");
//     _N("and", "(_ a)"      , "(and a b)");

}
