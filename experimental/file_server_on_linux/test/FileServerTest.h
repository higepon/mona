#ifndef __FILE_SERVER_TEST_H__
#define __FILE_SERVER_TEST_H__

#include <cppunit/extensions/HelperMacros.h>

class FileServerTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(FileServerTest);
    CPPUNIT_TEST(testISO9660ReadDirectory);
    CPPUNIT_TEST(testISO9660ReadDirectoryNG);
    CPPUNIT_TEST(testISO9660ReadFile);
    CPPUNIT_TEST(testISO9660WriteFile);
    CPPUNIT_TEST(testISO9660ReadFileNG);
    CPPUNIT_TEST(testISO9660FileSize);
    CPPUNIT_TEST(testISO9660CreateFile);
    CPPUNIT_TEST(testProcessReadDirectory);
    CPPUNIT_TEST(testProcessReadFile);
    CPPUNIT_TEST(testProcessWriteFile);
    CPPUNIT_TEST(testProcessCreateFile);
    CPPUNIT_TEST(testFAT12ReadFile);
    CPPUNIT_TEST(testFAT12WriteFile);
    CPPUNIT_TEST(testFAT12FileSize);
    CPPUNIT_TEST(testFAT12ReadDirectory);
    CPPUNIT_TEST(testFAT12ReadDirectoryNG);
    CPPUNIT_TEST(testFAT12ReadFileNG);
    CPPUNIT_TEST(testFAT12CreateFile);
    CPPUNIT_TEST(testST5);
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();
    void testISO9660ReadDirectory();
    void testISO9660ReadDirectoryNG();
    void testISO9660ReadFile();
    void testISO9660WriteFile();
    void testISO9660ReadFileNG();
    void testISO9660FileSize();
    void testISO9660CreateFile();
    void testProcessReadDirectory();
    void testProcessReadFile();
    void testProcessWriteFile();
    void testProcessCreateFile();
    void testFAT12ReadFile();
    void testFAT12WriteFile();
    void testFAT12FileSize();
    void testFAT12ReadDirectory();
    void testFAT12ReadDirectoryNG();
    void testFAT12ReadFileNG();
    void testFAT12CreateFile();
    void testST5();

private:
    void testProcessReadFileOnce();
    void testProcessReadDirectoryOnce();
};

#endif // __FILE_SERVER_TEST_H__
