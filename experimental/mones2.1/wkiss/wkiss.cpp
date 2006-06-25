//$Id$
#include <NetClient.h>
#include <monalibc/stdio.h>
#include <monapi/Date.h>
using namespace MonAPI;
using namespace mones;

//
int MonaMain(List<char*>* pekoe)
{
    char buf[1024];
    NetClient client;
    int netdsc = client.TCPPasvOpen(DAYTIME);
    for(;;){
        int dsc = client.TCPAccept(netdsc);
        //printf("%d\n",dsc);
        Date d;
        sprintf(buf,"Why don't you buy high quality Rolex replica?\n\r%d/%d %d:%d:%d %d\n\r",
                d.month(),d.day(),d.hour(),d.min(),d.sec(),d.year());
        client.Write(dsc,(byte*)buf,strlen(buf));
        client.Close(dsc);
    }
    return 0;
}
