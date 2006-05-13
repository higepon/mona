/*!
    \file  NE2000.h
    \brief NE2000ドライバクラス ヘッダ

    Copyright (c) 2004 Yamami, Higepon
    All rights reserved.
    License=MIT/X License

    \author  Yamami Higepon
    \version $Revision: 1.1 $
    \date   create:2004/08/08 update:$Date: 2006/01/02 15:12:55 $
*/
#ifndef _MONA_MONES_NE2000_
#define _MONA_MONES_NE2000_

#include <sys/types.h>
#include <monapi.h>
#include "Nic.h"

// don't include NE2000const.h at here.

namespace mones {

class NE2000 : public Nic
{
    void inputFrame();	
    void outputFrame(byte* packet, byte* macAddress, dword size, word protocolId);
    dword getFrameBufferSize();
    void getFrameBuffer(byte* buffer, dword size);
public:
    void  Send(Ether::Frame*){};
    Ether::Frame* Read(int n){return NULL;};
	int interrupt(){return 0;};

    NE2000();
    ~NE2000();
    int init();
    int probe();
	void getMacAddress(byte* dest);
    byte getIRQ() const {return this->nicIRQ;}
    int getIOBase() const {return this->nicIo_Base;}
    void setIRQ(byte irq) {this->nicIRQ = irq;}
    void setIOBase(int iobase) {this->nicIo_Base = iobase;}
private:
    int nicIRQ;
    int nicIo_Base;
    void nic_init(void);
    void ne_pio_writemem( byte *, dword, dword );
    void ne_pio_readmem( dword, byte *, dword );
    int ne_bcompare( byte *, byte *, dword );
    void xfer_buf( byte *, byte *, dword, dword *, dword, dword,dword );

    //privateメンバ
    /* 受信リングバッファのリンク情報領域を読み込むところ */
    /* ne_ring_buffer */
    byte       ne_ringbuf_status;
    byte       ne_ringbuf_bound;
    dword      ne_ringbuf_len;
    dword      ne_rx_start;      /* 受信パケット本体の開始アドレス */
    byte       ne_rx_bound;      /* 受信後の境界レジスタ値 */
    dword      ne_rx_write_p;    /* 受信パケット書き込みアドレス */
    dword      ne_rx_sub_len;    /* 折り返し分の長さ */
    dword      ne_rx_remain_len; /* 残りの長さ(折り返しがないときは本体の長さと同じ) */
    /*! \brief 受信パケット本体の長さ */ 
    dword      frame_len;
    byte       frame_buf[1500];
    byte       ether_mac_addr[6];
    /* NE2000 ワークエリア */
    int        ne_sizeof_test_pattern;
    byte       ne_test_buffer[20];
};

}; // namespace mones
#endif
