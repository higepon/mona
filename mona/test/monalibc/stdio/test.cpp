#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdio.h>

#define TEST_TXT "/APPS/TSTDIO.APP/TEST.TXT"

int test1();
int test2();
int test3();
int test4();
int test5();

int main(int argc, char* argv[])
{
    if (argc > 1) {
        printf(argv[1]);
        return -2;
    } else {
        test1();
        test2();
        test3();
        test4();
        test5();
        TEST_RESULTS(stdio);
        return 0;
    }
}

int test1()
{
    FILE* f = fopen(TEST_TXT, "rb");
    EXPECT_TRUE(NULL != f);
    char buf[32];
    uint32_t size = fread(buf, 1, 32, f);
    EXPECT_EQ('A', buf[0]);
    EXPECT_EQ(buf[31], 'F');
    EXPECT_EQ(size, 32);
    fclose(f);
}

int test2()
{
    FILE* f = fopen(TEST_TXT, "rb");
    ASSERT_TRUE(f != NULL);
    ASSERT_TRUE(-1 != fseek(f, 0, SEEK_END));
    size_t size = ftell(f);
    EXPECT_EQ(size, 44);
    fclose(f);
}

int test3()
{
    FILE* f = fopen(TEST_TXT, "rb");
    ASSERT_TRUE(NULL != f);
    EXPECT_EQ(fgetc(f), 'A');
    EXPECT_EQ(fgetc(f), '1');
    EXPECT_EQ(fgetc(f), '2');
    EXPECT_EQ(fgetc(f), '3');
    EXPECT_EQ(fgetc(f), '4');
    EXPECT_EQ(fgetc(f), '5');
    fclose(f);
}

int test4()
{
    FILE* f = fopen(TEST_TXT, "rb");
    ASSERT_TRUE(NULL != f);
    ASSERT_TRUE(-1 != fseek(f, 0, SEEK_END));
    size_t size = ftell(f);
    EXPECT_EQ(size, 44);
    char* buffer = new char[size + 1];
    ASSERT_TRUE(NULL != buffer);
    fseek(f, 0, SEEK_SET);
    EXPECT_EQ(fread(buffer, 1, size, f), 44);
    EXPECT_EQ(buffer[0], 'A');
    EXPECT_EQ(buffer[31], 'F');
    delete[] buffer;
    fclose(f);
}

int test5()
{
    EXPECT_EQ(fileno(stdout), fileno(stdout));
    EXPECT_EQ(fileno(stderr), fileno(stderr));
}
