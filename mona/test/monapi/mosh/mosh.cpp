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
    intptr_t result = monapi_call_process_execute_file_get_tid(
        command.c_str(),
        MONAPI_TRUE,
        &tid,
        MonAPI::System::getProcessStdinID(),
        MonAPI::System::getProcessStdoutID());
    if (result != 0) {
        EXPECT_TRUE(false);
        return false;
    }
    uint64_t s11 = MonAPI::Date::nowInMsec();
    logprintf("s11-s1=%d\n", (int)(s11 - s1));
    monapi_process_wait_terminated(tid);
    uint64_t s2 = MonAPI::Date::nowInMsec();
    return (int)(s2 - s1);
}

static void testMoshPerformance(int targetTime, const std::string& script)
{
    int msec = executeMoshWithTime(script);
    if (msec > targetTime) {
        logprintf("%s %dmsec\n", __func__, msec);
    }
    EXPECT_TRUE(msec <= targetTime);
}

static void testEmptyScriptShouldRunQuickly()
{
    testMoshPerformance(60, "/LIBS/MOSH/bin/empty.sps");
}

static void testManyImportDoesNotHaveAnImpactOnPerformance()
{
    testMoshPerformance(120, "/LIBS/MOSH/bin/test-import.sps");
}

int main(int argc, char *argv[])
{
    testEmptyScriptShouldRunQuickly();
    testManyImportDoesNotHaveAnImpactOnPerformance();
    testEmptyScriptShouldRunQuickly();
    testManyImportDoesNotHaveAnImpactOnPerformance();
    TEST_RESULTS();
    return 0;
}
