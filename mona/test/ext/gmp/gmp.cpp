#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdio.h>

#ifdef __cplusplus
  #undef __cplusplus
  #define MOSH_CPP
#endif
extern "C" {
#include <gmp.h>
}
#ifdef MOSH_CPP
  #define __cplusplus
#endif

static void test_mpz()
{
    mpz_t a, b;
    mpz_init(a);
    mpz_init(b);
    mpz_set_ui (a, 0xFFFFFFFF);
    mpz_set_ui (b, 1);
    mpz_add(a, a, b);
    EXPECT_STR_EQ("100000000", mpz_get_str(NULL, 16, a));
    mpz_clear(a);
    mpz_clear(b);
}

int main(int argc, char *argv[])
{
    test_mpz();
    TEST_RESULTS(gmp);
    return 0;
}
