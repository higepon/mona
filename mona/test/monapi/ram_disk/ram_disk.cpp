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
    uint32_t id = monapi_file_open("/MEM/TESTFILE", false);
    if(id == 0)
      return false;
    monapi_file_close(id);
    return true;
}

static monapi_cmemoryinfo* alloc_buffer(const char* message)
{
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, strlen(message) + 1, 0);
    memcpy(buffer->Data, message, buffer->Size);
    return buffer;
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

static void writeContentToPath(const char* path, const char* contents)
{
    uint32_t id = monapi_file_open(path, true);

    monapi_cmemoryinfo* buffer = alloc_buffer(contents);
    int res = monapi_file_write(id, buffer, strlen(contents)+1);
    EXPECT_EQ(res, MONA_SUCCESS);
    monapi_file_close(id);
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

static void testReadDirectory_Empty()
{
    monapi_cmemoryinfo* ci = monapi_file_read_directory("/MEM");
    EXPECT_TRUE(ci != NULL);

    int size = *(int*)ci->Data;

    EXPECT_EQ(0, size);

    monapi_cmemoryinfo_dispose(ci);
    monapi_cmemoryinfo_delete(ci);
}

static void testReadDirectory_OneFile()
{
    createFile("/MEM/TEST1.TXT");
    monapi_cmemoryinfo* ci = monapi_file_read_directory("/MEM");

    int size = *(int*)ci->Data;

    EXPECT_EQ(1, size);

    monapi_cmemoryinfo_dispose(ci);
    monapi_cmemoryinfo_delete(ci);

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

    monapi_cmemoryinfo_dispose(ci);
    monapi_cmemoryinfo_delete(ci);

    monapi_file_delete("/MEM/TEST2.TXT");
    monapi_file_delete("/MEM/TEST1.TXT");
}




int main(int argc, char *argv[])
{
    testOpenNonExistingFile();
    testOpenNonExistingFileWithCreate();
    testOpenExistingFile();
    testDeleteFile();
    testWriteFile_Size();
    testWriteFile_Content();

    testReadDirectory_Empty();
    testReadDirectory_OneFile();
    testReadDirectory_TwoFile();

    TEST_RESULTS(ram_disk);
    return 0;
}
