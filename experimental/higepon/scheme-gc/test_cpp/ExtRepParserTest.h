#ifndef _EXT_REP_PARSER_TEST_H
#define _EXT_REP_PARSER_TEST_H

#include <scheme.h>
#include "yaml.h"
#include <cppunit/extensions/HelperMacros.h>

class ExtRepParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(ExtRepParserTest);
    CPPUNIT_TEST(testParser);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testParser();
};

#endif // _EXT_REP_PARSER_TEST_H
