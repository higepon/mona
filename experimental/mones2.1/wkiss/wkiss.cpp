//$Id$
#include <NetClient.h>
#include <monalibc/stdio.h>
#include <monapi/Date.h>
using namespace MonAPI;
using namespace mones;

void TcpEchoService()
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
}

void TcpDaytimeService()
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
}

void TcpCharGen()
{
    static char* buf = "01234567890abcdefghijklmnopqrstuvwxyz";
    NetClient client;
    int netdsc = client.TCPPasvOpen(CHARGEN);
    int dsc=client.TCPAccept(netdsc);
    for(;;){
        client.Write(dsc,(byte*)buf,strlen(buf));
    }
    client.Close(dsc);
}

int MonaMain(List<char*>* pekoe)
{
    dword id1=syscall_mthread_create((dword)TcpEchoService);    
    syscall_mthread_join(id1);
    dword id2=syscall_mthread_create((dword)TcpDaytimeService);
    syscall_mthread_join(id2);
    TcpCharGen();
    return 0;
}
