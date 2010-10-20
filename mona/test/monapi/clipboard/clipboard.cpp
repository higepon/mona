#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static monapi_cmemoryinfo* alloc_buffer_size(const char* message, int size)
{
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, size, 0, 0);
    memcpy(buffer->Data, message, buffer->Size);
    return buffer;
}

static monapi_cmemoryinfo* alloc_buffer(const char* message)
{
    return alloc_buffer_size(message, strlen(message) + 1);
}

static void testPushPop()
{
    const char* greeting = "Hello, World";
    monapi_cmemoryinfo* data = alloc_buffer(greeting);
    EXPECT_EQ(M_OK, monapi_clipboard_push(data));
    monapi_cmemoryinfo* cmi = monapi_clipboard_pop();
    ASSERT_TRUE(cmi != NULL);
    EXPECT_EQ(data->Size, cmi->Size);
    EXPECT_STR_EQ(greeting, cmi->Data);

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);
    monapi_cmemoryinfo_dispose(data);
    monapi_cmemoryinfo_delete(data);
}

static void testEmptyPop()
{
    monapi_cmemoryinfo* cmi = monapi_clipboard_pop();
    ASSERT_TRUE(cmi == NULL);
}

int main(int argc, char *argv[])
{
    testPushPop();
    testEmptyPop();
    TEST_RESULTS(clipboard);
    return 0;
}
