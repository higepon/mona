#include "TCPIO.h"
#include <monapi.h>
using namespace mones;
using namespace MonAPI;

void TCPIn::ThreadMain()
{
    for(;;){
        printf("In%d\n",id);
        sleep(1000);
    }
}

void TCPOut::ThreadMain()
{
    for(;;){
        printf("Out%d\n",id);
        sleep(1300);
    }
}
