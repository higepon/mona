//$Id: Connection.cpp 3287 2006-06-18 07:17:51Z eds1275 $
#include "TCPInfo.h"
#include "Dispatch.h"

using namespace mones;
using namespace MonAPI;

TCPCoInfo::TCPCoInfo(Dispatch* p):isPasv(true),seqnum(0),acknum(0),status(CLOSED),flags(NORM),window(1000)
{
    dispatcher=p;
    serialno=dispatcher->serialno;
    dispatcher->serialno++;
}

int TCPCoInfo::Strip(Ether* frame, byte** data)
{
    *data=frame->IPHeader->TCPHeader->data;
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(TCP);
}

bool TCPCoInfo::IsProcessed(Ether* frame)
{
    if( TransStateByPKT(frame) ){
        return true;
    }else if( msg.header == MSG_NET_READ ){
        Read_bottom_half(frame);
        return true;
    }else if( msg.header == MSG_NET_WRITE){ //FOR TCP
        Write_bottom_half(frame);
        return true;
    }
    return false;
}

bool TCPCoInfo::IsMyPacket(Ether* frame) 
{
    if( TYPETCP == frame->IPHeader->prot){ 
        if( remoteip   == frame->IPHeader->srcip &&
            localport  == bswap(frame->IPHeader->TCPHeader->dstport) &&
            remoteport == bswap(frame->IPHeader->TCPHeader->srcport) )
        {     
            return true;
        }else if( isPasv == true &&
            localport == bswap(frame->IPHeader->TCPHeader->dstport) )
        {
            remoteip =frame->IPHeader->srcip;
            remoteport=bswap(frame->IPHeader->TCPHeader->srcport);
            return true;
        }
    }
    return false;
}


void TCPCoInfo::CreateHeader(Ether* frame,byte* data, word size)
{
    IP* ip=frame->IPHeader; //for pseudo header
    ip->ttl =0x00;
    ip->prot=TYPETCP;
    ip->chksum=bswap(size+sizeof(TCP));///
    TCP* tcp=frame->IPHeader->TCPHeader;
    tcp->srcport=bswap(localport);
    tcp->dstport=bswap(remoteport);
    tcp->seqnumber=bswapl(seqnum);
    tcp->acknumber=bswapl(acknum);
    tcp->offset=0xF0 & ((sizeof(TCP))<<2);//options.
    tcp->flags=0x3F&flags;   
    tcp->window=bswap(window);
    tcp->chksum=0x0000;
    tcp->urgent=0;
    memcpy(tcp->data,data,size);
    tcp->chksum=bswap(checksum((byte*)&(ip->ttl),size+sizeof(TCP)+12));
    CreateIPHeader(frame,size+sizeof(TCP)+sizeof(IP),TYPETCP);
    //printf("CreateHeader%d %d\n",seqnum,acknum);
}

void TCPCoInfo::Accept(MessageInfo* m)
{      
    memcpy(&msg,(byte*)m,sizeof(MessageInfo)); //Register msg.
}

void TCPCoInfo::Read_bottom_half(Ether* frame)
{
    byte* data;
    monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
    if (mi != NULL){
        if( frame != NULL ){
            int size=Strip(frame, &data);
            //printf("size=%d\n",size);
            if( size >0 ){
                monapi_cmemoryinfo_create(mi,size, true);
                if( mi != NULL ){
                    memcpy(mi->Data,data,mi->Size);    
                    Message::reply(&msg, mi->Handle, mi->Size);
                    memset(&msg,'\0',sizeof(MessageInfo));
                    SendACK(frame);
                }
            }
        }
        monapi_cmemoryinfo_delete(mi);
    }else{
        Message::reply(&msg);
    }
}

void TCPCoInfo::Write(MessageInfo* m)
{ 
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if( ret != NULL){
        ret->Handle = m->arg2;
        ret->Owner  = m->from;
        ret->Size   = m->arg3;
        monapi_cmemoryinfo_map(ret);
        if( netdsc == m->arg1 ){
            dispatcher->Send(ret->Data,ret->Size,this);   //CHECK ACK.        
            memcpy(&msg,(byte*)m,sizeof(MessageInfo)); //Register msg.
            dispatcher->DoDispatch();
            //Message::reply(m);
        }
        monapi_cmemoryinfo_delete(ret);
    }
}

void TCPCoInfo::Write_bottom_half(Ether* frame)
{
    if( frame != NULL ){
        if( frame->IPHeader->TCPHeader->flags == ACK ){ //TODO Delay ack.
            Message::reply(&msg);        
            seqnum+=msg.arg3; //ADD data size.
            memset(&msg,'\0',sizeof(MessageInfo));   
            //printf("SEQNUM=%d\n",seqnum);
        }
    }
}    

int TCPCoInfo::Duplicate()
{
    TCPCoInfo* pT= new TCPCoInfo(dispatcher);
    int n = dispatcher->InfoNum();    
    memcpy(&(pT->msg),(byte*)(&msg),sizeof(MessageInfo)); //Setup New Info.
    pT->Init(0, localport,0, msg.from, netdsc);
    this->netdsc=n;
    pT->isPasv=true;    
    pT->msg.header=MSG_NET_PASVOPEN;
    pT->TransStateByMSG(MSG_NET_PASVOPEN);
    dispatcher->AddInfo(pT);
    return n;
}

void TCPCoInfo::ReplyUnReach(Ether* frame)
{
    remoteip=frame->IPHeader->srcip;
    localport=bswap(frame->IPHeader->TCPHeader->dstport);
    remoteport=bswap(frame->IPHeader->TCPHeader->srcport);// srcport!!!
    seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
    acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
    status=CLOSED;
    flags=PSH|RST;
    window=1408;
    dispatcher->Send(NULL,0,this);
}

void TCPCoInfo::Close()
{     
    if( status == ESTAB ){
        status=FIN_WAIT1;
        flags=FIN|ACK;
        window=1400; 
        //printf("ESTAB->FIN_WAIT1\n");
        dispatcher->Send(NULL,0,this);
    }
}

void TCPCoInfo::SendACK(Ether* frame)
{
     byte* tmp;
     int size=Strip(frame,&tmp);
     if( size == 0 ) size=1 ;
     seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
     acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+size;
     //printf("===>%d\n",acknum);
     status=ESTAB; 
     flags=ACK;    
     window=1405;
     //printf("SENDING READ ACK");
     dispatcher->Send(NULL,0,this);
}

/////////TRANSITION OF STATE FUNCTIONS/////////////////////
//=EVENT==========STAT.TRANS.============ACTION=================WHO
  //MSGACTVOpen    CLOSED->SYN_SENT       sendSYN               @THIS
//MSGOpen         CLOISED->LISTEN        -
//MSGWrite        LISTEN->ERROR.
//MSGClose        SYN_RCVD->FIN_WAIT1    sendFIN
//MSGClose        ESTAB->FIN_WAIT1       sendFIN
bool TCPCoInfo::TransStateByMSG(dword msg)
{
    if( status == CLOSED  && isPasv==false && msg==MSG_NET_ACTVOPEN){
        seqnum=bswapl(1);
        acknum=bswapl(1);
        status=SYN_SENT;
        flags=SYN;    
        window=1401;
        //printf("CLOSED->SYN_SENT\n");
        dispatcher->Send(NULL,0,this);
        return true;
    }
    if( status == CLOSED && isPasv==true && msg==MSG_NET_PASVOPEN ){
        status = LISTEN;
        flags=NORM;
        seqnum=0;
        window=1409;
        //printf("LISTENING\n");
        return true;
    }
    if( status == ESTAB ){
        flags=NORM;
        printf("XX");
    }
    return false;
}
/*        CLOSED=1,
        LISTEN 2
        SYN_SENT 3
        SYN_RCVD 4
        ESTAB 5
        FIN_WAIT1 6
        FIN_WAIT2 7
        CLOSE_WAIT 8
        LAST_ACK 9
        TIME_WAIT 10 */
//=EVENT==========STAT.TRANS.============ACTION=================WHO
  //recvSYN|ACK     SYN_SENT-> ESTAB       sendACK replyMSGOPEN @THIS
//recvSYN         SYN_SENT->SYN_RCVD     sendACK
//recvSYN         LISTEN->SYN_RCVD       sendSYN|ACK
//recvACK         SYN_RSVD->ESTAB        -
//recvFIN         ESTAB->CLOSE_WAIT      sendACK
//TRANS.CLOS_WAIT CLOSE_WAIT->LAST_ACK   sendFIN
//recvFIN|ACK     LAS_ACK->CLOSED
//recvFIN|ACK     FIN_WAIT1->FIN_WAIT2   -
//recvFIN         FIN_WAIT2->TIME_WAIT   sendACK
//recvACK         FIN_WAIT1->CLOSING     sendACK
//recvFIN|ACK     CLOSEING->TIME_WAIT    TRANS.CLOSED delay2MIN.
bool TCPCoInfo::TransStateByPKT(Ether* frame)
{
    byte* data;
    byte rflag= frame->IPHeader->TCPHeader->flags;
    ////ACTIVE OPEN CLOSE
    printf("[%x %d %d %d]\n",rflag,status,serialno,bswap(frame->IPHeader->id));
    if( (status == SYN_SENT) && (rflag == SYN|ACK) ){
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=ESTAB; 
        flags=ACK;
        window=1402;
        //printf("SYN_SENT->ESTAB\n");
        Message::reply(&msg, netdsc);
        dispatcher->Send(NULL,0,this);
        return true;
    }
    if( (status == FIN_WAIT1) && (rflag == ACK) ){
        status = FIN_WAIT2;
        return true;
    }
    if( (status == FIN_WAIT2) && (rflag == (FIN|ACK)) ){
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=TIME_WAIT; 
        flags=ACK;
        window=1403;
        //printf("FIN_WAIT1->TIME_WAIT %d %d\n",status,rflag);
        Message::reply(&msg, netdsc);//close
        dispatcher->Send(NULL,0,this);
        dispatcher->RemoveInfo(this,500);
        return true;
    }
    /////PASV////////////////////
    if( (status == LISTEN ) && (rflag == SYN ) ){
        remoteip=frame->IPHeader->srcip;
        remoteport=bswap(frame->IPHeader->TCPHeader->srcport);// srcport!!!
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=SYN_RCVD;
        flags=SYN|ACK;
        window=1404;
        //printf("LISTEN->SYN_RCVD %d %d\n",seqnum,acknum);
        dispatcher->Send(NULL,0,this);
        return true;
    }   
    if( (Strip(frame,&data) == 0) && (status == SYN_RCVD) && (rflag == ACK )){
        //printf("SYN_RCVD->ESTAB\n");
        int n=Duplicate(); ////////////REPLY FOR ACCEPT.
        Message::reply(&msg,n);        
        status = ESTAB;
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber);
        flags=PSH|ACK;
        window=1407;
        return true; 
    }
    if( (status == ESTAB ) && ( rflag == (FIN|ACK) ) ){
        //remoteip=frame->IPHeader->srcip;
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=CLOSE_WAIT; 
        flags=ACK;
        window=1406;
        //printf("FIN_WAIT1->TIME_WAIT %d %d\n",status,rflag);
        Message::reply(&msg, netdsc);//close
        dispatcher->Send(NULL,0,this);
        return true;
    }
    if( (rflag & RST) == RST){
        status = CLOSED;
        dispatcher->RemoveInfo(this,0);
        return true;
    }
    return false; 
}

