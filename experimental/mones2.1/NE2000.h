/*!
    \file  NE2000.h
    \brief NE2000ドライバクラス ヘッダ

    Copyright (c) 2004 Yamami, Higepon
    All rights reserved.
    License=MIT/X License

    \author  Yamami Higepon
    $Date: 2006/01/02 15:12:55 $
*/

#pragma once
#include <sys/types.h>
#include <monapi.h>
#include "Nic.h"

// don't include NE2000const.h at here.

namespace mones {

class NE2000 : public Nic
{
public:
    void  Send(Ether::Frame*);
    Ether::Frame* Recv(int n);
    int interrupt();
    NE2000();
    ~NE2000();
    int init();
    void getMacAddress(byte* dest){ memcpy(dest, ether_mac_addr, 6);}
private:
    void w_reg(word reg,byte val){ outp8(iobase+reg,val); };
    byte r_reg(word reg){ return inp8(iobase+reg); };
    void w_regw(word reg,word val){ outp16(iobase+reg,val); };
    word r_regw(word reg){ return inp16(iobase+reg); };

    void ne_pio_writemem( byte *, dword, dword );
    void ne_pio_readmem( dword, byte *, dword );
/////////
    void outputFrame(byte* packet, byte* macAddress, dword size, word protocolId);
    void getFrameBuffer(byte* buffer, dword size);
////////
    byte       ne_ringbuf_status;
    byte       ne_ringbuf_bound;
    dword      ne_ringbuf_len;
    dword      ne_rx_start;      /* 受信パケット本体の開始アドレス */
    byte       ne_rx_bound;      /* 受信後の境界レジスタ値 */
    dword      ne_rx_write_p;    /* 受信パケット書き込みアドレス */
    dword      ne_rx_sub_len;    /* 折り返し分の長さ */
    dword      ne_rx_remain_len; /* 残りの長さ(折り返しがないときは本体の長さと同じ) */ 
    dword      frame_len;
    byte       frame_buf[1500];
    byte       ether_mac_addr[6];
};

}; // namespace mones

