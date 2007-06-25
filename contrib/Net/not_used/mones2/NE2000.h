/*!
    \file  NE2000.h
    \brief NE2000ドライバクラス ヘッダ

    Copyright (c) 2004 Yamami, Higepon
    All rights reserved.
    License=MIT/X License

    \author  Yamami Higepon
    \version $Revision$
    \date   create:2004/08/08 update:$Date$
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
public:
    NE2000();
    virtual ~NE2000();

    int init();
    int probe();
    void inputFrame();
    void outputFrame(uint8_t* packet, uint8_t* macAddress, uint32_t size, uint16_t protocolId);

    virtual uint32_t getFrameBufferSize();
    virtual void getFrameBuffer(uint8_t* buffer, uint32_t size);
    void getMacAddress(uint8_t* dest);
    uint8_t getIRQ() const {return this->nicIRQ;}
    int getIOBase() const {return this->nicIo_Base;}
    void setIRQ(uint8_t irq) {this->nicIRQ = irq;}
    void setIOBase(int iobase) {this->nicIo_Base = iobase;}

private:
    int nicIRQ;
    int nicIo_Base;

    void nic_init(void);
    void ne_pio_writemem( uint8_t *, uint32_t, uint32_t );
    void ne_pio_readmem( uint32_t, uint8_t *, uint32_t );
    int ne_bcompare( uint8_t *, uint8_t *, uint32_t );
    void xfer_buf( uint8_t *, uint8_t *, uint32_t, uint32_t *, uint32_t, uint32_t,uint32_t );

    //privateメンバ
    /* 受信リングバッファのリンク情報領域を読み込むところ */
    /* ne_ring_buffer */
    uint8_t       ne_ringbuf_status;
    uint8_t       ne_ringbuf_bound;
    uint32_t        ne_ringbuf_len;

    uint32_t        ne_rx_start;      /* 受信パケット本体の開始アドレス */

    uint8_t       ne_rx_bound;      /* 受信後の境界レジスタ値 */
    uint32_t        ne_rx_write_p;    /* 受信パケット書き込みアドレス */
    uint32_t        ne_rx_sub_len;    /* 折り返し分の長さ */
    uint32_t        ne_rx_remain_len; /* 残りの長さ(折り返しがないときは本体の長さと同じ) */
    /*! \brief 受信パケット本体の長さ */ 
    uint32_t    frame_len;
    uint8_t   frame_buf[1500];
    uint8_t   ether_mac_addr[6];
    /* NE2000 ワークエリア */
    int        ne_sizeof_test_pattern;
    uint8_t       ne_test_buffer[20];
};

}; // namespace mones
#endif
