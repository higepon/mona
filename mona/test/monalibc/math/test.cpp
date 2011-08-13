#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>


int test_log();

int main(int argc, char* argv[])
{
    if (argc > 1) {
        printf(argv[1]);
        return -2;
    } else {
      //        test_log();

        TEST_RESULTS();
        return 0;
    }
}

// int test_log()
// {
//     double r = 0.0;

//     r = log(0.0);
//     EXPECT_TRUE(isinf(r));

//     r = log(-INFINITY);
//     EXPECT_TRUE(r == 0.0);

//     r = log(-1.0);
//     EXPECT_TRUE(r == 0.0);

//     r = log(INFINITY);
//     EXPECT_TRUE(isinf(r));

//     return 0;
// }
