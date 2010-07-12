#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdio.h>

extern "C" {
#include <gmp.h>
}

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

static void test_mpq_div()
{
    mpz_t a, b;
    ASSERT_EQ(0, mpz_init_set_str(a, "13900000000000000000000", 10));
    ASSERT_EQ(0, mpz_init_set_str(b, "3500000000000000000000", 10));

    mpq_t ret, x, y;
    mpq_init(ret);
    mpq_init(x);
    mpq_init(y);
    mpq_set_z(x, a);
    mpq_set_z(y, b);
    mpq_div(ret, x, y);
    EXPECT_STR_EQ("139/35", mpq_get_str(NULL, 10, ret));
    mpz_clear(a);
    mpz_clear(b);
    mpq_clear(ret);
}


int main(int argc, char *argv[])
{
    test_mpz();
    test_mpq();
    test_mpq_div();
    TEST_RESULTS(gmp);
    return 0;
}
