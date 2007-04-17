#include <monapi.h>
#include <stdio.h>
#include "test.h"

#define TEST_TXT "/APPS/TEST.APP/TEST.TXT"

int test1();
int test2();
int test3();
int test4();
int test5();

int main(int argc, char* argv[])
{

    FILE* f = fopen("/SERVERS/TEST/test.scm", "rb");
    _printf("open ok? %s\n", f == NULL ? "ng" : "ok");


    if (argc > 1)
    {
        printf(argv[1]);
        return -2;
    }
    else
    {
        printf("**** File system test start ****\n");
        test1();
        test2();
        test3();
        test4();
        test5();
        printf("**** File system test end ****\n");
        return -1;
    }
}

int test1()
{
    FILE* f = fopen(TEST_TXT, "rb");
    if (NULL == f)
    {
        TEST_ERROR("can't open file");
    }
    char buf[32];
    uint32_t size = fread(buf, 1, 32, f);
    TEST_ASSERT_EQUAL(buf[0], 'A');
    TEST_ASSERT_EQUAL(buf[31], 'F');
    TEST_ASSERT_EQUAL(size, 32);
    fclose(f);
    TEST_SUCCESS();
}

int test2()
{
    FILE* f = fopen(TEST_TXT, "rb");
    if (NULL == f)
    {
        TEST_ERROR("can't open file");
    }
    if (-1 == fseek(f, 0, SEEK_END))
    {
        TEST_ERROR("can't fseek");
    }
    size_t size = ftell(f);
    TEST_ASSERT_EQUAL(size, 44);
    fclose(f);
    TEST_SUCCESS();
}

int test3()
{
    FILE* f = fopen(TEST_TXT, "rb");
    if (NULL == f)
    {
        TEST_ERROR("can't open file");
    }
    TEST_ASSERT_EQUAL(fgetc(f), 'A');
    TEST_ASSERT_EQUAL(fgetc(f), '1');
    TEST_ASSERT_EQUAL(fgetc(f), '2');
    TEST_ASSERT_EQUAL(fgetc(f), '3');
    TEST_ASSERT_EQUAL(fgetc(f), '4');
    TEST_ASSERT_EQUAL(fgetc(f), '5');
    fclose(f);
    TEST_SUCCESS();
}

int test4()
{
    FILE* f = fopen(TEST_TXT, "rb");
    if (NULL == f)
    {
        TEST_ERROR("can't open file");
    }
    if (-1 == fseek(f, 0, SEEK_END))
    {
        TEST_ERROR("can't fseek");
    }
    size_t size = ftell(f);
    TEST_ASSERT_EQUAL(size, 44);
    char* buffer = new char[size + 1];
    if (NULL == buffer)
    {
        TEST_ERROR("memory error");
    }
    fseek(f, 0, SEEK_SET);
    TEST_ASSERT_EQUAL(fread(buffer, 1, size, f), 44);
    TEST_ASSERT_EQUAL(buffer[0], 'A');
    TEST_ASSERT_EQUAL(buffer[31], 'F');
    delete[] buffer;
    fclose(f);
    TEST_SUCCESS();
}

int test5()
{
    TEST_ASSERT_EQUAL(fileno(stdout), fileno(stdout));
    TEST_ASSERT_EQUAL(fileno(stderr), fileno(stderr));
    TEST_SUCCESS();
}
