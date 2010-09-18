// -*- Mode: CPP ; Coding: utf-8 -*-
#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <monapi/scoped_ptr.h>
#include <FAT32/Fat.h>
#include <BlockDeviceDriver.h>

using namespace MonAPI;

#define TEST_LONG_FILE_NAME "/APPS/TFILE.APP/this.is.my_long_file_name.file.txt"
#define TEST_UTF8_FILE_NAME "/APPS/TFILE.APP/this.is.my_unicode_file_name.あいう.file.txt"

static bool fileExist(const char* path)
{
    intptr_t id = monapi_file_open(path, 0);
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
    for (int i = 0; i < num; i++) {
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

static void test_seek_minus_offset_should_raise_error()
{
    intptr_t id = monapi_file_open("/APPS/HELLO.EX5", false);
    ASSERT_TRUE(id > 0);
    EXPECT_EQ(M_BAD_OFFSET, monapi_file_seek(id, -1, SEEK_CUR));
    monapi_file_close(id);
}

class TestFatFS
{
public:
    TestFatFS() : dev(new BlockDeviceDriver(3)),
                  vm(new VnodeManager),
                  fat(new FatFileSystem(*vm, *dev))
    {
    }

    FatFileSystem* get() const
    {
        return fat.get();
    }

private:
    scoped_ptr<BlockDeviceDriver> dev;
    scoped_ptr<VnodeManager> vm;
    scoped_ptr<FatFileSystem> fat;
};

static void test_fatfs_parameters()
{
    TestFatFS fs;
    EXPECT_EQ(512, fs.get()->getBytesPerSector());
}

static void test_fatfs_readdir_root()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();

    Vnode* root = fat->getRoot();
    ASSERT_TRUE(root != NULL);
    EXPECT_EQ((int)Vnode::DIRECTORY, root->type);
    EXPECT_EQ(fat, root->fs);
    EXPECT_TRUE(root->fnode != NULL);


    FatFileSystem::Directory* rootDir = (FatFileSystem::Directory*)root->fnode;
    FatFileSystem::Entries& entries = rootDir->getChildlen();
    EXPECT_EQ(20, entries.size());
}

static void test_fatfs_lookup()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    Vnode* found;
    ASSERT_EQ(MONA_SUCCESS, fat->lookup(root, "TEST2.TXT", &found, Vnode::REGULAR));
    FatFileSystem::Entry* file = (FatFileSystem::Entry*)(found->fnode);
    EXPECT_STR_EQ("TEST2.TXT", file->getName().c_str());
    EXPECT_EQ(6, file->getSize());
}

static void test_fatfs_read_file()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    Vnode* found;
    ASSERT_EQ(MONA_SUCCESS, fat->lookup(root, "TEST2.TXT", &found, Vnode::REGULAR));
    io::Context c;
    c.offset = 0;
    c.size   = 256;
    const char* expected = "Hello\n";
    const int len = strlen(expected);
    EXPECT_EQ(MONA_SUCCESS, fat->read(found, &c));
    ASSERT_EQ(len, c.offset);
    ASSERT_EQ(len, c.resultSize);
    ASSERT_TRUE(c.memory != NULL);
    ASSERT_EQ(len, c.memory->Size);
    EXPECT_TRUE(memcmp(expected, c.memory->Data, len) == 0);
}

static void test_fatfs_read_file_with_offset()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    Vnode* found;
    ASSERT_EQ(MONA_SUCCESS, fat->lookup(root, "TEST2.TXT", &found, Vnode::REGULAR));
    io::Context c;
    c.offset = 2;
    c.size   = 256;
    const char* expected = "llo\n";
    const int len = strlen(expected);
    EXPECT_EQ(MONA_SUCCESS, fat->read(found, &c));
    ASSERT_EQ(6, c.offset);
    ASSERT_EQ(len, c.resultSize);
    ASSERT_TRUE(c.memory != NULL);
    ASSERT_EQ(len, c.memory->Size);
    EXPECT_TRUE(memcmp(expected, c.memory->Data, len) == 0);
}

static void test_fatfs_read_file_multiple_clusters()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    Vnode* found;
    ASSERT_EQ(MONA_SUCCESS, fat->lookup(root, "HIGE.TXT", &found, Vnode::REGULAR));
    io::Context c;
    c.offset = 514;
    c.size   = 1000;
    EXPECT_EQ(MONA_SUCCESS, fat->read(found, &c));
    ASSERT_EQ(1000, c.resultSize);
    ASSERT_TRUE(c.memory != NULL);
    EXPECT_EQ(0x36, c.memory->Data[0]);
    EXPECT_EQ(0x35, c.memory->Data[c.memory->Size - 4]);
    EXPECT_EQ(0x0A, c.memory->Data[c.memory->Size - 3]);
    EXPECT_EQ(0x34, c.memory->Data[c.memory->Size - 2]);
    EXPECT_EQ(0x30, c.memory->Data[c.memory->Size - 1]);
}


static void createEmptyfile(FatFileSystem* fat, Vnode* dir, const std::string& filename)
{
    Vnode* found;
    ASSERT_EQ(MONA_SUCCESS, fat->create(dir, filename));
    ASSERT_EQ(MONA_SUCCESS, fat->lookup(dir, filename, &found, Vnode::REGULAR));
    FatFileSystem::Entry* file = (FatFileSystem::Entry*)(found->fnode);
    EXPECT_STR_EQ(filename, file->getName().c_str());
    EXPECT_EQ(0, file->getSize());
}

static void test_fatfs_create_empty_file()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    const char* filename = "MY.TXT";
    createEmptyfile(fat, root, filename);
}

static void test_fatfs_create_empty_file_need_new_cluster()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    for (int i = 0; i < 16; i++) {
        char filename[13];
        sprintf(filename, "MY%d.TXT", i);
        createEmptyfile(fat, root, filename);
    }
    FatFileSystem::Directory* rootDir = (FatFileSystem::Directory*)root->fnode;
    FatFileSystem::Entries& entries = rootDir->getChildlen();
    EXPECT_EQ(37, entries.size());
}

static monapi_cmemoryinfo* readAll(FatFileSystem* fat, io::Context& c, Vnode* dir, const std::string& filename)
{
    Vnode* found;
    ASSERT_EQ(MONA_SUCCESS, fat->lookup(dir, filename, &found, Vnode::REGULAR));
    c.offset = 0;
    c.size   = ((FatFileSystem::Entry*)(found->fnode))->getSize();
    EXPECT_EQ(MONA_SUCCESS, fat->read(found, &c));
    return c.memory;
}

static void test_fatfs_write_file()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    const char* FILENAME = "MY1.TXT";

    Vnode* found;
    ASSERT_EQ(MONA_SUCCESS, fat->lookup(root, FILENAME, &found, Vnode::REGULAR));

    const int BUFFER_SIZE = 1029;

    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, BUFFER_SIZE, 0, 0);
    memset(buffer->Data, 0, BUFFER_SIZE);
    buffer->Data[0] = 0xde;
    buffer->Data[BUFFER_SIZE - 1] = 0xad;

    io::Context c;
    c.offset = 0;
    c.size   = BUFFER_SIZE;
    c.memory = buffer;

    ASSERT_EQ(BUFFER_SIZE, fat->write(found, &c));

    io::Context readContext;
    monapi_cmemoryinfo* cmi = readAll(fat, readContext, root, FILENAME);
    ASSERT_TRUE(cmi != NULL);
    ASSERT_TRUE(cmi->Data != NULL);
    ASSERT_EQ(BUFFER_SIZE, cmi->Size);
    EXPECT_TRUE(memcmp(buffer->Data, cmi->Data, BUFFER_SIZE) == 0);
}

#define MAP_FILE_PATH "/APPS/TFILE.APP/TFILE.MAP"

int main(int argc, char *argv[])
{
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        monapi_fatal("syscall_stack_trace_enable failed%d\n", ret);
    }

    testFileExist_LongFileName();
    testFileExist_UTF8FileName();
    testFileRead_LongFileName();
    testFileRead_UTF8FileName();

    EXPECT_TRUE(testDirectoryHasFile("/", "LIBS"));
    EXPECT_TRUE(testDirectoryHasFile("/LIBS", "MOSH"));
    EXPECT_TRUE(testDirectoryHasFile("/LIBS/MOSH/LIB", "SRFI"));
    EXPECT_TRUE(testDirectoryHasFile("/LIBS/MOSH/LIB/SRFI", "%3A8.SLS"));

    test_seek_minus_offset_should_raise_error();
    test_fatfs_parameters();
    test_fatfs_readdir_root();
    test_fatfs_lookup();
    test_fatfs_read_file();
    test_fatfs_read_file_with_offset();
    test_fatfs_read_file_multiple_clusters();
    test_fatfs_create_empty_file();
    test_fatfs_create_empty_file_need_new_cluster();
    test_fatfs_write_file();
    TEST_RESULTS(file);
    return 0;
}
