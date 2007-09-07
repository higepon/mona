/*!
    \file  ParserTest.h
    \brief 

    Copyright (c) 2006-2007 HigePon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/

#ifndef _EXT_REP_PARSER_TEST_H
#define _EXT_REP_PARSER_TEST_H

#include <scheme.h>
#include "yaml.h"
#include <cppunit/extensions/HelperMacros.h>

class ParserTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(ParserTest);
    CPPUNIT_TEST(testParser);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testParser();
};

#endif // _EXT_REP_PARSER_TEST_H
