#include <monapi.h>

#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

using namespace MonAPI;

static SharedMemory* alloc_buffer_size(const char* message, int size)
{
    SharedMemory* buffer = new SharedMemory(size);
    buffer->map();
    memcpy(buffer->data(), message, buffer->size());
    return buffer;
}

static SharedMemory* alloc_buffer(const char* message)
{
    return alloc_buffer_size(message, strlen(message) + 1);
}

static void testSetGet()
{
    const char* greeting = "Hello, World";
    scoped_ptr<SharedMemory> data(alloc_buffer(greeting));
    const SharedMemory& shm = *data.get();
    EXPECT_EQ(M_OK, monapi_clipboard_set(shm));

    scoped_ptr<SharedMemory> cmi(monapi_clipboard_get());
    ASSERT_TRUE(cmi != NULL);
    EXPECT_EQ(strlen(greeting) + 1, cmi->size());
    EXPECT_STR_EQ(greeting, (const char*)cmi->data());

    EXPECT_EQ(M_OK, monapi_clipboard_clear());
}

static void testEmptyGet()
{
    SharedMemory* cmi = monapi_clipboard_get();
    ASSERT_TRUE(cmi == NULL);
}

int main(int argc, char *argv[])
{
    testSetGet();
    testEmptyGet();
    TEST_RESULTS(clipboard);
    return 0;
}
