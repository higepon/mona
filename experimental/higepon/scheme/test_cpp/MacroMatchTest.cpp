#include "MacroMatchTest.h"
#include <Macro.h>

CPPUNIT_TEST_SUITE_REGISTRATION(MacroMatchTest);

using namespace monash;
using namespace std;

void MacroMatchTest::setUp()
{
}

void MacroMatchTest::tearDown()
{
}

void MacroMatchTest::assertMacroMatch(const string& macroName, const string& words, const string& macro, const string& target, bool matchOrNot)
{
    SExp* m = SExp::fromString(macro);
    SExp* t = SExp::fromString(target);
    SExp* r = SExp::fromString(words);
    strings ss;
    for (SExps::iterator it = r->sexps.begin(); it != r->sexps.end(); ++it)
    {
        ss.push_back((*it)->text);
    }
    string msg(macro);
    msg += matchOrNot ? " matches " : " not match ";
    msg += target;
    bool result = Macro::match(macroName, ss, m, t) == matchOrNot;
    if (!result && matchOrNot) msg += ":" + Macro::error;
    CPPUNIT_ASSERT_MESSAGE(msg.c_str(), result);
}

void MacroMatchTest::testMatch()
{
    YAML yaml;
    loadYAML("match.yml", yaml);

    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        strings* s = (*it);
        if (s->size() != 4)
        {
            fprintf(stderr, "bad yaml!\n");
        }
        assertMacroMatch(s->at(0).c_str(), s->at(1).c_str(), s->at(2).c_str(), s->at(3).c_str());
    }
}

void MacroMatchTest::testUnmatch()
{
    YAML yaml;
    loadYAML("unmatch.yml", yaml);

    for (YAML::iterator it = yaml.begin(); it != yaml.end(); ++it)
    {
        strings* s = (*it);
        if (s->size() != 4)
        {
            fprintf(stderr, "bad yaml!\n");
        }
        assertMacroMatch(s->at(0).c_str(), s->at(1).c_str(), s->at(2).c_str(), s->at(3).c_str(), false);
    }
}
