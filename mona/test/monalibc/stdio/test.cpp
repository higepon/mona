#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdio.h>

#define TEST_TXT "/APPS/TSTDIO.APP/TEST.TXT"
#define TEST_IMG "/APPS/TSTDIO.APP/TEST.JPG"

int test1();
int test2();
int test3();
int test4();
int test5();
void test_fwrite_small(); 
void test_fwrite_manytimes();
void test_fwrite_large();
void test_fread_biggerthanfile();
void test_fread_small_many();
void test_bsearch();

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
        test_fread_biggerthanfile();
        test_fread_small_many();
        test_fwrite_small();
        test_fwrite_manytimes();
        // slow test. but better than nothing...
        test_fwrite_large();
        test_bsearch();
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

void test_fread_biggerthanfile()
{
    char buf[1024];
    FILE* f = fopen(TEST_TXT, "rb");
    EXPECT_EQ(44, fread(buf, 1, 1024, f));
    EXPECT_EQ('A', buf[0]);
    EXPECT_EQ('F', buf[31]);
    fclose(f);
}

void test_fread_small_many()
{
    char buf[1024];
    FILE* f = fopen(TEST_TXT, "rb");
    EXPECT_EQ(1, fread(buf, 1, 1, f));
    EXPECT_EQ('A', buf[0]);
    EXPECT_EQ(1, fread(buf, 1, 1, f));
    EXPECT_EQ('1', buf[0]);
    EXPECT_EQ(1, fread(buf, 1, 1, f));
    EXPECT_EQ('2', buf[0]);
    fclose(f);
}

static int fileSize(const char* path)
{
    uint32_t id = monapi_file_open(path, false);
    int actual = monapi_file_get_file_size(id);
    monapi_file_close(id);
    return actual;
}

void test_fwrite_small() 
{
    const char* path = "/MEM/TEST.DAT";
    FILE* fp = fopen(path, "w");
    EXPECT_TRUE(fp != NULL);
    const char buf[] = "test";
    fwrite(buf, sizeof(char), sizeof(buf), fp);
    fclose(fp);

    EXPECT_EQ(sizeof(buf), fileSize(path));
    monapi_file_delete(path);
}


void test_fwrite_manytimes() 
{
    const char expected[] = "hello";

    const char* path = "/MEM/TEST.DAT";
    FILE* fp = fopen(path, "w");
    EXPECT_TRUE(fp != NULL);
    fwrite("h", sizeof(char), 1, fp);
    fwrite("e", sizeof(char), 1, fp);
    fwrite("l", sizeof(char), 1, fp);
    fwrite("l", sizeof(char), 1, fp);
    fwrite("o", sizeof(char), 1, fp);
    fclose(fp);


    char actual[256];
    fp = fopen(path, "r");
    EXPECT_TRUE(fp != NULL);
    uint32_t size = fread(actual, 1, 256, fp);
    EXPECT_EQ(size, sizeof(expected)-1);

    int i;
    for(i = 0; i < size; i++)
    {
        EXPECT_EQ(expected[i], actual[i]);
    }

    fclose(fp);
    monapi_file_delete(path);
}

static void copyFile(const char *from, const char* to)
{
    FILE* fp_from = fopen(from, "r");
    FILE* fp = fopen(to, "w");

    char buf[256];
    int readSize;
    readSize = fread(buf, 1, 256, fp_from);
    while(readSize > 0)
    {
        fwrite(buf, 1, readSize, fp);
        readSize = fread(buf, 1, 256, fp_from);
    }
    fclose(fp);
    fclose(fp_from);

}

static void expectFileEqual(const char* org, const char* to)
{
    char buf[256];
    char buf2[256];
    FILE* fp_org = fopen(org, "r");
    FILE* fp = fopen(to, "r");
 //for analyze   int debpos = 0;
    if(fp == NULL || fp_org == NULL)
      {
          fprintf(stderr, "fp=%x, fp_org=%x\n", fp, fp_org);
      }

    int readSize = fread(buf, 1, 256, fp_org);
    while(readSize > 0)
    {
        int readSize2 = fread(buf2, 1, readSize, fp);
        EXPECT_EQ(readSize, readSize2);
        EXPECT_TRUE(0 == memcmp(buf, buf2, readSize2));
/*
        if(0 != memcmp(buf, buf2, readSize2))
        {
            for(int i = 0; i < readSize2; i++)
              {
                  if(buf[i] != buf2[i])
                  {
                      static int count = 0;
                      fprintf(stderr, "debpos=%x, i=%d, buf[i]=%x, buf2[i]=%x\n", debpos, i, buf[i], buf2[i]);
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

void test_fwrite_large() 
{
    const char* path = "/MEM/TEST.JPG";

    copyFile(TEST_IMG, path);
    expectFileEqual(TEST_IMG, path);

//    copyFile("/APPS/W3M/W3M.APP/EUCMAN.JPG", path);
//    expectFileEqual("/MEM/T1110.JPG", "/APPS/W3M/W3M.APP/EUCMAN.JPG");

    monapi_file_delete(path);
}


// bsearch
#include <stdlib.h>

static int cmp_int(const void* key, const void*target)
{
    int keyi = ((int)key);
    int targeti = *((int *)target);
    return keyi-targeti;
}

void test_bsearch()
{
    int arrays[] = {
        1, 3, 5, 7
    };

    void* actual = bsearch((void*)2, (void*)arrays, 4, sizeof(int), cmp_int);
    EXPECT_TRUE(actual == NULL);

    actual = bsearch((void*)3, (void*)arrays, 4, sizeof(int), cmp_int);
    EXPECT_TRUE(actual == &arrays[1]);

    actual = bsearch((void*)5, (void*)arrays, 4, sizeof(int), cmp_int);
    EXPECT_TRUE(actual == &arrays[2]);

    actual = bsearch((void*)7, (void*)arrays, 4, sizeof(int), cmp_int);
    EXPECT_TRUE(actual == &arrays[3]);

    actual = bsearch((void*)8, (void*)arrays, 4, sizeof(int), cmp_int);
    EXPECT_TRUE(actual == NULL);

    actual = bsearch((void*)1, (void*)arrays, 4, sizeof(int), cmp_int);
    EXPECT_TRUE(actual == &arrays[0]);

    // once crashed this case.
    actual = bsearch((void*)0, (void*)arrays, 4, sizeof(int), cmp_int);
    EXPECT_TRUE(actual == NULL);
}
