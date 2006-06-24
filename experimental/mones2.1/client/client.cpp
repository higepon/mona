//$Id$
#include <NetClient.h>
#include <monalibc/stdio.h>
using namespace MonAPI;
using namespace mones;
int Stat(NetClient& client)
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
    return 0;
}
int Ping(NetClient& client,dword remoteip)
{
    ///////////////////////////////////////////////
    printf("   Send ICMP echo request\n");
    int netdsc= client.ICMPOpen(remoteip);
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
        if(!strncmp((char*)buf,"How are you?",12)){
            printf("destination is Alive.\n");
        }
    }
    if( client.Close(netdsc) ){
        printf("CloseError.\n");
    }   
    return 0;
}

int Udp(NetClient& client,dword remoteip,word port)
{
    printf("   Send UDP to %d\n",port);    
    word localport = client.GetFreePort();
    printf("Port=%d\n",localport);
    int netdsc = client.UDPOpen(remoteip,localport,port);
    if( netdsc < 0 ){
        printf("OpenError.\n");
    }
    printf("Open::netdsc=%d\n",netdsc);
    
    if( client.Write(netdsc,(byte*)"What time is it now?",20) ){
        printf("WrieError.\n");
    }    
    byte buf[1024];//BAD design.
    int size= client.Read(netdsc,buf);
    if( size > 0 ){
        printf("(UDP)%s\n",buf);
    }    
    if( client.Close(netdsc) ){
        printf("CloseError.\n");
    }    
    return 0;
}

int TcpClient(NetClient& client,dword remoteip,word port)
{
    printf("   Send TCP tp %d\n",port);    
    word localport = client.GetFreePort();
    printf("Port=%d\n",localport);
    //isPasv=false;
    int netdsc = client.TCPActvOpen(remoteip,localport,port);
    if( netdsc < 0 ){
        printf("OpenError.\n");
    }
    printf("Open::netdsc=%d\n",netdsc);
    if( port == 7 ){
        if( client.Write(netdsc,(byte*)"Hello, How are you?",19) ){
            printf("WrieError.\n");
        }
    }
    byte buf[1024];//BAD design.
    int size= client.Read(netdsc,buf);
    if( size > 0 ){
        printf("(TCP)%s\n",buf);
    }
    if( client.Close(netdsc) ){
        printf("CloseError.\n");
    }    
    return 0;
}
int Ftp(NetClient& client,dword remoteip)
{
    return 0;
}

int MonaMain(List<char*>* pekoe)
{
    NetClient client;
    if( pekoe->size() < 2 || pekoe->size() > 4 ){
        printf("\nusage:\n");
        printf("         client dest-ip ping\n");
        printf("         client dest-ip udp svc\n");
        printf("         clinet dset-ip tcp svc\n");
        printf("         client dest-ip ftp");
        Stat(client);
        exit(0);
    }
    dword a,b,c,d;
    sscanf(pekoe->get(0),"%d.%d.%d.%d",&a,&b,&c,&d);
    dword remoteip=((d<<24)&0xFF000000)|((c<<16)&0x00FF0000)|((b<<8)&0x0000FF00)|(a&0x000000FF);
    word port;
    if( !strcmp(pekoe->get(1),"ping")){
        Ping(client,remoteip);
    }else if( !strcmp(pekoe->get(1), "udp")){
        sscanf(pekoe->get(2),"%d",&port);
        Udp(client,remoteip,port);
    }else if( !strcmp(pekoe->get(1), "tcp")){
        sscanf(pekoe->get(2),"%d",&port);
        TcpClient(client,remoteip,port);
    }else if( !strcmp(pekoe->get(1), "ftp")){
        Ftp(client,remoteip);
    }
    exit(0);
    return 0;
}
