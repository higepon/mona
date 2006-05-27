#include "VnodeCacherTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(VnodeCacherTest);

using namespace MonAPI;

#include <map>
using namespace std;

void VnodeCacherTest::setUp()
{
}

void VnodeCacherTest::tearDown()
{
}

void VnodeCacherTest::testFoundNothingCache()
{
    VnodeCacher* cacher = new VnodeCacher;
    vnode* directory1 = new vnode;
    vnode* directory2 = new vnode;
    vnode* file1      = new vnode;
    vnode* file2      = new vnode;

    cacher->add(directory1, "file1.cpp", file1);
    cacher->add(directory2, "file2.cpp", file2);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("unknown not found in directory1", (void*)NULL, (void*)cacher->lookup(directory1, "unknown.cpp"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file1.cpp not found in directory2", (void*)NULL, (void*)cacher->lookup(directory2, "file1.cpp"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file2.cpp not found in directory1", (void*)NULL, (void*)cacher->lookup(directory1, "file2.cpp"));

    delete directory1;
    delete directory2;
    delete file1;
    delete file2;
    delete cacher;
}

void VnodeCacherTest::testRemoveCache()
{
    VnodeCacher* cacher = new VnodeCacher;
    vnode* directory1 = new vnode;
    vnode* directory2 = new vnode;
    vnode* file1      = new vnode;
    vnode* file2      = new vnode;

    cacher->add(directory1, "file1.cpp", file1);
    cacher->add(directory2, "file2.cpp", file2);
    cacher->remove(directory1, "file1.cpp");

    CPPUNIT_ASSERT_EQUAL_MESSAGE("file1.cpp not found in directory1", (void*)NULL, (void*)cacher->lookup(directory1, "file1.cpp"));

    cacher->remove(directory2, "file2.cpp");
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file2.cpp found in directory2", (void*)NULL, (void*)cacher->lookup(directory2, "file2.cpp"));

    delete directory1;
    delete directory2;
    delete file1;
    delete file2;
    delete cacher;
}

void VnodeCacherTest::testAddLookupCache()
{
    VnodeCacher* cacher = new VnodeCacher;
    vnode* directory1 = new vnode;
    vnode* directory2 = new vnode;
    vnode* file1      = new vnode;
    vnode* file2      = new vnode;

    cacher->add(directory1, "file1.cpp", file1);
    cacher->add(directory2, "file2.cpp", file2);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("file1.cpp found in directory1", (void*)file1, (void*)cacher->lookup(directory1, "file1.cpp"));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file2.cpp found in directory2", (void*)file2, (void*)cacher->lookup(directory2, "file2.cpp"));

    delete directory1;
    delete directory2;
    delete file1;
    delete file2;
    delete cacher;
}
