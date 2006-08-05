//$Id$
#include <NetClient.h>
#include <monalibc/stdio.h>
#include <monapi/Date.h>
using namespace MonAPI;
using namespace mones;

int MonaMain(List<char*>* pekoe)
{
    char buf[1500];
    NetClient client;
    int netdsc = client.TCPPasvOpen(ECHO);
    for(;;){
        int dsc = client.TCPAccept(netdsc);
        for(;;){
            memset(buf,'\0',1500);
            client.Read(dsc,(byte*)buf);
            if( buf[0]!=NULL){
                printf(">%s",buf);
                if( !strncmp(buf,"quit",1)){
                    printf("q\n");
                    break;
                }
                client.Write(dsc,(byte*)buf,strlen(buf));
            }
        }
        client.Close(dsc);
    }
    return 0;
}
/*
int MonaMain(List<char*>* pekoe)
{
    char buf[1024];
    NetClient client;
    int netdsc = client.TCPPasvOpen(DAYTIME);
    for(;;){
        int dsc = client.TCPAccept(netdsc);
        //printf("%d\n",dsc);
        Date d;
        sprintf(buf,"Why don't you buy high quality Rolex replica?\r\n%d/%d %d:%d:%d %d\r\n",
                d.month(),d.day(),d.hour(),d.min(),d.sec(),d.year());
        client.Write(dsc,(byte*)buf,strlen(buf));
        client.Close(dsc);
    }
    return 0;
}
*/
