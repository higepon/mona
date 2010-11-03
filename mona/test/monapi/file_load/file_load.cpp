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

int main(int argc, char *argv[])
{
    uint32_t pid = syscall_get_pid();
    intptr_t ret = syscall_stack_trace_enable(pid, MAP_FILE_PATH);
    if (ret != M_OK) {
        monapi_fatal("syscall_stack_trace_enable failed%d\n", ret);
    }

    open_close_many_times("/USER/BIN/SHOW-WORDS.SCM"); // FAT32
    open_close_many_times("/APPS/IME.EX5");            // ISO9660
    TEST_RESULTS(file_load);
    return 0;
}
