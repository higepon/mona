#define TEST_ASSERT(str, condition) {if (!(condition)) {_printf("%s failed \"%s\" \"%s\" %s:%d\n", __func__, str, #condition, __FILE__, __LINE__);return -1;}}

#define TEST_ERROR(str) {_printf("%s error \"%s\" %s:%d\n", __func__, str, __FILE__, __LINE__);return -1;}

#define TEST_ASSERT_EQUAL(value, expected) {if (value != expected) {_printf("%s failed expected = %d, but got = %d %s:%d\n", __func__, expected, value , __FILE__, __LINE__);return -1;}}

#define TEST_SUCCESS() _printf("%s OK\n", __func__);return 0;

