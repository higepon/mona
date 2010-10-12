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
                  fat(new FatFileSystem(*dev))
    {
    }

    FatFileSystem* get() const
    {
        return fat.get();
    }

private:
    scoped_ptr<BlockDeviceDriver> dev;
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


    FatFileSystem::File* rootDir = (FatFileSystem::File*)root->fnode;
    FatFileSystem::Files* entries = rootDir->getChildlen();
    EXPECT_EQ(21, entries->size());
}

static void test_fatfs_lookup()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, "TEST2.TXT", &found, Vnode::REGULAR));
    FatFileSystem::File* file = (FatFileSystem::File*)(found->fnode);
    EXPECT_STR_EQ("TEST2.TXT", file->getName().c_str());
    EXPECT_EQ(6, file->getSize());
}

static void test_fatfs_read_file()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, "TEST2.TXT", &found, Vnode::REGULAR));
    io::Context c;
    c.offset = 0;
    c.size   = 256;
    const char* expected = "Hello\n";
    const int len = strlen(expected);
    EXPECT_EQ(M_OK, fat->read(found, &c));
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
    ASSERT_EQ(M_OK, fat->lookup(root, "TEST2.TXT", &found, Vnode::REGULAR));
    io::Context c;
    c.offset = 2;
    c.size   = 256;
    const char* expected = "llo\n";
    const int len = strlen(expected);
    EXPECT_EQ(M_OK, fat->read(found, &c));
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
    ASSERT_EQ(M_OK, fat->lookup(root, "HIGE.TXT", &found, Vnode::REGULAR));
    io::Context c;
    c.offset = 514;
    c.size   = 1000;
    EXPECT_EQ(M_OK, fat->read(found, &c));
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
    ASSERT_EQ(M_OK, fat->create(dir, filename));
    ASSERT_EQ(M_OK, fat->lookup(dir, filename, &found, Vnode::REGULAR));
    FatFileSystem::File* file = (FatFileSystem::File*)(found->fnode);
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
    FatFileSystem::File* rootDir = (FatFileSystem::File*)root->fnode;
    FatFileSystem::Files* entries = rootDir->getChildlen();
    EXPECT_EQ(38, entries->size());
}

static monapi_cmemoryinfo* readAll(FatFileSystem* fat, io::Context& c, Vnode* dir, const std::string& filename)
{
    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(dir, filename, &found, Vnode::REGULAR));
    c.offset = 0;
    c.size   = ((FatFileSystem::File*)(found->fnode))->getSize();
    EXPECT_EQ(M_OK, fat->read(found, &c));
    return c.memory;
}

static void test_fatfs_write_file()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    const char* FILENAME = "MY1.TXT";

    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, FILENAME, &found, Vnode::REGULAR));

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

static void test_fatfs_write_file_overwrite()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    const char* FILENAME = "MY1.TXT";

    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, FILENAME, &found, Vnode::REGULAR));

    const int BUFFER_SIZE = 10;

    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, BUFFER_SIZE, 0, 0);
    memset(buffer->Data, 0, BUFFER_SIZE);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        buffer->Data[i] = i;
    }

    io::Context c;
    c.offset = 513;
    c.size   = BUFFER_SIZE;
    c.memory = buffer;

    ASSERT_EQ(BUFFER_SIZE, fat->write(found, &c));

    io::Context readContext;
    monapi_cmemoryinfo* cmi = readAll(fat, readContext, root, FILENAME);
    ASSERT_TRUE(cmi != NULL);
    ASSERT_TRUE(cmi->Data != NULL);
    EXPECT_EQ(0xde, cmi->Data[0]);
    EXPECT_EQ(0xad, cmi->Data[cmi->Size -1]);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        EXPECT_EQ(i, cmi->Data[513 + i]);
    }
}

static void test_fatfs_write_file_overwrite_expand()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    const char* FILENAME = "MY1.TXT";

    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, FILENAME, &found, Vnode::REGULAR));

    const int BUFFER_SIZE = 1540;

    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, BUFFER_SIZE, 0, 0);
    memset(buffer->Data, 0xce, BUFFER_SIZE);
    buffer->Data[0] = 0xca;
    buffer->Data[buffer->Size - 1] = 0xfe;

    io::Context c;
    c.offset = 513;
    c.size   = BUFFER_SIZE;
    c.memory = buffer;

    ASSERT_EQ(BUFFER_SIZE, fat->write(found, &c));

    io::Context readContext;
    monapi_cmemoryinfo* cmi = readAll(fat, readContext, root, FILENAME);
    ASSERT_TRUE(cmi != NULL);
    ASSERT_TRUE(cmi->Data != NULL);
    ASSERT_EQ(2053, cmi->Size);
    EXPECT_EQ(0xde, cmi->Data[0]);
    EXPECT_EQ(0xca, cmi->Data[513]);
    EXPECT_EQ(0xfe, cmi->Data[513 + BUFFER_SIZE - 1]);
}

static void test_fatfs_delete_file()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    FatFileSystem::File* rootDir = (FatFileSystem::File*)root->fnode;
    FatFileSystem::Files* entries = rootDir->getChildlen();
    const char* FILENAME = "HIGE.TXT";

    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, FILENAME, &found, Vnode::REGULAR));
    FatFileSystem::File* file = (FatFileSystem::File*)(found->fnode);
    EXPECT_EQ(3893, file->getSize());

    uint32_t prevFreeSize = fat->getFreeSize();
    uint32_t prevNumFiles = entries->size();
    ASSERT_EQ(M_OK, fat->delete_file(found));
    EXPECT_EQ(512 * 8, fat->getFreeSize() - prevFreeSize);
    EXPECT_EQ(1, prevNumFiles - entries->size());
}

static void test_fatfs_truncate()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    const char* FILENAME = "TEST1.TXT";

    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, FILENAME, &found, Vnode::REGULAR));
    FatFileSystem::File* file = (FatFileSystem::File*)(found->fnode);
    EXPECT_EQ(6, file->getSize());

    uint32_t prevFreeSize = fat->getFreeSize();
    ASSERT_EQ(M_OK, fat->truncate(found));
    EXPECT_EQ(512, fat->getFreeSize() - prevFreeSize);
    EXPECT_EQ(0, file->getSize());
}

static void test_fatfs_readdir()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    monapi_cmemoryinfo* cmi;
    ASSERT_EQ(M_OK, fat->readdir(root, &cmi));
    ASSERT_TRUE(cmi != NULL);

    int size = *(int*)cmi->Data;

    EXPECT_EQ(37, size);

    monapi_directoryinfo* p = (monapi_directoryinfo*)&cmi->Data[sizeof(int)];

    EXPECT_STR_EQ("TEST1.TXT", (const char*)CString(p[0].name));

    for (int i = 0; i < size; i++) {
        if (CString(p[i].name) == "SUBDIR") {
            EXPECT_TRUE((p[i].attr & ATTRIBUTE_DIRECTORY) != 0);
        }
    }

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
}

static void test_fatfs_lookup_subdir()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();
    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, "SUBDIR", &found, Vnode::DIRECTORY));
    FatFileSystem::File* dir = (FatFileSystem::File*)(found->fnode);
    EXPECT_STR_EQ("SUBDIR", dir->getName().c_str());
    EXPECT_EQ(4, dir->getChildlen()->size());
}

static void test_fatfs_read_file_subdir(const char* fileName, const char* expectedContent)
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    Vnode* subdir;
    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, "SUBDIR", &subdir, Vnode::DIRECTORY));

    ASSERT_EQ(M_OK, fat->lookup(subdir, fileName, &found, Vnode::REGULAR));
    io::Context c;
    c.offset = 0;
    c.size   = 256;
    const int len = strlen(expectedContent);
    EXPECT_EQ(M_OK, fat->read(found, &c));
    ASSERT_EQ(len, c.offset);
    ASSERT_EQ(len, c.resultSize);
    ASSERT_TRUE(c.memory != NULL);
    ASSERT_EQ(len, c.memory->Size);
    EXPECT_TRUE(memcmp(expectedContent, c.memory->Data, len) == 0);
}

static void test_fatfs_lookup_file_long_name()
{
    TestFatFS fs;
    FatFileSystem* fat = fs.get();
    Vnode* root = fat->getRoot();

    Vnode* subdir;
    Vnode* found;
    ASSERT_EQ(M_OK, fat->lookup(root, "SUBDIR", &subdir, Vnode::DIRECTORY));

    ASSERT_EQ(M_OK, fat->lookup(subdir, "this_is_very_long_file_name.txt", &found, Vnode::REGULAR));
}

static void test_fatfs_create_long_file_name(const char* filename)
{
    {
        TestFatFS fs;
        FatFileSystem* fat = fs.get();
        Vnode* root = fat->getRoot();

        Vnode* subdir;
        ASSERT_EQ(M_OK, fat->lookup(root, "SUBDIR", &subdir, Vnode::DIRECTORY));
        createEmptyfile(fat, subdir, filename);
    }
    {
        TestFatFS fs;
        FatFileSystem* fat = fs.get();
        Vnode* root = fat->getRoot();

        Vnode* subdir;
        Vnode* found;
        ASSERT_EQ(M_OK, fat->lookup(root, "SUBDIR", &subdir, Vnode::DIRECTORY));

        ASSERT_EQ(M_OK, fat->lookup(subdir, filename, &found, Vnode::REGULAR));
    }

}

static void test_fatfs_create_long_file_name_need_new_cluster()
{
    int MAX_ENTRY_PER_CLUSTER;
    {
        // this test is depend on cluster size
        TestFatFS fs;
        FatFileSystem* fat = fs.get();
        ASSERT_EQ(512, fat->getClusterSizeByte());
        const int ENTRY_SIZE = 32;
        MAX_ENTRY_PER_CLUSTER = fat->getClusterSizeByte() / ENTRY_SIZE;
    }
    // we force long filname clustered into two clusters.
    for (int i = 0; i < MAX_ENTRY_PER_CLUSTER; i++) {
        char buf[64];
        sprintf(buf, "hi_i_am_higepon_writing_fat_fs.mosh.sls%d", i);
        test_fatfs_create_long_file_name(buf);
    }
}

static void testReadDirectory_OneFile()
{
    monapi_cmemoryinfo* ci = monapi_file_read_directory("/USER");

    int size = *(int*)ci->Data;

    EXPECT_EQ(2, size);
}

static void test_fatfs_monapi_open()
{
    intptr_t file = monapi_file_open("/USER/BIN/SHOW-WORDS.SCM", 0);
    ASSERT_TRUE(file > 0);
    monapi_file_close(file);
}

static void test_fatfs_truncate_and_write()
{
    {
        intptr_t file = monapi_file_open("/USER/BIN/TMP.TXT", FILE_CREATE);
        ASSERT_TRUE(file > 0);
        monapi_file_close(file);
    }
    {
        intptr_t file = monapi_file_open("/USER/BIN/TMP.TXT", FILE_TRUNCATE);
        ASSERT_TRUE(file > 0);
        monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
        monapi_cmemoryinfo_create(buffer, 1, 0, 0);
        for (int i = 0; i <= 0xff; i++) {
            buffer->Data[0] = i;
            EXPECT_EQ(1, monapi_file_write(file, buffer, 1));
        }
        monapi_file_close(file);
    }
    {
        monapi_cmemoryinfo* cmi = monapi_file_read_all("/USER/BIN/TMP.TXT");
        ASSERT_TRUE(cmi != NULL);
        ASSERT_EQ(256, cmi->Size);
        EXPECT_EQ(0xff, cmi->Data[0xff]);
        EXPECT_EQ(0x00, cmi->Data[0x00]);
        monapi_cmemoryinfo_dispose(cmi);
        monapi_cmemoryinfo_delete(cmi);
        monapi_file_delete("/USER/BIN/TMP.TXT");
    }
            // todo over cluster
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
    test_fatfs_write_file_overwrite();        // depends on test_fatfs_write_file()
    test_fatfs_write_file_overwrite_expand(); // depends on test_fatfs_write_file_overwrite()
    test_fatfs_delete_file();
    test_fatfs_truncate();
    test_fatfs_readdir();
    test_fatfs_lookup_subdir();
    test_fatfs_read_file_subdir("SUBDIR.TXT", "Hello\n");
    test_fatfs_lookup_file_long_name();
    test_fatfs_read_file_subdir("this_is_very_long_file_name.txt", "Hello World\n");
    test_fatfs_create_long_file_name("hi_i_am_higepon_writing_fat_fs.mosh.sls");
    test_fatfs_create_long_file_name_need_new_cluster();
    test_fatfs_monapi_open();
    test_fatfs_truncate_and_write();
    testReadDirectory_OneFile();


    TEST_RESULTS(file);
    return 0;
}
