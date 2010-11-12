// -*- Mode: CPP ; Coding: utf-8 -*-
#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi.h>
#include <stdio.h>
#include <limits.h>
#include <string>

using namespace MonAPI;

#define MAP_FILE_PATH "/APPS/TFLOAD.APP/TFLOAD.MAP"

static void open_close_many_times(const char* path)
{
    const int N = 100000;
    for (int i = 0; i < N; i++) {
        intptr_t file = monapi_file_open(path, 0);
        ASSERT_TRUE(file > 0);
        monapi_file_close(file);
    }
}

static void read_head_byte_many_times(const char* path, uint8_t expected)
{
    const int N = 10000;
    for (int i = 0; i < N; i++) {
        if (i % 10 == 0) {
            logprintf(".");
        }
        if (i % 100 == 0) {
            logprintf("\n");
        }
        if (i % 1000 == 0) {
            logprintf("\n1000\n");
        }

        intptr_t file = monapi_file_open(path, 0);
        ASSERT_TRUE(file > 0);
        scoped_ptr<SharedMemory> shm(monapi_file_read(file, 1));
        EXPECT_EQ(1, shm->size());
        EXPECT_EQ(expected, shm->data()[0]);
        monapi_file_close(file);
    }
}

int main(int argc, char *argv[])
{
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        monapi_fatal("syscall_stack_trace_enable failed%d\n", ret);
    }

    const char* FAT32_FILE = "/USER/BIN/SHOW-WORDS.SCM";
    const char* ISO9660_FILE = "/APPS/IME.EX5";

    open_close_many_times(FAT32_FILE);
    open_close_many_times(ISO9660_FILE);
    read_head_byte_many_times(FAT32_FILE, 0x3b);
    TEST_RESULTS(file_load);
    return 0;
}
