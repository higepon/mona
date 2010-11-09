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
    intptr_t id = monapi_file_open(path, 0);
    if(id < 0)
      return false;
    monapi_file_close(id);
    return true;
}

static SharedMemory alloc_buffer_size(const char* message, int size)
{
    SharedMemory shm(size);
    ASSERT_EQ(M_OK, shm.map());
    memcpy(shm.data(), message, shm.size());
    return shm;
}

static SharedMemory alloc_buffer(const char* message)
{
    return alloc_buffer_size(message, strlen(message) + 1);
}

static void testOpenNonExistingFile()
{
    intptr_t id = monapi_file_open("/MEM/UNEXIST", 0);
    EXPECT_TRUE(id < 0);
}

static void testOpenNonExistingFileWithCreate()
{
    intptr_t id = monapi_file_open("/MEM/UNEXIST2", FILE_CREATE);
    EXPECT_TRUE(id > 0);
    monapi_file_close(id);
    monapi_file_delete("/MEM/UNEXIST2");
}


static void createFile(const char* path)
{
    EXPECT_TRUE(!fileExist(path));
    intptr_t id = monapi_file_open(path, FILE_CREATE);

    const char* message = "Hello World\n";
    SharedMemory shm = alloc_buffer(message);

    monapi_file_write(id, shm, shm.size());

    EXPECT_EQ(M_OK, shm.unmap());
    monapi_file_close(id);
}

static void testOpenExistingFile()
{
    createFile("/MEM/TESTFILE");

    intptr_t id = monapi_file_open("/MEM/TESTFILE", FILE_CREATE);
    EXPECT_TRUE(id > 0);
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
    intptr_t id = monapi_file_open(path, create ? FILE_CREATE : 0);

#define MAXDATA 20
    SharedMemory shm(MAXDATA);
    ASSERT_EQ(M_OK, shm.map());
    int res = 0;
    while(size > 0) {
        int copySize = size > MAXDATA ? MAXDATA : size;
        memcpy(shm.data(), contents, copySize);
        res = monapi_file_write(id, shm, copySize);
        EXPECT_EQ(copySize, res);
        size -= copySize;
        contents += copySize;
    }
    EXPECT_EQ(M_OK, shm.unmap());

#if 0
    monapi_cmemoryinfo* buffer = alloc_buffer_size(contents, size);
    int res = monapi_file_write(id, buffer, size);
#endif
    EXPECT_TRUE(res >= 0);
    monapi_file_close(id);
}

static void writeContentToPath(const char* path, const char* contents, bool create=true)
{
    writeContentToPathWithSize(path, contents, strlen(contents)+1, create);
}

int fileSize(const char* path)
{
    intptr_t id = monapi_file_open(path, 0);
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

    intptr_t id = monapi_file_open(path, 0);
    scoped_ptr<SharedMemory> actual(monapi_file_read(id, 256));
    monapi_file_close(id);

    EXPECT_EQ(len, actual->size());
    EXPECT_TRUE( 0 == memcmp(actual->data(), data, len));
    actual->unmap();
    monapi_file_delete(path);
}

SharedMemory* readContentFromPath(const char *path)
{
    intptr_t id = monapi_file_open(path, 0);
    SharedMemory* shm = monapi_file_read(id, 256);
    // check EOF
    EXPECT_TRUE(NULL == monapi_file_read(id, 256));
    monapi_file_close(id);
    return shm;
}


static void testWriteTwice()
{
    const char* expect = "second";
    int expect_len = strlen(expect)+1;

    const char* path = "/MEM/TESTFILE";
    writeContentToPath(path, "first");
    writeContentToPath(path, expect, false);

    scoped_ptr<SharedMemory> actual(readContentFromPath(path));

    EXPECT_EQ(expect_len, actual->size());
    EXPECT_TRUE( 0 == memcmp(actual->data(), expect, expect_len));

    actual->unmap();
    monapi_file_delete(path);
}

static void testWriteTwice_CreateTrue()
{
    const char* expect = "second";
    int expect_len = strlen(expect)+1;

    const char* path = "/MEM/TESTFILE";
    writeContentToPath(path, "first");
    writeContentToPath(path, expect, true);

    scoped_ptr<SharedMemory> actual(readContentFromPath(path));

    EXPECT_EQ(expect_len, actual->size());
    EXPECT_TRUE( 0 == memcmp(actual->data(), expect, expect_len));

    actual->unmap();

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
    scoped_ptr<SharedMemory> shm(monapi_file_read_all(from));
    ASSERT_TRUE(shm != NULL);
    writeContentToPathWithSize(to, (char*)shm->data(), shm->size());
    shm->unmap();
}

// use stdio because I already have one.
static void expectFileEqual(const char* org, const char* to)
{
    /* check with monapi_file_read_all */
    scoped_ptr<SharedMemory> m1(monapi_file_read_all(org));
    scoped_ptr<SharedMemory> m2(monapi_file_read_all(to));

    ASSERT_TRUE(m1.get() != NULL);
    ASSERT_TRUE(m2.get() != NULL);
    ASSERT_EQ(m1->size(), m2->size());
    EXPECT_TRUE(0 == memcmp(m1->data(), m2->data(), m1->size()));

    EXPECT_EQ(M_OK, m1->unmap());
    EXPECT_EQ(M_OK, m2->unmap());

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
    scoped_ptr<SharedMemory> shm(monapi_file_read_directory("/MEM"));
    EXPECT_TRUE(shm.get() != NULL);

    int size = *(int*)shm->data();

    EXPECT_EQ(0, size);
    shm->unmap();
}

static void testReadDirectory_OneFile()
{
    createFile("/MEM/TEST1.TXT");
    scoped_ptr<SharedMemory> shm(monapi_file_read_directory("/MEM"));

    int size = *(int*)shm->data();

    EXPECT_EQ(1, size);

    shm->unmap();
    monapi_file_delete("/MEM/TEST1.TXT");
}

#include <monapi/CString.h>

static void testReadDirectory_TwoFile()
{
    createFile("/MEM/TEST1.TXT");
    createFile("/MEM/TEST2.TXT");
    scoped_ptr<SharedMemory> shm(monapi_file_read_directory("/MEM"));

    int size = *(int*)shm->data();
    monapi_directoryinfo* p = (monapi_directoryinfo*)&shm->data()[sizeof(int)];

    EXPECT_EQ(2, size);
    EXPECT_TRUE(CString(p[0].name) ==  "TEST1.TXT");
    EXPECT_TRUE(CString(p[1].name) ==  "TEST2.TXT");

    shm->unmap();

    monapi_file_delete("/MEM/TEST2.TXT");
    monapi_file_delete("/MEM/TEST1.TXT");
}

static void testReadDirectory_Root()
{
    scoped_ptr<SharedMemory> shm(monapi_file_read_directory("/"));
    EXPECT_TRUE(shm.get() != NULL);

    int size = *(int*)shm->data();
    monapi_directoryinfo* p = (monapi_directoryinfo*)&shm->data()[sizeof(int)];

    bool ramdiskFound = false;
    for (int i = 0; i < size; i++) {
        if (CString(p[i].name) == "MEM") {
            ramdiskFound = true;
            break;
        }
    }
    EXPECT_TRUE(ramdiskFound);
    shm->unmap();
}

static void testFprintf()
{
    const char* tmpFile = "/MEM/TEST1.TXT";
    FILE* fp = fopen(tmpFile, "w");
    ASSERT_TRUE(fp != NULL);
    fprintf(fp, "Hello, %s", "World!");
    fclose(fp);
    scoped_ptr<SharedMemory> shm(monapi_file_read_all(tmpFile));
    ASSERT_TRUE(shm.get() != NULL);
    const char* expected = "Hello, World!";
    // Be sure, \0 is not written
    EXPECT_EQ((int)strlen(expected), shm->size());
    EXPECT_EQ(0, memcmp(expected, shm->data(), shm->size()));
    shm->unmap();
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
    scoped_ptr<SharedMemory> shm(monapi_file_read_all(tmpFile));
    ASSERT_TRUE(shm.get() != NULL);
    EXPECT_STR_EQ(data, (char*)shm->data());
    shm->unmap();
}

static void testMonAPIwrite()
{
    const char* tmpFile = "/MEM/TEST1.TXT";
    intptr_t id = monapi_file_open(tmpFile, FILE_CREATE);
    ASSERT_TRUE(id > 0);
    const char* data = "Hello,\nWorld";
    const int len = strlen(data) + 1;

    SharedMemory shm(1);
    ASSERT_EQ(M_OK, shm.map());

    for (int i = 0; i < len; i++) {
        shm.data()[0] = data[i];
        monapi_file_write(id, shm, 1);
    }
    monapi_file_close(id);
    scoped_ptr<SharedMemory> shm2(readContentFromPath(tmpFile));
    EXPECT_STR_EQ(data, (char*)shm2->data());

    ASSERT_EQ(M_OK, shm.unmap());
    ASSERT_EQ(M_OK, shm2->unmap());
    monapi_file_delete(tmpFile);
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
    scoped_ptr<SharedMemory> shm(monapi_file_read_all(tmpFile));
    ASSERT_TRUE(shm.get() != NULL);
    EXPECT_STR_EQ(data, (char*)shm->data());
    shm->unmap();
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
    scoped_ptr<SharedMemory> shm(monapi_file_read_all(tmpFile));
    ASSERT_TRUE(shm.get() != NULL);
    EXPECT_STR_EQ(data, (char*)shm->data());
    shm->unmap();
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
    scoped_ptr<SharedMemory> shm(monapi_file_read_all(tmpFile));
    ASSERT_TRUE(shm.get() != NULL);

    EXPECT_TRUE(memcmp("Hello\n    \nWorld     \n", shm->data(), shm->size()) == 0);
    shm->unmap();
    monapi_file_delete(tmpFile);
    monapi_file_delete(tmpFile);
}

static void testFopen_No_Leak()
{
    const char* tmpFile = "/MEM/TEST1.TXT";

    malloc_stat st1;
    malloc_stats(&st1);

    FILE* fp = fopen(tmpFile, "w");
    fwrite("Hello", 1, 5, fp);
    fprintf(fp, "\n");
    char buf[256];
    fread(buf, 1, 256, fp);
    fclose(fp);

    malloc_stat st2;
    malloc_stats(&st2);
    EXPECT_EQ(st1.used, st2.used);

    monapi_file_delete(tmpFile);
}

static void testSeek_MinusOffsetShouldBeError()
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

    intptr_t id = monapi_file_open(tmpFile, 0);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(M_BAD_OFFSET, monapi_file_seek(id, -1, SEEK_CUR));
    monapi_file_close(id);
    monapi_file_delete(tmpFile);
}

static void testOpen_create_truncate_file_not_exist()
{
    const char* filename = "/MEM/NOT_EXIST";
    intptr_t id = monapi_file_open(filename, FILE_CREATE | FILE_TRUNCATE);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(0, monapi_file_get_file_size(id));
    monapi_file_close(id);
    monapi_file_delete(filename);
}

static void testOpen_create_truncate_file_exists()
{
    const char* filename = "/MEM/HOGE.TXT";
    createFile(filename);
    intptr_t id = monapi_file_open(filename, FILE_CREATE | FILE_TRUNCATE);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(0, monapi_file_get_file_size(id));
    monapi_file_close(id);
    monapi_file_delete(filename);
}

static void testOpen_create_noTruncate_file_not_exist()
{
    const char* filename = "/MEM/NOT_EXIST";
    intptr_t id = monapi_file_open(filename, FILE_CREATE);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(0, monapi_file_get_file_size(id));
    monapi_file_close(id);
    monapi_file_delete(filename);
}

static void testOpen_create_noTruncate_file_exists()
{
    const char* filename = "/MEM/HOGE.TXT";
    createFile(filename);
    intptr_t id = monapi_file_open(filename, FILE_CREATE);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(13, monapi_file_get_file_size(id));
    monapi_file_close(id);
    monapi_file_delete(filename);
}

static void testOpen_noCreate_truncate_file_not_exist()
{
    const char* filename = "/MEM/NOT_EXIST";
    intptr_t id = monapi_file_open(filename, FILE_TRUNCATE);
    ASSERT_EQ(M_FILE_NOT_FOUND, id);
}

static void testOpen_noCreate_truncate_file_exists()
{
    const char* filename = "/MEM/HOGE.TXT";
    createFile(filename);
    intptr_t id = monapi_file_open(filename, FILE_TRUNCATE);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(0, monapi_file_get_file_size(id));
    monapi_file_close(id);
    monapi_file_delete(filename);
}

static void testOpen_noCreate_noTruncate_file_not_exist()
{
    const char* filename = "/MEM/NOT_EXIST";
    intptr_t id = monapi_file_open(filename, 0);
    ASSERT_EQ(M_FILE_NOT_FOUND, id);
}

static void testOpen_noCreate_noTruncate_file_exists()
{
    const char* filename = "/MEM/HOGE.TXT";
    createFile(filename);
    intptr_t id = monapi_file_open(filename, 0);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(13, monapi_file_get_file_size(id));
    monapi_file_close(id);
    monapi_file_delete(filename);
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
    testFopen_No_Leak();
    testMonAPIwrite();
    testSeek_MinusOffsetShouldBeError();

    // open crete and truncate flags
    testOpen_create_truncate_file_not_exist();
    testOpen_create_truncate_file_exists();
    testOpen_create_noTruncate_file_not_exist();
    testOpen_create_noTruncate_file_exists();
    testOpen_noCreate_truncate_file_not_exist();
    testOpen_noCreate_truncate_file_exists();
    testOpen_noCreate_noTruncate_file_not_exist();
    testOpen_noCreate_noTruncate_file_exists();
    TEST_RESULTS(ram_disk);
    return 0;
}
