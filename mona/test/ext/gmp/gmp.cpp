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

static void test_mpq()
{
    mpq_t a, b;
    mpq_init(a);
    mpq_init(b);
    mpq_set_ui(a, 1, 3);
    mpq_set_ui(b, 6, 1);

    EXPECT_STR_EQ("1/3", mpq_get_str(NULL, 10, a));
    EXPECT_STR_EQ("6", mpq_get_str(NULL, 10, b));
    mpq_mul(a, a, b);
    EXPECT_STR_EQ("2", mpq_get_str(NULL, 10, a));
    mpq_clear(a);
    mpq_clear(b);
}


int main(int argc, char *argv[])
{
    test_mpz();
    test_mpq();
    TEST_RESULTS(gmp);
    return 0;
}
