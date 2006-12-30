#include <string>
#include "MacroMatchTest.h"
#include <MacroUtility.h>

CPPUNIT_TEST_SUITE_REGISTRATION(MacroMatchTest);

using namespace monash;
using namespace std;

void MacroMatchTest::setUp()
{
}

void MacroMatchTest::tearDown()
{
}

#define _M(macroName, macro, target)                                                         \
{                                                                                            \
    Node* m = MacroUtility::macroToNode(macro);                                              \
    Node* t = MacroUtility::macroToNode(target);                                             \
    CPPUNIT_ASSERT_MESSAGE(macro " matches " target, MacroUtility::match(macroName, m, t));  \
}

#define _N(macroName, macro, target)                                                             \
{                                                                                                \
    Node* m = MacroUtility::macroToNode(macro);                                                  \
    Node* t = MacroUtility::macroToNode(target);                                                 \
    CPPUNIT_ASSERT_MESSAGE(macro " not matche " target, !MacroUtility::match(macroName, m, t));  \
}

void MacroMatchTest::testNormalMatch()
{
    _M("and", "(_ a b ...)", "(and a b c)");
    _N("and", "(_ a)"      , "(and a b)");
}

