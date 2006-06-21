#include "FileServerTest.h"
#include "Message.h"
#include "monapi/messages.h"

CPPUNIT_TEST_SUITE_REGISTRATION(FileServerTest);

using namespace MonAPI;

void FileServerTest::setUp()
{
    Message::initialize();
}

void FileServerTest::tearDown()
{
}

void FileServerTest::testFat12ReadDirectory()
{
//     int ret = monapi_call_change_drive(DRIVE_FD0, MONAPI_FALSE);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("change drive to fd0", ret, (int)MONA_SUCCESS);

//     monapi_cmemoryinfo* mi = monapi_call_file_read_directory("/", MONAPI_FALSE);
//     monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
//     int size = *(int*)mi->Data;
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("entry count", 11, size);
//     CPPUNIT_ASSERT_MESSAGE("file MONA.CFG exist?", strcmp((&p[0])->name, "MONA.CFG") == 0);
//     CPPUNIT_ASSERT_MESSAGE("file SERVERS exist?", strcmp((&p[10])->name, "SERVERS") == 0);
}

void FileServerTest::testFat12ReadFile()
{
//     int ret = monapi_call_change_drive(DRIVE_FD0, MONAPI_FALSE);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("change drive to fd0", ret, (int)MONA_SUCCESS);


//     monapi_cmemoryinfo* mi = monapi_call_file_read_data("/MONA.CFG", MONAPI_FALSE);

//     CPPUNIT_ASSERT_MESSAGE("file /MONA.CFG found?", NULL != mi);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("file size == 200byte", 200, (int)mi->Size);
//     CPPUNIT_ASSERT_MESSAGE("strncmp", strncmp("# 1600(x1200), 1280(x1024), 1024", (char*)mi->Data, 32) == 0);

//     monapi_cmemoryinfo_dispose(mi);
//     monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testFat12FileSize()
{
    // not implemented
    CPPUNIT_ASSERT_MESSAGE("dummy", 1);
}

void FileServerTest::testISO9660ReadDirectory()
{
//     int ret = monapi_call_change_drive(DRIVE_CD0, MONAPI_FALSE);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("change drive to cd0", ret, (int)MONA_SUCCESS);


//     monapi_cmemoryinfo* mi = monapi_call_file_read_directory("/SERVERS", MONAPI_FALSE);
//     monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
//     int size = *(int*)mi->Data;

//     CPPUNIT_ASSERT_EQUAL_MESSAGE("entry count", 7, size);
//     CPPUNIT_ASSERT_MESSAGE("file . exist?", strcmp((&p[0])->name, ".") == 0);
//     CPPUNIT_ASSERT_MESSAGE("file .. exist?", strcmp((&p[1])->name, "..") == 0);
//     CPPUNIT_ASSERT_MESSAGE("file SHELL.EX5 exist?", strcmp((&p[6])->name, "SHELL.EX5") == 0);
//     monapi_cmemoryinfo_dispose(mi);
//     monapi_cmemoryinfo_delete(mi);
}

void FileServerTest::testISO9660ReadFile()
{

    {
    monapi_cmemoryinfo* mi = monapi_call_file_read_directory2("/", MONAPI_FALSE);
    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    int size = *(int*)mi->Data;


    printf("%s %s:%d %s\n", __func__, __FILE__, __LINE__, p[0].name);fflush(stdout);

    dword fileID = monapi_call_file_open2("/CMD");
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

    int ret = monapi_call_file_seek2(fileID, 72, 0);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file seek", ret != MONA_FAILURE);

    mi = monapi_call_file_read_data2(fileID, 19);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
    printf("read %s", (char*)mi->Data);

    }


    // normal pattern
    dword fileID = monapi_call_file_open2("/MONA.CFG");
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID != MONA_FAILURE);

    int ret = monapi_call_file_seek2(fileID, 72, 0);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file seek", ret != MONA_FAILURE);

    monapi_cmemoryinfo* mi = monapi_call_file_read_data2(fileID, 19);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file read not null", mi != NULL);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file read", strncmp((char*)mi->Data, "VESA_RESOLUTION=800", 19) == 0);

    ret = monapi_call_file_close2(fileID);
    CPPUNIT_ASSERT_MESSAGE("iso9660 file close", ret != MONA_FAILURE);

    // error pattern 1
    fileID = monapi_call_file_open2("/NOTFOUND.TXT");
    CPPUNIT_ASSERT_MESSAGE("iso9660 file open", fileID == MONA_FAILURE);

    mi = monapi_call_file_read_directory2("/SERVERS", MONAPI_FALSE);
    monapi_directoryinfo* p = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
    int size = *(int*)mi->Data;


    CPPUNIT_ASSERT_EQUAL_MESSAGE("entry count", 7, size);
    CPPUNIT_ASSERT_MESSAGE("file MONA.CFG exist?", strcmp(p[0].name, ".") == 0);
    CPPUNIT_ASSERT_MESSAGE("file SERVERS exist?", strcmp((&p[6])->name, "SHELL.EX5") == 0);
}



//     int ret = monapi_call_change_drive(DRIVE_CD0, MONAPI_FALSE);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("change drive to cd0", ret, (int)MONA_SUCCESS);


//     monapi_cmemoryinfo* mi = monapi_call_file_read_data("/MONA.CFG", MONAPI_FALSE);

//     CPPUNIT_ASSERT_MESSAGE("file /MONA.CFG found?", NULL != mi);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("file size == 200byte", 200, (int)mi->Size);
//     CPPUNIT_ASSERT_MESSAGE("strncmp", strncmp("# 1600(x1200), 1280(x1024), 1024", (char*)mi->Data, 32) == 0);

//     monapi_cmemoryinfo_dispose(mi);
//     monapi_cmemoryinfo_delete(mi);

void FileServerTest::testISO9660FileSize()
{
//     int ret = monapi_call_change_drive(DRIVE_CD0, MONAPI_FALSE);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("change drive to cd0", ret, (int)MONA_SUCCESS);

//     dword id = monapi_call_file_open("/MONA.CFG");
//     dword size = monapi_call_file_get_file_size(id);
//     CPPUNIT_ASSERT_EQUAL_MESSAGE("file size == 200byte", 200, (int)size);
//     monapi_call_file_close(id);

}
