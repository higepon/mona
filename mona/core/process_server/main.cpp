#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monapi/Array.h>
#include <monalibc.h>
#include "ProcessServer.h"
#include "ProcessManager.h"
#include "ProcessServer.h"
#include "ProcessManager.h"

#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/Assert.h>
#include <vector>
#include "ProcessInfo.h"
using namespace std;
using namespace MonAPI;


#include "ProcessServer.h"

int main(int argc, char* argv[])
{
    ProcessServer server;
    server.service();
    return 0;
}
