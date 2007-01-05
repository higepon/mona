#ifndef _MACRO_FILTER_TEST_H_
#define _MACRO_FILTER_TEST_H_

#include <scheme.h>
#include "yaml.h"
#include <cppunit/extensions/HelperMacros.h>

class MacroFilterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(MacroFilterTest);
    CPPUNIT_TEST(testFilter);
    CPPUNIT_TEST(testFindDefineSyntaxes);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testFilter();
    void testFindDefineSyntaxes();
};

#endif // _MACRO_FILTER_TEST_H_
