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

void FileServerTest::testProcessReadDirectory()
{
    // readdir OK
    monapi_cmemoryinfo* mi = monapi_call_file_read_directory2("/process", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("readdir /process mi != NULL", mi != NULL);

    // there may be at least 3 processes.
    int size = *(int*)mi->Data;
    CPPUNIT_ASSERT_MESSAGE("there may be at least 3 processes.", size >= 3);

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testProcessReadFile()
{
    // readdir OK
    monapi_cmemoryinfo* mi = monapi_call_file_read_directory2("/process", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("readdir /process mi != NULL", mi != NULL);

    // there may be at least 3 processes.
    int size = *(int*)mi->Data;
    CPPUNIT_ASSERT_MESSAGE("there may be at least 3 processes.", size >= 3);

    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    string path = "/process/";
    path += p[0].name;

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
    printf("%s %s:%d%s\n", __func__, __FILE__, __LINE__, path.c_str());fflush(stdout);
    // file open
    dword fileID = monapi_call_file_open2(path.c_str());
    CPPUNIT_ASSERT_MESSAGE("process file open", fileID != MONA_FAILURE);

    // file read
    monapi_cmemoryinfo* mi2 = monapi_call_file_read_data2(fileID, 20);
    CPPUNIT_ASSERT_MESSAGE("read /process/file mi2 != NULL", mi2 != NULL);

    // contain pid?
    for (dword i = 0; i < mi2->Size; i++)
    {
        CPPUNIT_ASSERT_MESSAGE("process file has pid. is Numeric?", isdigit(mi->Data[i]) == 0);
    }

    monapi_cmemoryinfo_dispose(mi2);
    monapi_cmemoryinfo_delete(mi2);
}

void FileServerTest::testProcessFileSize()
{
    // not implemented
    CPPUNIT_ASSERT_MESSAGE("dummy", 1);
}

void FileServerTest::testISO9660ReadDirectory()
{
    // readdir OK
    monapi_cmemoryinfo* mi = monapi_call_file_read_directory2("/SERVERS", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("readdir /SERVERS mi != NULL", mi != NULL);
    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    int size = *(int*)mi->Data;

    CPPUNIT_ASSERT_EQUAL_MESSAGE("entry count", 7, size);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file MONA.CFG exist?", strcmp(p[0].name, "."), 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file SERVERS exist?", strcmp((&p[6])->name, "SHELL.EX5"), 0);

    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);

    // once more
    mi = monapi_call_file_read_directory2("/APPS/HIGEPON", MONAPI_FALSE);
    CPPUNIT_ASSERT_MESSAGE("readdir /APPS/HIGEPON mi != NULL", mi != NULL);
    p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    size = *(int*)mi->Data;

    CPPUNIT_ASSERT_EQUAL_MESSAGE("entry count", 3, size);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file . exist?", strcmp(p[0].name, "."), 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file .. exist?", strcmp(p[1].name, ".."), 0);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("file README.TXT exist?", strcmp(p[2].name, "README.TXT"), 0);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testISO9660ReadDirectoryNG()
{
    // readdir NG
    monapi_cmemoryinfo* mi = monapi_call_file_read_directory2("/HUGA/HOGE", MONAPI_FALSE);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("readdir /HUGA/HOGE mi == NULL", (int)mi, NULL);
}


void FileServerTest::testISO9660ReadFile()
{
    // normal pattern 1
    {
        dword fileID = monapi_call_file_open2("/MONA.CFG");
        CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

        int ret = monapi_call_file_seek2(fileID, 72, 0);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file seek", ret != MONA_FAILURE);

        monapi_cmemoryinfo* mi = monapi_call_file_read_data2(fileID, 19);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file read", strncmp((char*)mi->Data, "VESA_RESOLUTION=800", 19), 0);

        ret = monapi_call_file_close2(fileID);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file close", ret != MONA_FAILURE);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }

    // normal pattern 2
    {
        dword fileID = monapi_call_file_open2("/APPS/HIGEPON/README.TXT");
        CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

        int ret = monapi_call_file_seek2(fileID, 0, 0);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file seek", ret != MONA_FAILURE);

        monapi_cmemoryinfo* mi = monapi_call_file_read_data2(fileID, 10);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file read", strncmp((char*)mi->Data, "TEST", 4), 0);

        ret = monapi_call_file_close2(fileID);
        CPPUNIT_ASSERT_MESSAGE("iso9660 file close", ret != MONA_FAILURE);
        monapi_cmemoryinfo_dispose(mi);
        monapi_cmemoryinfo_delete(mi);
    }
}

void FileServerTest::testISO9660ReadFileNG()
{
    {
        dword fileID = monapi_call_file_open2("/NONE.TXT");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file open", (int)fileID, (int)MONA_FAILURE);
    }

    {
        dword fileID = monapi_call_file_open2("/APPS/NONE.TXT");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("iso9660 file open", (int)fileID, (int)MONA_FAILURE);
    }
}


void FileServerTest::testISO9660FileSize()
{
//     int ret = monapi_call_change_drive(DRIVE_CD0, MONAPI_FALSE);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("change drive to cd0", ret, (int)MONA_SUCCESS);

//     dword id = monapi_call_file_open("/MONA.CFG");
//     dword size = monapi_call_file_get_file_size(id);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("file size == 200byte", 200, (int)size);
//     monapi_call_file_close(id);

}
