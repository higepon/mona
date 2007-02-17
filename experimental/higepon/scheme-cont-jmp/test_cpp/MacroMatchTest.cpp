#include "MacroMatchTest.h"
#include <Macro.h>

CPPUNIT_TEST_SUITE_REGISTRATION(MacroMatchTest);

using namespace monash;
using namespace monash::util;

void MacroMatchTest::setUp()
{
}

void MacroMatchTest::tearDown()
{
}

void MacroMatchTest::assertMacroMatch(const util::String& macroName, const util::String& words, const util::String& macro, const util::String& target, bool matchOrNot)
{
    SExp* m = SExp::fromString(macro);
    SExp* t = SExp::fromString(target);
    SExp* r = SExp::fromString(words);
    strings ss;
    for (int i = 0; i < r->sexps.size(); i++)
    {
        ss.add(r->sexps[i]->text);
    }
    util::String msg(macro);
    msg += matchOrNot ? " matches " : " not match ";
    msg += target;
    bool result = Macro::match(macroName, ss, m, t) == matchOrNot;
    if (!result && matchOrNot) msg += ":" + Macro::error;
    CPPUNIT_ASSERT_MESSAGE(msg.data(), result);
}

void MacroMatchTest::testMatch()
{
    YAML yaml;
    loadYAML("test_cpp/match.yml", yaml);
    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }

    for (int i = 0; i < yaml.size(); i++)
    {
        strings* s = yaml[i];
        if (s->size() != 4)
        {
            fprintf(stderr, "bad yaml!\n");
        }
        assertMacroMatch(s->get(0).data(), s->get(1).data(), s->get(2).data(), s->get(3).data());
    }
}

void MacroMatchTest::testUnmatch()
{
    YAML yaml;
    loadYAML("test_cpp/unmatch.yml", yaml);
    if (yaml.size() == 0)
    {
        fprintf(stderr, "bad yaml!\n");
    }
    for (int i = 0; i < yaml.size(); i++)
    {
        strings* s = yaml[i];
        if (s->size() != 4)
        {
            fprintf(stderr, "bad yaml!\n");
        }
        assertMacroMatch(s->get(0).data(), s->get(1).data(), s->get(2).data(), s->get(3).data(), false);
    }
}
