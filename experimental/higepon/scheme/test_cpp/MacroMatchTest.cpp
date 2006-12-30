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

#define _Y(macroName, macro, target) MACRO_MATCH(macroName, macro, target, true)
#define _N(macroName, macro, target) MACRO_MATCH(macroName, macro, target, false)

void MacroMatchTest::testNormalMatch()
{
    _Y("and"     , "(_ a b ...)"                     , "(and a b c)");
    _Y("when"    , "(_ pred a b)"                    , "(when (= x 3) (display \"x=3\") (display \"\n\"))");
    _Y("decf"    , "(_ x) (begin (set! x (- x 1)) x)", "(decf x)");
    _Y("decf"    , "(_ x dx)"                        , "(decf x 1)");
    _Y("my-let*" , "(_ ((a b)) z ...)"               , "(my-let* ((a 99)) (p a))");
    _Y("my-let*" , "(_ ((a b) (c d) ...) z ...)"     , "(my-let* ((a 99) (b (+ 1 a)) (c (+ b 1))) (p a) (p b) (p c))");
    _Y("pp"      , "(_ a)"                           , "(pp 3)");
    _Y("pp"      , "(_ a b ...)"                     , "(pp a b c d)");
    _Y("my-if"   , "(_ a my-then b)"                 , "(my-if (= 3 3) my-then 3");
    _Y("my-if"   , "(_ a my-then b my-else c)"       , "(my-if (= 3 3) my-then 4 my-else 5)");
}

void MacroMatchTest::testNotMatch()
{
    _Y("and", "(_ a b ...)", "(and a b c)");
    _N("and", "(_ a)"      , "(and a b)");
    _N("and", "(_ a)"      , "(and a b)");

}
