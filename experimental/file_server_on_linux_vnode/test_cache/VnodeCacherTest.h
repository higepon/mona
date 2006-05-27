#ifndef __VNODE_CACHER_TEST_H__
#define __VNODE_CACHER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>
#include "VnodeCacher.h"

class VnodeCacherTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(VnodeCacherTest);
    CPPUNIT_TEST(testAddLookupCache);
    CPPUNIT_TEST(testFoundNothingCache);
    CPPUNIT_TEST(testRemoveCache);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testAddLookupCache();
    void testFoundNothingCache();
    void testRemoveCache();
};

#endif // __FILE_SERVER_TEST_H__
