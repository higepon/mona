#include <monapi.h>
#include "IDEDriver.h"

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
    syscall_get_io();

    IDEDriver ide;

    ide.printDebug();

    return 0;
}
