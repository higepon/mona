#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <monapi/net.h>

using namespace MonAPI;

int main(int argc, char *argv[])
{
    uint8_t* p = (uint8_t*)0;
    *p = 1; // just kill myself
    return 0;
}
