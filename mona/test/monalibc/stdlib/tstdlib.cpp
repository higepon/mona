#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fenv.h>

int test_strncasecmp()
{
  int r;

  r = strncasecmp("+INF", "+inf", 4);
  EXPECT_TRUE(r == 0);

  return 0;
}

int test_float()
{
  EXPECT_TRUE(isnan(NAN));

  return 0;
}

int test_strtod();

int main(int argc, char* argv[])
{
    if (argc > 1) {
        printf(argv[1]);
        return -2;
    } else {
        test_strncasecmp();
       test_float();
        test_strtod();

        TEST_RESULTS(stdlib);
        return 0;
    }
}

int test_strtod()
{
  char *p;
  double r;

  r = strtod("INFINITY:", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ(":", p);

  r = strtod("INFINITY:", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("+INFINITY: with sign", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ(": with sign", p);

  r = strtod("+INFINITY: with sign", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("Infinity: Capital", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ(": Capital", p);

  r = strtod("Infinity: Capital", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("INF:", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ(":", p);

  r = strtod("INF:", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("inf: lower", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ(": lower", p);

  r = strtod("inf: lower", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("+INF: AAA", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ(": AAA", p);

  r = strtod("+INF: AAA", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("-INFINITY", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ("", p);

  r = strtod("-INFINITY", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("   -Inf in hoge", &p);
  EXPECT_TRUE(isinf(r));
  EXPECT_STR_EQ(" in hoge", p);

  r = strtod("   -Inf in hoge", NULL);
  EXPECT_TRUE(isinf(r));

  r = strtod("NAN", &p);
  EXPECT_TRUE(isnan(r));
  EXPECT_STR_EQ("", p);

  r = strtod("NAN", NULL);
  EXPECT_TRUE(isnan(r));

  r = strtod("nan lower", &p);
  EXPECT_TRUE(isnan(r));
  EXPECT_STR_EQ(" lower", p);

  r = strtod("0.124 is the number.", &p);
  EXPECT_TRUE(r == 0.124);
  EXPECT_STR_EQ(" is the number.", p);

  r = strtod("+0.124 is positive", &p);
  EXPECT_TRUE(r == 0.124);
  EXPECT_STR_EQ(" is positive", p);

  r = strtod("-10.092 is negative", &p);
  EXPECT_TRUE(r == -10.092);
  EXPECT_STR_EQ(" is negative", p);

  r = strtod("11.23556e+5", &p);
  EXPECT_TRUE(r == 11.23556e+5);
  EXPECT_STR_EQ("", p);

  r = strtod("11.23556E+5", &p);
  EXPECT_TRUE(r == 11.23556E+5);
  EXPECT_STR_EQ("", p);

  r = strtod("121", &p);
  EXPECT_TRUE(r == 121.0);
  EXPECT_STR_EQ("", p);

  r = strtod("0x1.154p+02 HEX", &p);
  EXPECT_TRUE(r == 0x1.154p+02);
  EXPECT_STR_EQ(" HEX", p);

  r = strtod("0xE HEX", &p);
  EXPECT_TRUE(r == 0xE.000p+00);
  EXPECT_STR_EQ(" HEX", p);

  r = strtod("0x1.F54P+02", &p);
  EXPECT_TRUE(r == 0x1.F54P+02);
  EXPECT_STR_EQ("", p);

  r = strtod("0x1.ab46p-12", &p);
  EXPECT_TRUE(r == 0x1.ab46p-12);
  EXPECT_STR_EQ("", p);

  r = strtod("0x1.ab46", &p);
  EXPECT_TRUE(r == 0x1.ab46p+00);
  EXPECT_STR_EQ("", p);

  r = strtod("0.333333333333333333333333333333333333333", &p);
  EXPECT_STR_EQ("", p);
  EXPECT_TRUE(r == 0.3333333333333333);

  r = strtod("1.7976931348623160e+308", &p);
  EXPECT_TRUE(errno == ERANGE);
  EXPECT_TRUE(isinf(r));
  errno = 0;

  r = strtod("1.7976931348623160e+308", NULL);
  EXPECT_TRUE(errno == ERANGE);
  EXPECT_TRUE(isinf(r));
  errno = 0;

  r = strtod("0x1.0p+1025", NULL);
  EXPECT_TRUE(errno == ERANGE);
  EXPECT_TRUE(isinf(r));
  errno = 0;

  r = strtod("2.2250738585072014e-324", NULL);
  EXPECT_TRUE(r == 0.0);

  r = strtod("0x1.0p-3000", NULL);
  EXPECT_TRUE(r == 0.0);

  r = strtod("0x1.0p-1025", NULL);
  EXPECT_TRUE(r == 0x1.0p-1025);

  r = strtod("nonsense", &p);
  EXPECT_TRUE(r == 0.0);
  EXPECT_STR_EQ("nonsense", p);

  r = strtod("nonsense", NULL);
  EXPECT_TRUE(r == 0.0);

  r = strtod(NULL, &p);
  EXPECT_TRUE(r == 0.0);

  r = strtod(NULL, NULL);
  EXPECT_TRUE(r == 0.0);

  return 0;
}
