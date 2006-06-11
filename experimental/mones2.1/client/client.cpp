//$Id$
#include "NetClient.h"
#include <monalibc/stdio.h>
using namespace MonAPI;
using namespace mones;

int Example(NetClient& client)
{
    //read nic status.
    NetStatus stat;
    if( client.Stat(&stat) ){
        printf("\n%s:\nIPaddress:",stat.devname);
        for(int j=0;j<4;j++)
            printf("%d.",*(((byte*)&stat.localip)+j));
        printf("NetMask:");
        for(int j=0;j<4;j++)
            printf("%d.",*(((byte*)&stat.netmask)+j));
        printf("\nDefaultRoute:");
        for(int j=0;j<4;j++)
            printf("%d.",*(((byte*)&stat.defaultroute)+j));
        printf("MTU:%d MAC:",stat.mtu);
        char buf[16];
        for(int j=0;j<6;j++){
            sprintf(buf,"%02x",stat.mac[j]);
            printf("%s:",buf);
        }
        printf("\n");
    }
    dword remoteip=(3<<24)|(10<<16)|(168<<8)|(192);
    if( stat.localip==LOOPBACKIP ){
        remoteip=LOOPBACKIP;
    }
    ///////////////////////////////////////////////
    printf("\nSend ICMP echo request\n");
    int netdsc= client.Open(remoteip,0,0,TYPEICMP);
    if( netdsc< 0 ){
        printf("OpenError.\n");
    }
    printf("Open::netdsc=%d\n",netdsc);
    if( client.Write(netdsc,(byte*)"How are you?",12) ){
        print("WriteError\n");
    }
    byte buf[1024];//BAD design.
    int size= client.Read(netdsc,buf);
    if( size > 0 ){
        //printf("Read: %s\n",buf);
        if(!strcmp((char*)buf,"How are you?")){
            printf("destination is Alive.\n");
        }
        //if recived string is same as sended.
        //printf("dset is alive,\n");
    }
    if( client.Close(netdsc) ){
        printf("CloseError.\n");
    }   
    ////////////////////////////////////////////
    printf("\nSend UDP to DAYTIME\n");    
    word localport = client.GetFreePort();
    printf("Port=%d\n",localport);
    netdsc = client.Open(remoteip,localport,DAYTIME,TYPEUDP);
    if( netdsc < 0 ){
        printf("OpenError.\n");
    }
    printf("Open::netdsc=%d\n",netdsc);
    
    if( client.Write(netdsc,(byte*)"What time is it now?",20) ){
        printf("WrieError.\n");
    }
    size= client.Read(netdsc,buf);
    if( size > 0 ){
        printf("remote time is %s\n",buf);
    }    
    if( client.Close(netdsc) ){
        printf("CloseError.\n");
    }    
    //////////////////////////////////////////////////
    printf("\nSend TCP to DAYTIME\n");    
    localport = client.GetFreePort();
    printf("Port=%d\n",localport);
    netdsc = client.Open(remoteip,localport,DAYTIME,TYPETCP);
    if( netdsc < 0 ){
        printf("OpenError.\n");
    }
    printf("Open::netdsc=%d\n",netdsc);
    
    if( client.Write(netdsc,(byte*)"What time is it now?",20) ){
        printf("WrieError.\n");
    }
    size= client.Read(netdsc,buf);
    if( size > 0 ){
        printf("remote time is %s\n",buf);
    }    
    if( client.Close(netdsc) ){
        printf("CloseError.\n");
    }    
    //////////////////////////////////////////////////
    //re-setup nic. 
    char devname[]="pcnet0";
    if( client.Config(devname,(5<<24)|(0<<16)|(168<<8)|192,(1<24)|(0<<16)|(168<<8)|192,24,60,1500) ){
        printf("ConfigError\n");
    }
    printf("tests are completed.\n");
    return 0;
}

int MonaMain(List<char*>* pekoe)
{
    /*
    if( pekoe->size() != 2 ){
        printf("\nusage: client ping dest-ip\n");
        printf("       client udpdaytime dest-ip\n");
        printf("       clinet tcpdaytime dset-ip\n");
        printf("       client ftp dest-ip\n");
        exit(0);
    }    
    //dword dstip = scanf(pekoe->get(1));
    NetClient client;
    if( !strcmp(pekoe->get(0),"ping")){
        printf("ping\n");
    }else if( !strcmp(pekoe->get(0), "udpdaytime")){
        printf("udp\n");
    }else if( !strcmp(pekoe->get(0), "tcpdaytime")){
        printf("tcp\n");
    }else if( !strcmp(pekoe->get(0), "ftp")){
        printf("ftp\n");
    }
    */
    NetClient client;
    Example(client);
    exit(0);
    return 0;
}
