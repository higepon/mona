#define TEST_ASSERT(str, condition) {if (!(condition)) {printf("%s failed \"%s\" \"%s\" %s:%d\n", __func__, str, #condition, __FILE__, __LINE__);return -1;}}

#define TEST_ERROR(str) {printf("%s error \"%s\" %s:%d\n", __func__, str, __FILE__, __LINE__);return -1;}

#define TEST_ASSERT_EQUAL(value, expected) {if (value != expected) {printf("%s failed expected = %d, but got = %d %s:%d\n", __func__, expected, value , __FILE__, __LINE__);return -1;}}

//#define TEST_SUCCESS() printf("%s OK\n", __func__);return 0;
#define TEST_SUCCESS() return 0;

