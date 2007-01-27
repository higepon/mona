#ifndef _QUOTE_FILTER_TEST_H_
#define _QUOTE_FILTER_TEST_H_

#include <scheme.h>
#include "yaml.h"
#include <cppunit/extensions/HelperMacros.h>

class QuoteFilterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(QuoteFilterTest);
    CPPUNIT_TEST(testFilter);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testFilter();
};

#endif // _QUOTE_FILTER_TEST_H_
