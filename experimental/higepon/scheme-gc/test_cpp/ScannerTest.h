#ifndef _SCANNER_TEST_H
#define _SCANNER_TEST_H

#include <scheme.h>
#include "yaml.h"
#include <cppunit/extensions/HelperMacros.h>

class ScannerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(ScannerTest);
    CPPUNIT_TEST(testScaner);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testScaner();
};

#endif // _SCANNER_TEST_H
