#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <time.h>
#include "MemoryManager.h"
#include "types.h"
#include "string.h"


#define MANAGE_SIZE (10 * 1024 * 1024)
#define RANDOMIZE() srand(time(NULL))
#define RANDOM(x) (rand() % (x) + 1)


typedef unsigned int dword;
typedef unsigned char byte;

void test1(int id, dword start, dword end, int size);
void test2(int id, dword start, dword end, int size);
void test3(int id, dword start, dword end);
void test4(int id, dword start, dword end);
void exitMessage(int id, const char* message);

char* ltona(long value, char* str, int n, int base) {

    static const char xdigit[] = {"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    unsigned long ulvalue;
    unsigned int ubase;
    int sign = 0;
    char* ps = str + n;

    *ps = '\0';
    if (base < 0) {
        ubase = -base;
        if (value < 0) {
            value = -value;
            sign  = -1;
            --n;
        }
    } else {
        ubase = base;
    }

    ulvalue  = value;
    if (n > 0 && (2 <= ubase && ubase <  sizeof(xdigit))) {
        do {
            *--ps = xdigit[(unsigned)(ulvalue % ubase)];
        } while (--n > 0 && (ulvalue /= ubase) != 0);
    }
    if (sign < 0)
        *--ps = '-';
    while (--n >= 0)
        *--ps = '0';
    return str;
}

using namespace std;

/*
    main()
*/
int main(int argc, char** argv) {

    /* ltona */
    char buf[512];
    long d = 0xFa;
    ltona(d, buf, 8, 16);
    printf("%s", buf);
    exit(0);


    int id = 0;
    byte* memory = new byte[MANAGE_SIZE];

    if (memory == NULL) {
        exitMessage(0, "memory allocate");
    }

    RANDOMIZE();

    /* test1 */
    for (int i = 1; i < 1025; i++) {
        test1(2, (dword)memory, MANAGE_SIZE + (dword)memory, i);
    }

    /* test2 */
    for (int i = 1; i < 1025; i++) {
        test2(2, (dword)memory, MANAGE_SIZE + (dword)memory, i);
    }

    /* test3 */
    for (int i = 1; i < 10000; i++) {
        test3(3, (dword)memory, MANAGE_SIZE + (dword)memory);
    }

    /* test4 */
    test4(4, (dword)memory, MANAGE_SIZE + (dword)memory);

    delete memory;
    return 0;
}

void exitMessage(int id, const char* message) {

    printf("test[%d] errror:%s\n", id, message);
    exit(-1);
}

void test1(int id, dword start, dword end, int size) {

    MemoryManager mm;
    mm.initialize(start, end);
    vector<void*> v;

    /* free memory(original) */
    dword freeSize = mm.getFreeMemorySize();

    dword freeSz = freeSize;

    for (int i = 0; i < 50; i++) {

        void* p = mm.allocate(size);

        if ((dword)p >= end || (dword)p < start) {
            exitMessage(id, "range error");
        }

        freeSz -= sizeof(MemoryEntry) + size;
        if (mm.getFreeMemorySize() != freeSz) {
            exitMessage(id, "size unmatch");
        }

        v.push_back(p);
    }

    for (int i = 0; i < v.size(); i++) {
        mm.free(v.at(i));
    }

    if (freeSize != mm.getFreeMemorySize()) {
        exitMessage(id, "size error");
    }

    printf("test1 OK\n");
}

void test2(int id, dword start, dword end, int size) {

    MemoryManager mm;
    mm.initialize(start, end);
    vector<void*> v;

    /* free memory(original) */
    dword freeSize = mm.getFreeMemorySize();

    dword freeSz = freeSize;

    for (int i = 0; i < 50; i++) {

        void* p = mm.allocate(size);

        if ((dword)p >= end || (dword)p < start) {
            exitMessage(id, "range error");
        }

        freeSz -= sizeof(MemoryEntry) + size;
        if (mm.getFreeMemorySize() != freeSz) {
            exitMessage(id, "size unmatch");
        }

        v.push_back(p);
    }

    for (int i = v.size() - 1; i >= 0; i--) {
        mm.free(v.at(i));
    }

    if (freeSize != mm.getFreeMemorySize()) {
        exitMessage(id, "size error");
    }

    printf("test2 OK\n");
}

void test3(int id, dword start, dword end) {

    MemoryManager mm;
    mm.initialize(start, end);
    vector<void*> v;
    dword size;

    /* free memory(original) */
    dword freeSize = mm.getFreeMemorySize();
    dword freeSz   = freeSize;

    for (int i = 0; i < 100; i++) {

        size = RANDOM(1024);
        void* p = mm.allocate(size);

        if ((dword)p >= end || (dword)p < start) {
            exitMessage(id, "range error");
        }

        freeSz -= sizeof(MemoryEntry) + size;
        if (mm.getFreeMemorySize() != freeSz) {
            exitMessage(id, "size unmatch");
        }

        v.push_back(p);
    }

    for (int i = 0; i < v.size(); i++) {
        mm.free(v.at(i));
    }

    if (freeSize != mm.getFreeMemorySize()) {
        exitMessage(id, "size error");
    }

    printf("test3 OK\n");
}


void test4(int id, dword start, dword end) {

    MemoryManager mm;
    mm.initialize(start, end);

    /* free memory(original) */
    dword freeSize = mm.getFreeMemorySize();
    dword freeSz = freeSize;

    void* p = mm.allocate(0x200000);

    if ((dword)p >= end || (dword)p < start) {
        exitMessage(id, "range error");
    }

    freeSz -= sizeof(MemoryEntry) + 0x200000;
    if (mm.getFreeMemorySize() != freeSz) {
        exitMessage(id, "size unmatch");
    }

    mm.free(p);

    if (freeSize != mm.getFreeMemorySize()) {
        exitMessage(id, "size error");
    }

    printf("test4 OK\n");
}
