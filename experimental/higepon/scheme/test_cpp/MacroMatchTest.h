#ifndef _MACRO_MATCH_TEST_H_
#define _MACRO_MATCH_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

class MacroMatchTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(MacroMatchTest);
    CPPUNIT_TEST(testNormalMatch);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testNormalMatch();

private:
};

#endif // _MACRO_MATCH_TEST_H_
