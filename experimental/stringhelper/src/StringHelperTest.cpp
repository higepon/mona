#include <string>
#include "FileServerTest.h"
#include "Message.h"
#include "monapi/messages.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FileServerTest);

using namespace MonAPI;
using namespace std;

void FileServerTest::setUp()
{
    Message::initialize();
}

void FileServerTest::tearDown()
{
}

void FileServerTest::testProcessReadDirectoryOnce()
{
    // readdir OK
    monapi_cmemoryinfo* mi = monapi_file_read_directory("/process");
    CPPUNIT_ASSERT_MESSAGE("readdir /process mi != NULL", mi != NULL);

    // there may be at least 3 processes.
    int size = *(int*)mi->Data;
    CPPUNIT_ASSERT_MESSAGE("there may be at least 3 processes.", size >= 3);

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testProcessCreateFile()
{
    // don't create file because read only
    dword fileID = monapi_file_open("/process/DUMMY.TXT", MONAPI_TRUE);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID == MONA_FAILURE);
}

void FileServerTest::testProcessReadDirectory()
{
    // first lookup
    testProcessReadDirectoryOnce();

    // cache lookup
    testProcessReadDirectoryOnce();
}

void FileServerTest::testProcessReadFile()
{
    // first lookup
    testProcessReadFileOnce();

    // cache lookup
    testProcessReadFileOnce();
}

void FileServerTest::testProcessWriteFile()
{
}

void FileServerTest::testProcessReadFileOnce()
{
    // readdir OK
    monapi_cmemoryinfo* mi = monapi_file_read_directory("/process");
    CPPUNIT_ASSERT_MESSAGE("readdir /process mi != NULL", mi != NULL);

    // there may be 3 processes.
    int size = *(int*)mi->Data;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("there may be 3 processes.", size, 3);

    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    string path = "/process/";
    path += p[1].name;

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);

    // file open
    dword fileID = monapi_file_open(path.c_str(), MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("process file open", fileID != MONA_FAILURE);

    // file read
    monapi_cmemoryinfo* mi2 = monapi_file_read(fileID, 20);
    CPPUNIT_ASSERT_MESSAGE("read /process/file mi2 != NULL", mi2 != NULL);
    CPPUNIT_ASSERT_MESSAGE("read size /process/file", mi2->Size == sizeof(dword));

    // contain pid?
    for (dword i = 0; i < mi2->Size; i++)
    {
        CPPUNIT_ASSERT_MESSAGE("process file has pid. is Numeric?", isdigit(mi->Data[i]) == 0);
    }

    CPPUNIT_ASSERT_EQUAL_MESSAGE("proces file size", 0, (int)monapi_file_get_file_size(fileID));

    int ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("process file close", ret, (int)MONA_SUCCESS);

    monapi_cmemoryinfo_dispose(mi2);
    monapi_cmemoryinfo_delete(mi2);
}

void FileServerTest::testISO9660ReadDirectory()
{
    // readdir OK
    monapi_cmemoryinfo* mi = monapi_file_read_directory("/SERVERS");
    CPPUNIT_ASSERT_MESSAGE("readdir /SERVERS mi != NULL", mi != NULL);
    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    int size = *(int*)mi->Data;

    CPPUNIT_ASSERT_EQUAL_MESSAGE("entry count", 8, size);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file MONA.CFG exist?", strcmp(p[0].name, "."), 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file SERVERS exist?", strcmp((&p[7])->name, "SHELL.EX5"), 0);

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);

    // once more
    mi = monapi_file_read_directory("/APPS/HIGEPON");
    CPPUNIT_ASSERT_MESSAGE("readdir /APPS/HIGEPON mi != NULL", mi != NULL);
    p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    size = *(int*)mi->Data;

    CPPUNIT_ASSERT_EQUAL_MESSAGE("entry count", 3, size);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file . exist?", strcmp(p[0].name, "."), 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file .. exist?", strcmp(p[1].name, ".."), 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file README.TXT exist?", strcmp(p[2].name, "README.TXT"), 0);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);

    // add 2006/09/15 by higepon
    mi = monapi_file_read_directory("/APPS/");
    CPPUNIT_ASSERT_MESSAGE("readdir /APPS/ mi != NULL", mi != NULL);
    p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    CPPUNIT_ASSERT_EQUAL_MESSAGE("directory name HIGEPON is OK?", strcmp(p[7].name, "HIGEPON"), 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("directory name HIGEPON is OK?", strcmp(p[17].name, "MONAPPT.APP"), 0);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testISO9660ReadDirectoryNG()
{
    // readdir NG
    monapi_cmemoryinfo* mi = monapi_file_read_directory("/HUGA/HOGE");
    CPPUNIT_ASSERT_MESSAGE("readdir /HUGA/HOGE mi == NULL", mi == NULL);
}

void FileServerTest::testISO9660WriteFile()
{
    dword fileID = monapi_file_open("/MONA.CFG", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("ISO9660 file open", fileID != MONA_FAILURE);
    string data = "Hello Write\n";

    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
    monapi_cmemoryinfo_create(mi, data.size(), MONAPI_FALSE);

    // write
    memcpy((void*)mi->Data, data.c_str(), mi->Size);
    int ret = monapi_file_write(fileID, mi, mi->Size);
    CPPUNIT_ASSERT_MESSAGE("ISO9660 Write file", ret != MONA_SUCCESS);

    // close
    ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file close", ret != MONA_FAILURE);
}

void FileServerTest::testISO9660ReadFileSize()
{
    dword fileID = monapi_file_open("/MONA.CFG", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

    // actual size = 200
    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 203);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file read", (int)mi->Size, 200);

    int ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file close", ret != MONA_FAILURE);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testISO9660ReadFile()
{
    // normal pattern 1
    {
        dword fileID = monapi_file_open("/MONA.CFG", MONAPI_FALSE);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

        int ret = monapi_file_seek(fileID, 72, 0);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file seek", ret != MONA_FAILURE);

        monapi_cmemoryinfo* mi = monapi_file_read(fileID, 19);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file read", strncmp((char*)mi->Data, "VESA_RESOLUTION=800", 19), 0);

        ret = monapi_file_close(fileID);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file close", ret != MONA_FAILURE);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }

    // normal pattern 2
    {
        dword fileID = monapi_file_open("/APPS/HIGEPON/README.TXT", MONAPI_FALSE);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

        int ret = monapi_file_seek(fileID, 0, 0);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file seek", ret != MONA_FAILURE);

        monapi_cmemoryinfo* mi = monapi_file_read(fileID, 10);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file read", strncmp((char*)mi->Data, "TEST", 4), 0);

        ret = monapi_file_close(fileID);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file close", ret != MONA_FAILURE);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }

    // use read_data pattern
    {
        monapi_cmemoryinfo* mi = monapi_file_read_all("/MONA.CFG");
        CPPUNIT_ASSERT_MESSAGE("iso9660 file read_data not null", mi != NULL);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file read_data", strncmp((char*)mi->Data, "# 1600(x1200), 1280", 19), 0);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }
}

void FileServerTest::testISO9660ReadFileNG()
{
    {
        dword fileID = monapi_file_open("/NONE.TXT", MONAPI_FALSE);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file open", (int)fileID, (int)MONA_FAILURE);
    }

    {
        dword fileID = monapi_file_open("/APPS/NONE.TXT", MONAPI_FALSE);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file open", (int)fileID, (int)MONA_FAILURE);
    }
}


void FileServerTest::testISO9660FileSize()
{

    dword fileID = monapi_file_open("/MONA.CFG", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

    int ret = monapi_file_seek(fileID, 72, 0);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file seek", ret != MONA_FAILURE);

    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 19);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file read", strncmp((char*)mi->Data, "VESA_RESOLUTION=800", 19), 0);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file size", 200, (int)monapi_file_get_file_size(fileID));

    ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file close", ret != MONA_FAILURE);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testISO9660CreateFile()
{
    // don't create exist file
    dword fileID = monapi_file_open("/MONA.CFG", MONAPI_TRUE);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID == MONA_FAILURE);

    // don't create file because read only
    fileID = monapi_file_open("/DUMMY.TXT", MONAPI_TRUE);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID == MONA_FAILURE);

    // don't create file because read only
    fileID = monapi_file_open("/SERVERS/DUMMY.TXT", MONAPI_TRUE);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID == MONA_FAILURE);
}

void FileServerTest::testFAT12WriteFile()
{
    dword fileID = monapi_file_open("/fd/WRITE.TXT", MONAPI_TRUE);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file open", fileID != MONA_FAILURE);
    string data = "Hello Write\n";

    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();
    monapi_cmemoryinfo_create(mi, data.size(), MONAPI_FALSE);

    // write
    memcpy((void*)mi->Data, data.c_str(), mi->Size);
    int ret = monapi_file_write(fileID, mi, mi->Size);
    CPPUNIT_ASSERT_MESSAGE("FAT12 Write file", ret == MONA_SUCCESS);

    // seek
    ret = monapi_file_seek(fileID, 3, 0);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file seek", ret != MONA_FAILURE);

///    mi = monapi_cmemoryinfo_new();
//    monapi_cmemoryinfo_create(mi, data.size(), MONAPI_FALSE);

    // rewrite
    memcpy((void*)mi->Data, data.c_str(), mi->Size);
    ret = monapi_file_write(fileID, mi, mi->Size);
    CPPUNIT_ASSERT_MESSAGE("FAT12 Write file", ret == MONA_SUCCESS);

    // close
    ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file close", ret != MONA_FAILURE);

    // open
    fileID = monapi_file_open("/fd/WRITE.TXT", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file open", fileID != MONA_FAILURE);

    mi = monapi_file_read(fileID, 15);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file read not null", mi != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("FAT 12file read", strncmp((char*)mi->Data, "HelHello Write", 14), 0);

    ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file close", ret != MONA_FAILURE);
//    monapi_cmemoryinfo_dispose(mi);
//    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testFAT12ReadFile()
{
    dword fileID = monapi_file_open("/fd/MONA.CFG", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file open", fileID != MONA_FAILURE);

    int ret = monapi_file_seek(fileID, 72, 0);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file seek", ret != MONA_FAILURE);

    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 19);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file read not null", mi != NULL);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("FAT12 file read", strncmp((char*)mi->Data, "VESA_RESOLUTION=800", 19), 0);

    ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file close", ret != MONA_FAILURE);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testFAT12FileSize()
{
    dword fileID = monapi_file_open("/fd/MONA.CFG", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file open", fileID != MONA_FAILURE);

    int ret = monapi_file_seek(fileID, 72, 0);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file seek", ret != MONA_FAILURE);

    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 19);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file read not null", mi != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("FAT12 file read", strncmp((char*)mi->Data, "VESA_RESOLUTION=800", 19), 0);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("FAT12 file size", 200, (int)monapi_file_get_file_size(fileID));

    ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file close", ret != MONA_FAILURE);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testFAT12ReadDirectory()
{
    // readdir OK
    {
        monapi_cmemoryinfo* mi = monapi_file_read_directory("/fd/SERVERS");
        CPPUNIT_ASSERT_MESSAGE("readdir /SERVERS mi != NULL", mi != NULL);
        monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];

        CPPUNIT_ASSERT_EQUAL_MESSAGE("file . exist?", strcmp(p[0].name, "."), 0);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("file SERVERS/SHELL.EX5 exist?", strcmp((&p[7])->name, "SHELL.EX5"), 0);

        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }

    {
        monapi_cmemoryinfo* mi = monapi_file_read_directory("/fd/SERVERS");
        CPPUNIT_ASSERT_MESSAGE("readdir /SERVERS mi != NULL", mi != NULL);
        monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];

        CPPUNIT_ASSERT_EQUAL_MESSAGE("file . exist?", strcmp(p[0].name, "."), 0);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("file SERVERS/SHELL.EX5 exist?", strcmp((&p[7])->name, "SHELL.EX5"), 0);

        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }

}

void FileServerTest::testFAT12ReadDirectoryNG()
{
    // readdir NG
    monapi_cmemoryinfo* mi = monapi_file_read_directory("/fd/HUGA/HOGE");
    CPPUNIT_ASSERT_MESSAGE("readdir /fd/HUGA/HOGE mi == NULL", mi == NULL);
}

void FileServerTest::testFAT12ReadFileNG()
{
    {
        dword fileID = monapi_file_open("/fd/NONE.TXT", MONAPI_FALSE);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("FAT12 file open", (int)fileID, (int)MONA_FAILURE);
    }

    {
        dword fileID = monapi_file_open("/fd/APPS/NONE.TXT", MONAPI_FALSE);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("FAT12 file open", (int)fileID, (int)MONA_FAILURE);
    }
}

void FileServerTest::testFAT12CreateFile()
{
    dword fileID = monapi_file_open("/fd/DUMMY.TXT", MONAPI_TRUE);
    CPPUNIT_ASSERT_MESSAGE("FAT12 file open", fileID != MONA_FAILURE);
   monapi_file_close(fileID);
}

void FileServerTest::testFAT12ReadFileSize()
{
    dword fileID = monapi_file_open("/fd/MONA.CFG", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("fat12 file open", fileID != MONA_FAILURE);

    // actual size = 200
    monapi_cmemoryinfo* mi = monapi_file_read(fileID, 203);
    CPPUNIT_ASSERT_MESSAGE("fat12 file read not null", mi != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("fat12 file read", (int)mi->Size, 200);

    int ret = monapi_file_close(fileID);
    CPPUNIT_ASSERT_MESSAGE("fat12 file close", ret != MONA_FAILURE);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testST5()
{
    monapi_cmemoryinfo* mi = monapi_file_decompress_st5_file("/ST5TEST.TX5");
    CPPUNIT_ASSERT_MESSAGE("monapi_file_decompress_st5_file fail", mi != NULL);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("monapi_file_decompress_st5_file", strncmp((char*)mi->Data, "Hello!", 6), 0);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}
