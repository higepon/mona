#include <string>
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

#define MACRO_MATCH(macroName, macro, target, matchOrNot)                                     \
{                                                                                             \
    Node* m = Macro::toNode(macro);                                                           \
    Node* t = Macro::toNode(target);                                                          \
    CPPUNIT_ASSERT_MESSAGE(matchOrNot ? macro " matches " target : macro " not match " target \
    , Macro::match(macroName, m, t) == matchOrNot);                                           \
}

#define _M(macroName, macro, target) MACRO_MATCH(macroName, macro, target, true)
#define _N(macroName, macro, target) MACRO_MATCH(macroName, macro, target, false)

void MacroMatchTest::testNormalMatch()
{
    _M("and", "(_ a b ...)", "(and a b c)");
    _M("and", "(_ a)"      , "(and a b)");
}

