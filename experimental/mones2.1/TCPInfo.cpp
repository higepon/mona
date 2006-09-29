//$Id$
#include "TCPInfo.h"
#include "Dispatch.h"

using namespace mones;
using namespace MonAPI;

void TCPCoInfo::SetBlockingMode(MessageInfo* msg)
{
    blockingmode=msg->arg2;
    //printf("BM %d\n",blockingmode);
    return;
}

void TCPCoInfo::Reset(dword rip, word lport, word rport)
{
    remoteip=rip;
    localport=lport;
    remoteport=rport;
    seqnum=1400; //needs some condition.
    acknum=1;
    flags=RST|FIN|PSH;    
    window=1399;
    dispatcher->Send(NULL,0,this);
    printf("SENT RESET\n");
}

TCPCoInfo::TCPCoInfo(Dispatch* p):blockingmode(W_BLOCK|R_BLOCK),nomoredata(false),
    seqnum(0),acknum(0),status(CLOSED),flags(NORM),window(1000),need_retry(false)
{
    dispatcher=p;
}

void TCPCoInfo::Dump()
{
    L4Base::Dump();
    printf("STATUS:%d ACK%d SEC:%d FLAG:%d WINDOW:%d\n",status,seqnum,acknum,flags,window);
}

int TCPCoInfo::Strip(Ether* frame, byte** data)
{
    if( data != NULL ){
        *data=frame->IPHeader->TCPHeader->data;
    }
    return bswap(frame->IPHeader->len)-sizeof(IP)-sizeof(TCP);
}

bool TCPCoInfo::IsProcessed(Ether* frame)
{
    if( status == TIME_WAIT ){
        return false;
    }else if( TransStateByPKT(frame) ){
        return true;
    }else if( msg.header == MSG_NET_READ ){
        if(IsExpected(frame)==true)
            return Read_bottom_half(frame);
        return false;
    }else if( msg.header == MSG_NET_WRITE){
        Write_bottom_half(frame);
        return true;
    }
    return false;
}

bool TCPCoInfo::IsMyPacket(Ether* frame) 
{    
    if( status == TIME_WAIT ){
        return false;
    }else if( TYPETCP == frame->IPHeader->prot){ 
        if( remoteip   == frame->IPHeader->srcip &&
            localport  == bswap(frame->IPHeader->TCPHeader->dstport) &&
            remoteport == bswap(frame->IPHeader->TCPHeader->srcport) )
        {     
            return true;
        }else if( status == LISTEN &&
            frame->IPHeader->TCPHeader->flags == SYN &&
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

bool TCPCoInfo::TimeoutCheck(dword now)
{
    if( msg.header == MSG_NET_WRITE && write_timeout > now ){
        Message::reply(&msg);
    }
    if(  msg.header == MSG_NET_READ && read_timeout > now ){
        Message::reply(&msg);
    }
    return L4Base::TimeoutCheck(now);
}

bool TCPCoInfo::IsExpected(Ether* frame)
{    
    //seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
    //acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
    return true;
}

bool TCPCoInfo::Read_bottom_half(Ether* frame)
{
    byte* data;
    if( msg.header == MSG_NET_READ ){
        monapi_cmemoryinfo* mi = monapi_cmemoryinfo_new();  
        if (mi != NULL){
            if( frame != NULL ){
                if( status == LAST_ACK ){
                    printf("LASTACKED\n");
                    nomoredata=true;
                }
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
        return true;
    }
    return false;
}
////
void TCPCoInfo::Read(MessageInfo* m)
{    
    ///SOMETHING WRONG!!!! HERE.
    if(  nomoredata == true ){
        printf("NOREAD\n");
        Message::reply(&msg,NULL,0);
        memset(&msg,'\0',sizeof(MessageInfo));
        return;
    }
    memcpy(&msg,(byte*)m,sizeof(MessageInfo));         
    read_timeout=syscall_get_tick()+500;
}

void TCPCoInfo::Write(MessageInfo* m)
{ 
    need_retry=true;
    monapi_cmemoryinfo* ret = monapi_cmemoryinfo_new();
    if( ret != NULL){
        ret->Handle = m->arg2;
        ret->Owner  = m->from;
        ret->Size   = m->arg3;
        monapi_cmemoryinfo_map(ret);
        if( netdsc == m->arg1 ){
            dispatcher->Send(ret->Data,ret->Size,this);
            memcpy(&msg,(byte*)m,sizeof(MessageInfo)); //Register msg.
            //Register to timer.
            dispatcher->DoDispatch();
        }
        monapi_cmemoryinfo_delete(ret); /// some other one delete this resouce.
    }    
    write_timeout=syscall_get_tick()+500;
    //if(( blockingmode&W_BLOCK )==0){
    //    Message::reply(m);
    //} 
}

void TCPCoInfo::Write_bottom_half(Ether* frame)
{
    if( frame != NULL ){
        if( frame->IPHeader->TCPHeader->flags == ACK ){ //TODO Delay ack.
            need_retry=false;
            Message::reply(&msg);        
            seqnum+=msg.arg3; //ADD data size.
            memset(&msg,'\0',sizeof(MessageInfo));   
        }
    }
}    

bool TCPCoInfo::Write_retry() //Anyone call me!
{
    return true;
}

int TCPCoInfo::Duplicate()
{
    printf("DUP\n");
    TCPCoInfo* pT= new TCPCoInfo(dispatcher);
    int n = dispatcher->getSerialNo();  
    memcpy(&(pT->msg),(byte*)(&msg),sizeof(MessageInfo)); //Setup New Info.
    pT->Init(0, localport,0, msg.from, netdsc);
    this->netdsc=n;  
    pT->msg.header=MSG_NET_PASVOPEN;
    pT->PasvOpen();
    dispatcher->AddInfo(pT);   
    Message::reply(&msg,n);
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
    window=1400;
    dispatcher->Send(NULL,0,this);
}

void TCPCoInfo::SendACK(Ether* frame)
{
    if( status == ESTAB ){
        byte* tmp;
        int size=Strip(frame,&tmp);
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+size;
        flags=ACK;    
        window=1401;
        dispatcher->Send(NULL,0,this);
    }
}
//////////////////////////////////////////
bool TCPCoInfo::ActvOpen()
{
    if( status == CLOSED){
        status=SYN_SENT;
        flags=SYN;    
        window=1402;
        dispatcher->Send(NULL,0,this);
        return true;
    }
    return false;
}

bool TCPCoInfo::PasvOpen()
{
    if( status == CLOSED ){
        status = LISTEN;
        flags=NORM;
        window=1403;
        return true;
    }
    return false;
}

void TCPCoInfo::Close()
{     
    if( status == ESTAB ){
        status=FIN_WAIT1;
        flags=FIN|ACK;
        window=1404; 
        dispatcher->Send(NULL,0,this);
    }
}
/*CLOSED=1,LISTEN 2,SYN_SENT 3,SYN_RCVD 4,ESTAB 5
FIN_WAIT1 6,FIN_WAIT2 7,CLOSE_WAIT 8,LAST_ACK 9,TIME_WAIT 10,CLOSING 11*/
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
    //printf("[%x %d %d %d]\n",rflag,status,serialno,bswap(frame->IPHeader->id));
    byte rflag = (frame->IPHeader->TCPHeader->flags) & ~PSH;
    ////ACTIVE 
    if( (status == SYN_SENT) && (rflag == SYN|ACK) ){
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=ESTAB; 
        flags=ACK;
        window=1405;
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
        window=1406;
        dispatcher->Send(NULL,0,this);
        dispatcher->RemoveInfo(this,500); 
        Message::reply(&msg, netdsc);//close
        return true;
    }
    ////SIMALCLOSE///////////////
    if( (status == FIN_WAIT1 ) && (rflag == FIN|ACK )){
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=CLOSING; 
        flags=ACK;
        window=1407;
        dispatcher->Send(NULL,0,this);     
    }   
    if( (status == CLOSING) && (rflag == ACK) ){
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=TIME_WAIT; 
        flags=ACK;
        window=1408;
        dispatcher->Send(NULL,0,this);
        dispatcher->RemoveInfo(this,500); 
        if( msg.header == MSG_NET_CLOSE ){
            Message::reply(&msg, netdsc);//close
        }
        return true;
    }
    /////PASV////////////////////
    if( (status == LISTEN ) && (rflag == SYN ) ){
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=SYN_RCVD;
        flags=SYN|ACK;
        window=1409;
        dispatcher->Send(NULL,0,this);
        return true;
    }   
    if( (Strip(frame,NULL) == 0) && (status == SYN_RCVD) &&
        (rflag == ACK ) && ( msg.header==MSG_NET_ACCEPT ) ){
        Duplicate();  ////////////REPLY FOR ACCEPT.        
        status = ESTAB;
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber);
        flags=PSH|ACK;
        window=1410;
        nomoredata=false;
        return true; 
    }
    if( (status == ESTAB ) && ( rflag  == (FIN|ACK) )  ){
        seqnum=bswapl(frame->IPHeader->TCPHeader->acknumber);
        acknum=bswapl(frame->IPHeader->TCPHeader->seqnumber)+1;
        status=CLOSE_WAIT;
        flags=ACK;
        window=1411;
        dispatcher->Send(NULL,0,this);
        if( msg.header == MSG_NET_CLOSE ){
            Message::reply(&msg, netdsc);//close
        }
        status=LAST_ACK;
        flags=FIN|ACK;
        window=1412;
        dispatcher->Send(NULL,0,this);
        return true;
    }
    if( (status == LAST_ACK ) && ( rflag == ACK ) ){
        status=CLOSED;
        dispatcher->RemoveInfo(this,0);
        return true;
    }
    if( (rflag & RST) == RST){
        status = CLOSED;
        dispatcher->RemoveInfo(this,0);
        return true;
    }
    return false; 
}
