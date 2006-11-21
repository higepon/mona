#include "TCPIO.h"
#include <monapi.h>
using namespace mones;
using namespace MonAPI;

void TCPIn::ThreadMain()
{
    for(;;){
        printf("In\n");
        sleep(1000);
    }
}

void TCPOut::ThreadMain()
{
    for(;;){
        printf("Out\n");
        sleep(1300);
    }
}
