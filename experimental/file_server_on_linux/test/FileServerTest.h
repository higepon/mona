#ifndef __FILE_SERVER_TEST_H__
#define __FILE_SERVER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

class FileServerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(FileServerTest);
    CPPUNIT_TEST(testISO9660ReadDirectory);
    CPPUNIT_TEST(testISO9660ReadDirectoryNG);
    CPPUNIT_TEST(testISO9660ReadFile);
    CPPUNIT_TEST(testISO9660ReadFileNG);
    CPPUNIT_TEST(testISO9660FileSize);
    CPPUNIT_TEST(testProcessReadDirectory);
    CPPUNIT_TEST(testProcessReadFile);
    CPPUNIT_TEST(testFAT12ReadFile);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testISO9660ReadDirectory();
    void testISO9660ReadDirectoryNG();
    void testISO9660ReadFile();
    void testISO9660ReadFileNG();
    void testISO9660FileSize();
    void testProcessReadDirectory();
    void testProcessReadFile();
    void testFAT12ReadFile();

private:
    void testProcessReadFileOnce();
    void testProcessReadDirectoryOnce();
};

#endif // __FILE_SERVER_TEST_H__
