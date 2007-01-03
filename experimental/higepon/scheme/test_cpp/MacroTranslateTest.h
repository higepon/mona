#ifndef _MACRO_TRANSLATE_TEST_H_
#define _MACRO_TRANSLATE_TEST_H_

#include <scheme.h>
#include "yaml.h"
#include <cppunit/extensions/HelperMacros.h>

class MacroTranslateTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(MacroTranslateTest);
    CPPUNIT_TEST(testTranslate);
    CPPUNIT_TEST(testFindDefineSyntaxes);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testTranslate();
    void testFindDefineSyntaxes();
};

#endif // _MACRO_TRANSLATE_TEST_H_
