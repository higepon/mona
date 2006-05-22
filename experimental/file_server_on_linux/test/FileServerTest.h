#ifndef __FILE_SERVER_TEST_H__
#define __FILE_SERVER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

class FileServerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(FileServerTest);
    CPPUNIT_TEST(testReadDirectory);
    CPPUNIT_TEST(testReadFile);
    CPPUNIT_TEST(testFileSize);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testReadDirectory();
    void testReadFile();
    void testFileSize();

private:
};

#endif // __FILE_SERVER_TEST_H__
