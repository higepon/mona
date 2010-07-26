// -*- Mode: CPP ; Coding: utf-8 -*-
#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi.h>
#include <stdio.h>
#include <limits.h>
#include <string>

using namespace MonAPI;

#define TEST_LONG_FILE_NAME "/APPS/TFILE.APP/this.is.my_long_file_name.file.txt"
#define TEST_UTF8_FILE_NAME "/APPS/TFILE.APP/this.is.my_unicode_file_name.あいう.file.txt"

static bool fileExist(const char* path)
{
    intptr_t id = monapi_file_open(path, false);
    if(id < 0)
      return false;
    monapi_file_close(id);
    return true;
}

static void testFileExist_LongFileName()
{
    EXPECT_TRUE(fileExist(TEST_LONG_FILE_NAME));
}

static void testFileExist_UTF8FileName()
{
    EXPECT_TRUE(fileExist(TEST_UTF8_FILE_NAME));
}

static void testFileRead_LongFileName()
{
    monapi_cmemoryinfo* cmi = monapi_file_read_all(TEST_LONG_FILE_NAME);
    ASSERT_TRUE(cmi != NULL);
    const char* expected = "Hello\n";
    // Be sure, \0 is not written
    EXPECT_EQ((int)strlen(expected), cmi->Size);
    EXPECT_EQ(0, memcmp(expected, cmi->Data, cmi->Size));

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
}

static void testFileRead_UTF8FileName()
{
    monapi_cmemoryinfo* cmi = monapi_file_read_all(TEST_UTF8_FILE_NAME);
    ASSERT_TRUE(cmi != NULL);
    const char* expected = "World\n";
    // Be sure, \0 is not written
    EXPECT_EQ((int)strlen(expected), cmi->Size);
    EXPECT_EQ(0, memcmp(expected, cmi->Data, cmi->Size));

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
}

static bool fileExistInDirectory(monapi_directoryinfo* entry, int num, const std::string& file)
{
    logprintf("dump start\n");
    for (int i = 0; i < num; i++) {
        logprintf("<%s>\n", entry[i].name);
        if (std::string(entry[i].name) == file) {
            return true;
        }
    }
    return false;
}

static bool testDirectoryHasFile(const std::string& dir, const std::string& file)
{
    monapi_cmemoryinfo* cmi = monapi_file_read_directory(dir.c_str());
    ASSERT_TRUE(cmi != NULL);

    int size = *(int*)cmi->Data;
    monapi_directoryinfo* p = (monapi_directoryinfo*)&cmi->Data[sizeof(int)];

    bool ret = fileExistInDirectory(p, size, file.c_str());

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
    return ret;
}

int main(int argc, char *argv[])
{
    testFileExist_LongFileName();
    testFileExist_UTF8FileName();
    testFileRead_LongFileName();
    testFileRead_UTF8FileName();

    EXPECT_TRUE(testDirectoryHasFile("/", "LIBS"));
    EXPECT_TRUE(testDirectoryHasFile("/LIBS", "MOSH"));
    EXPECT_TRUE(testDirectoryHasFile("/LIBS/MOSH/LIB", "SRFI"));
    EXPECT_TRUE(testDirectoryHasFile("/LIBS/MOSH/LIB/SRFI", "%3A8.SLS"));
    TEST_RESULTS(file);
    return 0;
}
