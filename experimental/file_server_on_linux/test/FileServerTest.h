#ifndef __FILE_SERVER_TEST_H__
#define __FILE_SERVER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

class FileServerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(FileServerTest);
    CPPUNIT_TEST(testISO9660ReadDirectory);
    CPPUNIT_TEST(testISO9660ReadFile);
    CPPUNIT_TEST(testISO9660FileSize);
    CPPUNIT_TEST(testFat12ReadDirectory);
    CPPUNIT_TEST(testFat12ReadFile);
    CPPUNIT_TEST(testFat12FileSize);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testISO9660ReadDirectory();
    void testISO9660ReadFile();
    void testISO9660FileSize();
    void testFat12ReadDirectory();
    void testFat12ReadFile();
    void testFat12FileSize();

private:
};

#endif // __FILE_SERVER_TEST_H__
