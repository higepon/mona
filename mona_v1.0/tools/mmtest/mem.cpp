#include <stdlib.h>
#include <stdio.h>
#include "MemoryManager.h"
#include "types.h"

//#define printf      printf
#define MANAGE_SIZE (10 * 1024 * 1024)

typedef unsigned int dword;
typedef unsigned char byte;

void test(MemoryManager* mm);

/*
    main()
*/
int main(int argc, char* argv) {

    byte* memory = (byte*)malloc(MANAGE_SIZE);

    if (memory == NULL) {

        printf("memory allocate error\n");
        exit(-1);
    }

    static MemoryManager mm((dword)memory, (dword)memory + MANAGE_SIZE);

    test(&mm);

    free(memory);
    return 0;
}

void test(MemoryManager* mm) {

    /* this test code expects 10mb of memory is availble */
    dword testNumber = 1;

    /* test normal */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p = mm->allocate(8* 1024);
        mm->free(p);

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test normal */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p = mm->allocate(513);
        mm->free(p);

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test normal */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p = mm->allocate(5);
        mm->free(p);

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test 2 times allocate */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p1 = mm->allocate(5);
        dword p2 = mm->allocate(512);
        mm->free(p2);
        mm->free(p1);

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test 2 times allocate */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p1 = mm->allocate(2 * 1024 * 1024);
        dword p2 = mm->allocate(512);
        mm->free(p2);
        mm->free(p1);

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test 2 times allocate */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p1 = mm->allocate(2 * 1024 * 1024);
        dword p2 = mm->allocate(512);
        mm->free(p1);
        mm->free(p2);

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test 30 times allocate */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p[30];

        for (int i = 0; i < 30; i++) {

            p[i] = mm->allocate(100 * 1024);
            if (p[i] == (dword)NULL) {
                printf("test%d failed allocate error\n", testNumber);
                exit(-1);
            }
        }

        for (int i = 0; i < 30; i++) {

            mm->free(p[i]);
        }

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test 30 times allocate reverse */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p[30];

        for (int i = 0; i < 30; i++) {

            p[i] = mm->allocate(100 * 1024);
            if (p[i] == (dword)NULL) {
                printf("test%d failed allocate error\n", testNumber);
                exit(-1);
            }
        }

        for (int i = 29; i >= 0 ; i--) {

            mm->free(p[i]);
        }

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test 100 times allocate reverse */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p[100];

        for (int i = 0; i < 100; i++) {

            p[i] = mm->allocate(i + 1);
            if (p[i] == (dword)NULL) {
                printf("test%d failed allocate error\n", testNumber);
                exit(-1);
            }
        }

        for (int i = 99; i >= 0 ; i--) {

            mm->free(p[i]);
        }

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }

    /* test 500 times allocate reverse */
    {
        dword freeMememorySize = mm->getFreeMemorySize();
        dword usedMememorySize = mm->getUsedMemorySize();

        dword p;

        for (int i = 0; i < 500; i++) {

            p = mm->allocate(i + 1);
            mm->free(p);
        }

        if (freeMememorySize != mm->getFreeMemorySize() || usedMememorySize != mm->getUsedMemorySize()) {

            printf("test%d failed\n", testNumber);
            exit(-1);
        }
        printf("test%d OK\n", testNumber);
        testNumber++;
    }
}
