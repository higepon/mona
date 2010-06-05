#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <monapi.h>
#include <stdio.h>

using namespace MonAPI;

static bool fileExist(const char* path)
{
    uint32_t id = monapi_file_open(path, false);
    if(id == 0)
      return false;
    monapi_file_close(id);
    return true;
}

static void cm_destroy_delete(monapi_cmemoryinfo *cm)
{
    monapi_cmemoryinfo_dispose(cm);
    monapi_cmemoryinfo_delete(cm);
}

static monapi_cmemoryinfo* alloc_buffer_size(const char* message, int size)
{
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, size, 0);
    memcpy(buffer->Data, message, buffer->Size);
    return buffer;
}

static monapi_cmemoryinfo* alloc_buffer(const char* message)
{
    return alloc_buffer_size(message, strlen(message) + 1);
}

static void testOpenNonExistingFile()
{
    uint32_t id = monapi_file_open("/MEM/UNEXIST", false);
    EXPECT_EQ(0, id);
}

static void testOpenNonExistingFileWithCreate()
{
    uint32_t id = monapi_file_open("/MEM/UNEXIST2", true);
    EXPECT_TRUE(id != 0);
    monapi_file_close(id);
    monapi_file_delete("/MEM/UNEXIST2");
}


static void createFile(const char* path)
{
    EXPECT_TRUE(!fileExist(path));
    uint32_t id = monapi_file_open(path, true);

    const char* message = "Hello World\n";
    monapi_cmemoryinfo* buffer = alloc_buffer(message);

    monapi_file_write(id, buffer, buffer->Size);

    monapi_cmemoryinfo_dispose(buffer);
    monapi_cmemoryinfo_delete(buffer);

    monapi_file_close(id);
}

static void testOpenExistingFile()
{
    createFile("/MEM/TESTFILE");

    uint32_t id = monapi_file_open("/MEM/TESTFILE", false);
    EXPECT_TRUE(0 != id);
    monapi_file_close(id);

    monapi_file_delete("/MEM/TESTFILE");
}

static void testDeleteFile()
{
    const char* path = "/MEM/TESTFILE";
    createFile(path);

    EXPECT_TRUE(fileExist(path));
    monapi_file_delete(path);
    EXPECT_TRUE(!fileExist(path));
}

static void writeContentToPathWithSize(const char* path, const char* contents, int size, bool create=true)
{
    uint32_t id = monapi_file_open(path, create);

#define MAXDATA 20
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, MAXDATA, 0);
    int res = MONA_FAILURE;
    while(size > 0) {
        int copySize = size > MAXDATA ? MAXDATA : size;
        memcpy(buffer->Data, contents, copySize);
        res = monapi_file_write(id, buffer, copySize);
        EXPECT_EQ(res, MONA_SUCCESS);
        monapi_file_seek(id, copySize, SEEK_CUR);
        size -= copySize;
        contents += copySize;
    }
    cm_destroy_delete(buffer);

#if 0
    monapi_cmemoryinfo* buffer = alloc_buffer_size(contents, size);
    int res = monapi_file_write(id, buffer, size);
#endif
    EXPECT_EQ(res, MONA_SUCCESS);
    monapi_file_close(id);
}

static void writeContentToPath(const char* path, const char* contents, bool create=true)
{
    writeContentToPathWithSize(path, contents, strlen(contents)+1, create);
}

int fileSize(const char* path)
{
    uint32_t id = monapi_file_open(path, false);
    int actual = monapi_file_get_file_size(id);
    monapi_file_close(id);
    return actual;
}

static void testWriteFile_Size()
{
    const char* message = "test data";
    int expectedLen = strlen(message)+1;

    const char* path = "/MEM/TESTFILE";
    writeContentToPath(path, message);

    int actual = fileSize(path);

    EXPECT_EQ(expectedLen, actual);
    monapi_file_delete(path);
}

static void testWriteFile_Content()
{
    const char* data = "test data";
    int len = strlen(data)+1;

    const char* path = "/MEM/TESTFILE";
    writeContentToPath(path, data);

    uint32_t id = monapi_file_open(path, false);
    monapi_cmemoryinfo *actual = NULL;
    actual = monapi_file_read(id, 256);
    monapi_file_close(id);

    EXPECT_EQ(len, actual->Size);
    EXPECT_TRUE( 0 == memcmp(actual->Data, data, len));

    monapi_cmemoryinfo_dispose(actual);
    monapi_cmemoryinfo_delete(actual);

    monapi_file_delete(path);
}

monapi_cmemoryinfo* readContentFromPath(const char *path)
{
    uint32_t id = monapi_file_open(path, false);
    monapi_cmemoryinfo *cmi = monapi_file_read(id, 256);
    monapi_file_close(id);
    return cmi;
}


static void testWriteTwice()
{
    const char* expect = "second";
    int expect_len = strlen(expect)+1;

    const char* path = "/MEM/TESTFILE";
    writeContentToPath(path, "first");
    writeContentToPath(path, expect, false);

    monapi_cmemoryinfo *actual = readContentFromPath(path);

    EXPECT_EQ(expect_len, actual->Size);
    EXPECT_TRUE( 0 == memcmp(actual->Data, expect, expect_len));

    cm_destroy_delete(actual);
    monapi_file_delete(path);
}

static void testWriteTwice_CreateTrue()
{
    const char* expect = "second";
    int expect_len = strlen(expect)+1;
    
    const char* path = "/MEM/TESTFILE";
    writeContentToPath(path, "first");
    writeContentToPath(path, expect, true);

    monapi_cmemoryinfo *actual = readContentFromPath(path);

    EXPECT_EQ(expect_len, actual->Size);
    EXPECT_TRUE( 0 == memcmp(actual->Data, expect, expect_len));

    cm_destroy_delete(actual);

    monapi_file_delete(path);
}

static void testWriteTwice_Size()
{
    const char* message = "test data";
    int expectedLen = strlen(message)+1;

    const char* path = "/MEM/TESTFILE";
    writeContentToPath(path, "1");
    writeContentToPath(path, message);

    int actual = fileSize(path);

    EXPECT_EQ(expectedLen, actual);
    monapi_file_delete(path);
}

static void copyFile(const char *from, const char* to)
{
    monapi_cmemoryinfo *cmi = monapi_file_read_all(from);
    ASSERT_TRUE(cmi != NULL);
    writeContentToPathWithSize(to, (char*)cmi->Data, cmi->Size);
}

// use stdio because I already have one.
static void expectFileEqual(const char* org, const char* to)
{
    /* check with monapi_file_read_all */
    monapi_cmemoryinfo* m1 = monapi_file_read_all(org);
    monapi_cmemoryinfo* m2 = monapi_file_read_all(to);

    ASSERT_TRUE(m1 != NULL);
    ASSERT_TRUE(m2 != NULL);
    ASSERT_EQ(m1->Size, m2->Size);
    EXPECT_TRUE(0 == memcmp(m1->Data, m2->Data, m1->Size));

    monapi_cmemoryinfo_dispose(m1);
    monapi_cmemoryinfo_delete(m1);
    monapi_cmemoryinfo_dispose(m2);
    monapi_cmemoryinfo_delete(m2);

    /* check with fread */
    char buf[256];
    char buf2[256];
    FILE* fp_org = fopen(org, "r");
    FILE* fp = fopen(to, "r");
    if(fp == NULL || fp_org == NULL)
      {
          _printf("fp=%x, fp_org=%x\n", fp, fp_org);
      }

// for analyze    int debpos = 0;
    int readSize = fread(buf, 1, 256, fp_org);
    while(readSize > 0)
    {
        int readSize2 = fread(buf2, 1, readSize, fp);
        EXPECT_EQ(readSize, readSize2);
        if (0 != memcmp(buf, buf2, readSize2)) {
            logprintf("buf=\n");
            for (int i = 0; i < readSize2; i++) {
                logprintf("[%x]", buf[i]);
            }
            logprintf("\n");
            logprintf("buf2=\n");
            for (int i = 0; i < readSize2; i++) {
                logprintf("[%x]", buf2[i]);
            }

        }
        ASSERT_TRUE(0 == memcmp(buf, buf2, readSize2));
/*
        if(0 != memcmp(buf, buf2, readSize2))
        {
            for(int i = 0; i < readSize2; i++)
              {
                  if(buf[i] != buf2[i])
                  {
                      static int count = 0;
                      fprintf(stderr, "debpos=%x, i=%x, buf[i]=%x, buf2[i]=%x\n", debpos, i, buf[i], buf2[i]);
                      break;
                      if(count++ > 20)
                        break;
                  }
              }
            fclose(fp);
            fclose(fp_org);
            return;
        }
        debpos+=readSize;
*/

        readSize = fread(buf, 1, 256, fp_org);
    }
    fclose(fp);
    fclose(fp_org);

}



static void testWriteLargeFile()
{
    // depend on other test file...
    const char* org = "/APPS/TSTDIO.APP/TEST.JPG";
    const char* path = "/MEM/TESTFILE";

    copyFile(org, path);
    expectFileEqual(org, path);

    monapi_file_delete(path);
}

static void testReadDirectory_Empty()
{
    monapi_cmemoryinfo* ci = monapi_file_read_directory("/MEM");
    EXPECT_TRUE(ci != NULL);

    int size = *(int*)ci->Data;

    EXPECT_EQ(0, size);

    cm_destroy_delete(ci);
}

static void testReadDirectory_OneFile()
{
    createFile("/MEM/TEST1.TXT");
    monapi_cmemoryinfo* ci = monapi_file_read_directory("/MEM");

    int size = *(int*)ci->Data;

    EXPECT_EQ(1, size);

    cm_destroy_delete(ci);

    monapi_file_delete("/MEM/TEST1.TXT");
}

#include <monapi/CString.h>

static void testReadDirectory_TwoFile()
{
    createFile("/MEM/TEST1.TXT");
    createFile("/MEM/TEST2.TXT");
    monapi_cmemoryinfo* ci = monapi_file_read_directory("/MEM");

    int size = *(int*)ci->Data;
    monapi_directoryinfo* p = (monapi_directoryinfo*)&ci->Data[sizeof(int)];

    EXPECT_EQ(2, size);
    EXPECT_TRUE(CString(p[0].name) ==  "TEST1.TXT");
    EXPECT_TRUE(CString(p[1].name) ==  "TEST2.TXT");

    cm_destroy_delete(ci);

    monapi_file_delete("/MEM/TEST2.TXT");
    monapi_file_delete("/MEM/TEST1.TXT");
}

static void testReadDirectory_Root()
{
    monapi_cmemoryinfo* ci = monapi_file_read_directory("/");
    EXPECT_TRUE(ci != NULL);

    int size = *(int*)ci->Data;
    monapi_directoryinfo* p = (monapi_directoryinfo*)&ci->Data[sizeof(int)];

    bool ramdiskFound = false;
    for (int i = 0; i < size; i++) {
        if (CString(p[i].name) == "MEM") {
            ramdiskFound = true;
            break;
        }
    }
    EXPECT_TRUE(ramdiskFound);

    monapi_cmemoryinfo_dispose(ci);
    monapi_cmemoryinfo_delete(ci);
}

static void testFprintf()
{
    const char* tmpFile = "/MEM/TEST1.TXT";
    FILE* fp = fopen(tmpFile, "w");
    ASSERT_TRUE(fp != NULL);
    fprintf(fp, "Hello, %s", "World!");
    fclose(fp);
    monapi_cmemoryinfo* cmi = monapi_file_read_all(tmpFile);
    ASSERT_TRUE(cmi != NULL);
    const char* expected = "Hello, World!";
    // Be sure, \0 is not written
    EXPECT_EQ((int)strlen(expected), cmi->Size);
    EXPECT_EQ(0, memcmp(expected, cmi->Data, cmi->Size));

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
    monapi_file_delete(tmpFile);
}

static void testFwrite()
{
    const char* tmpFile = "/MEM/TEST1.TXT";
    FILE* fp = fopen(tmpFile, "w");
    ASSERT_TRUE(fp != NULL);
    const char* data = "Hello,\nWorld";
    const int len = strlen(data) + 1;
    for (int i = 0; i < len; i++) {
        fwrite(data + i, 1, 1, fp);
    }
    fclose(fp);
    monapi_cmemoryinfo* cmi = monapi_file_read_all(tmpFile);
    ASSERT_TRUE(cmi != NULL);
    EXPECT_STR_EQ(data, (char*)cmi->Data);

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
}

static void testFwrite2()
{
    const char* tmpFile = "/MEM/TEST1.TXT";
    FILE* fp = fopen(tmpFile, "w");
    ASSERT_TRUE(fp != NULL);
    const char* data = "Hello,\nWorld";
    const int len = strlen(data) + 1;
    fwrite(data, 1, len, fp);
    fclose(fp);
    monapi_cmemoryinfo* cmi = monapi_file_read_all(tmpFile);
    ASSERT_TRUE(cmi != NULL);
    EXPECT_STR_EQ(data, (char*)cmi->Data);

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
    monapi_file_delete(tmpFile);
}

static void testFwrite_Overwrite()
{
    const char* tmpFile = "/MEM/TEST1.TXT";
    FILE* fp = fopen(tmpFile, "w");
    ASSERT_TRUE(fp != NULL);
    const char* data = "Tokyo, Japan";
    const int len = strlen(data) + 1;
    for (int i = 0; i < len; i++) {
        fwrite(data + i, 1, 1, fp);
    }
    fclose(fp);
    monapi_cmemoryinfo* cmi = monapi_file_read_all(tmpFile);
    ASSERT_TRUE(cmi != NULL);
    EXPECT_STR_EQ(data, (char*)cmi->Data);

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
    monapi_file_delete(tmpFile);
    monapi_file_delete(tmpFile);
}

static void testFwrite_Overwrite2()
{
    const char* tmpFile = "/MEM/TEST1.TXT";
    {
        FILE* fp = fopen(tmpFile, "w");
        ASSERT_TRUE(fp != NULL);
        const char* data = "Hello\n    \n    \n    ";
        const int len = strlen(data) + 1;
        fwrite(data, 1, len, fp);
        fclose(fp);
    }

    {
        FILE* fp = fopen(tmpFile, "w");
        ASSERT_TRUE(fp != NULL);
        fwrite("Hello", 1, 5, fp);
        fprintf(fp, "\n");
        fwrite("    ", 1, 4, fp);
        fprintf(fp, "\n");
        fwrite("World     ", 1, 10, fp);
        fprintf(fp, "\n");
        fwrite("", 1, 1, fp); // NULL terminate
        fclose(fp);
    }
    monapi_cmemoryinfo* cmi = monapi_file_read_all(tmpFile);
    ASSERT_TRUE(cmi != NULL);

    EXPECT_TRUE(memcmp("Hello\n    \nWorld     \n", cmi->Data, cmi->Size) == 0);

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
    monapi_file_delete(tmpFile);
    monapi_file_delete(tmpFile);
}

int main(int argc, char *argv[])
{
    testOpenNonExistingFile();
    testOpenNonExistingFileWithCreate();
    testOpenExistingFile();
    testDeleteFile();
    testWriteFile_Size();
    testWriteFile_Content();
    testWriteTwice();
    testWriteTwice_CreateTrue();
    testWriteTwice_Size();
    testWriteLargeFile();

    testReadDirectory_Empty();
    testReadDirectory_OneFile();
    testReadDirectory_TwoFile();
    testReadDirectory_Root();

    // We use ram_disk for testing fprintf
    testFprintf();
    testFwrite();
    testFwrite_Overwrite();
    testFwrite2();
    testFwrite_Overwrite2();

    TEST_RESULTS(ram_disk);
    return 0;
}
