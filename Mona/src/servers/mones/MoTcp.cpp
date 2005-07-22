/*!
    \file   MoTcp.cpp
    \brief  TCPプロトコル実装クラス

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/10/11 update:$Date$
*/

/*! \class MoTcp
 *  \brief IPプロトコル実装クラス
 */



#include "MoTcp.h"
#include "MonesConfig.h"
#include "MonesGlobal.h"


/*!
    \brief initialize
         MoTcp コンストラクタ
    \author Yamami
    \date   create:2004/10/11 update:$Date$
*/
MoTcp::MoTcp()
{
/*
    //Socketリスト初期化
    conSocHash = new HList<Socket*>();
    
    
    //ここで、Echoサーバをリスンする。
    //TODO 本来ならEchoサーバとして実装すべし。Socket層も同じく。
    //とりあえず動く物を
    
    Socket *echoSoc;
    
    echoSoc = new Socket();
    echoSoc->status=LISTEN;
    echoSoc->myport=7;

    //Socketリストに追加
    conSocHash->add(echoSoc);
*/

}

/*!
    \brief initialize
         MoTcp initTcp
    \author Yamami
    \param  AbstractMonic *pminsNic [in] NICクラスへのポインタ
    \date   create:2004/10/11 update:$Date$
*/
void MoTcp::initTcp(AbstractMonic *pminsNic ) 
{
    //NICクラスを保持
    insAbstractNic = pminsNic;
    return;
}


/*!
    \brief initialize
         MoTcp デスクトラクタ
    \author Yamami
    \date   create:2004/10/11 update:$Date$
*/
MoTcp::~MoTcp() 
{

}


/*!
    \brief send_tcp
         TCPデータ送出処理
    \param  T_TSOCK *sock [in] 通信相手を指すTCPソケット構造体
    \param  char flags [in] 制御フラグ
    \param  struct Packet *packet [in] ソケット
    \param  char *data [in] 送信データ
    \param  dword size [in] 送信データのサイズ
    \return 無し

    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
void send_tcp(Socket *sock, char flags, struct Packet *packet, char *data, dword size)
{

/*
    dword tsize;
    dword sum;
    char *ip_hdr;
    char *tcp_hdr;
    char *tcp_body;

    //パケットのポインタを、イーサネットヘッダ分とする。
    packet->ptr = packet->buffer + SIZEOF_ETHERHDR;
    //パケットのサイズは、TCPヘッダ+データサイズ
    packet->size = SIZEOF_TCPHDR + size;
    //IPヘッダへのポインタは、パケットのポインタ
    ip_hdr = packet->ptr;
    //TCPヘッダのポインタは、IPヘッダから、IPヘッダサイズ分プラス
    tcp_hdr = ip_hdr + SIZEOF_IPHDR;
    //TCPボディへのポインタは、TCPヘッダから、TCPヘッダサイズ分
    tcp_body = tcp_hdr + SIZEOF_TCPHDR;

    // TCPデータ部のセット
    memcpy(tcp_body, data, size);


    // TCPヘッダのセット
    packet_put_n(tcp_hdr, tcphdr_src_port, sock->myport);
    packet_put_n(tcp_hdr, tcphdr_dest_port, sock->youport);
    packet_put_nl(tcp_hdr, tcphdr_opt_mss, 0x020405b4); // kind = 2, len = 4, mss = 1460
    tcp_hdr[tcphdr_len] &= 0x0f;
    tcp_hdr[tcphdr_len] |= (5 + 1) << 4; // Unit is 32bit words

    tcp_hdr[tcphdr_flag] = flags & 0x3f;
    packet_put_n(tcp_hdr, tcphdr_chksum, 0);
    packet_put_n(tcp_hdr, tcphdr_urg, 0);

    packet_put_nl(tcp_hdr, tcphdr_ack, sock->ack);
    packet_put_nl(tcp_hdr, tcphdr_seq, sock->seq);

    // pseudo header & checksum
    packet_put_nl(tcp_hdr, pseudo_dest, node_ip_addr);
    packet_put_nl(tcp_hdr, pseudo_src, sock->ip);
    tcp_hdr[pseudo_zero]  = 0;
    tcp_hdr[pseudo_proto] = 6;
    tsize = size + SIZEOF_TCPHDR;
    if (size & 0x01) {  // size is odd
    //tsize++;
        tcp_hdr[tsize] = 0x00;
    }
    packet_put_n(tcp_hdr, pseudo_len, tsize);
    sum = calc_chksum(tcp_hdr + pseudo_src, tsize + 12);
    packet_put_n(tcp_hdr, tcphdr_chksum, sum);

    // IPヘッダのセット
    ip_hdr[iphdr_protocol] = IP_PROTO_TCP;
    packet_put_nl(ip_hdr, iphdr_src_addr, node_ip_addr);
    packet_put_nl(ip_hdr, iphdr_dest_addr, sock->ip);

    //IPクラスで送信処理
    ip_output(packet);

    //シーケンス番号処理
    if (flags & (TCP_FLAG_SYN | TCP_FLAG_FIN)) {
        sock->seq++;
    } else if (size > 0) {
        sock->seq += size;
    }
*/

}


/*!
    \brief receiveTcp
         Tcpプロトコル受信 処理
    \param  IP_HEADER *ipHead [in] IPヘッダへのポインタ
    \return int 結果 
        
    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
int MoTcp::receiveTcp(IP_HEADER *ipHead)
{

    int Tcp_size;
    TCP_HEADER *Tcp;

    Tcp=(TCP_HEADER*)ipHead->data;

    Tcp_size=MoPacUtl::swapShort(ipHead->len)-sizeof(IP_HEADER);
    
    // チェックサムの確認
    //TODO 面倒なので後回し。この計算じゃダメ
    //if(MoPacUtl::calcCheckSum((dword*)Tcp,Tcp_size)){
    //    logprintf("Tcp_size = %d\n",Tcp_size);
    //    logprintf("Tcp->chksum = %x\n",MoPacUtl::swapShort(Tcp->chksum));
    //    logprintf("Tcp CheckSum BAD!!\n");
    //    return 0;
    //}

    saveRecv(ipHead,Tcp_size+sizeof(IP_HEADER));

    return 0;
}




/*!
    \brief saveRecv
         Tcp応答受信 処理
    \param  IP_HEADER *ipHead [in] IPヘッダ
    \param  int size [in] パケットサイズ
    \return 無し
    \author Yamami
    \date   create:2004/09/20 update:2004/09/20
*/
void MoTcp::saveRecv(IP_HEADER *ipHead, int size)
{


    int Tcp_size;
    TCP_HEADER *Tcp;

    Tcp=(TCP_HEADER*)ipHead->data;
    Tcp_size=MoPacUtl::swapShort(ipHead->len)-sizeof(IP_HEADER)-(Tcp->headlen * 4);

    //ここで到着Tcpパケットを単にダンプ
    //if (MoPacUtl::swapShort(Tcp->dstport) == 7){
        char buf[1530];
        memset(buf,0,sizeof(buf));
        logprintf("srcip = %x\n",MoPacUtl::swapLong(ipHead->srcip));
        logprintf("srcport = %d\n",MoPacUtl::swapShort(Tcp->srcport));
        logprintf("dstport = %d\n",MoPacUtl::swapShort(Tcp->dstport));
        logprintf("seqnum = %x\n",MoPacUtl::swapLong(Tcp->seqnum));
        logprintf("acknum = %x\n",MoPacUtl::swapLong(Tcp->acknum));
        logprintf("headlen(BYTE) = %x\n",(Tcp->headlen * 4));
        logprintf("flag = %x\n",Tcp->flag);
        logprintf("wndsize = %x\n",MoPacUtl::swapShort(Tcp->wndsize));
        logprintf("chksum = %x\n",MoPacUtl::swapShort(Tcp->chksum));
        logprintf("urgpoint = %x\n",MoPacUtl::swapShort(Tcp->urgpoint));

        logprintf("size = %x\n",Tcp_size);
        
        //TODO 
        memcpy(buf, Tcp->option , Tcp_size);
        for(int i = 0; i < Tcp_size ; i++){
            logprintf("%c",buf[i]);
        }
    //}

}
