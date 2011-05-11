#include <monapi.h>
#include <limits.h>
#include <string>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static int executeMoshWithTime(const std::string& scriptPath)
{
    std::string command(MonAPI::System::getMoshPath());
    command += " ";
    command += scriptPath;
    uint32_t tid;
    uint64_t s1 = MonAPI::Date::nowInMsec();
    intptr_t result = monapi_process_execute_file_get_tid(
        command.c_str(),
        MONAPI_TRUE,
        &tid,
        MonAPI::System::getProcessStdinID(),
        MonAPI::System::getProcessStdoutID());
    if (result != 0) {
        EXPECT_TRUE(false);
        return false;
    }
    monapi_process_wait_terminated(tid);
    uint64_t s2 = MonAPI::Date::nowInMsec();
    return (int)(s2 - s1);
}

#define TEST_MOSH_PERFORMANCE(targetTime, script) \
{                                                 \
    int msec = executeMoshWithTime(script);       \
    if (msec > targetTime) {                      \
        logprintf("%s %dmsec\n", __func__, msec); \
    }                                             \
    EXPECT_TRUE(msec <= targetTime);              \
}

static void testEmptyScriptShouldRunQuickly()
{
    // Mona on KVM Thinkpad x60: 90msec
    TEST_MOSH_PERFORMANCE(1000, "/LIBS/MOSH/bin/empty.sps"); // warm up
    TEST_MOSH_PERFORMANCE(120, "/LIBS/MOSH/bin/empty.sps");
}

static void testManyImportDoesNotHaveAnImpactOnPerformance()
{
    TEST_MOSH_PERFORMANCE(1000, "/LIBS/MOSH/bin/test-import.sps");
    // Mona on KVM Thinkpad x60: 190msec
    TEST_MOSH_PERFORMANCE(230, "/LIBS/MOSH/bin/test-import.sps");
}

int main(int argc, char *argv[])
{
    testEmptyScriptShouldRunQuickly();
    testManyImportDoesNotHaveAnImpactOnPerformance();
    TEST_RESULTS();
    return 0;
}
