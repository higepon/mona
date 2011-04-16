#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monapi/Array.h>
#include <monalibc.h>
#include <monapi/Assert.h>
#include "ProcessServer.h"

int main(int argc, char* argv[])
{
    ProcessServer server;
    server.service();
    return 0;
}
