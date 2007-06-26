/*!
    \file   NE2000.cpp
    \brief  NE2000ƒhƒ‰ƒCƒoƒNƒ‰ƒX

    Copyright (c) 2004 Yamami, Higepon
    All rights reserved.
    License=MIT/X License

    \author  Yamami, Higepon
    \version $Revision: 4416 $
    \date   create:2004/07/28 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/

/*! \class Ne2000
 *  \brief Ne2000ƒhƒ‰ƒCƒoƒNƒ‰ƒX
 */

#include "NE2000.h"
#include "NE2000const.h"


using namespace mones;
/*!
    \brief initialize
         NE2000 ƒRƒ“ƒXƒgƒ‰ƒNƒ^
    \author Yamami
    \date   create:2004/08/08 update:
*/
NE2000::NE2000()
{
}

NE2000::~NE2000()
{
}

/*!
    \brief initialize
         NE2000 init
    \author Yamami
    \return int Œ‹‰Ê 0:³í A 0ˆÈŠO:ˆÙí
    \date   create:2004/08/12 update:
*/
int NE2000::init()
{
    int reti;

    //ƒvƒ‰ƒCƒx[ƒgƒƒ“ƒo‰Šú‰»
    ne_ringbuf_status=0;
    ne_ringbuf_bound=0;
    ne_ringbuf_len=0;

    ne_rx_start=0;      /* óMƒpƒPƒbƒg–{‘Ì‚ÌŠJnƒAƒhƒŒƒX */
    frame_len=0;        /* óMƒpƒPƒbƒg–{‘Ì‚Ì’·‚³ */
    ne_rx_bound=0;      /* óMŒã‚Ì‹«ŠEƒŒƒWƒXƒ^’l */
    ne_rx_write_p=0;    /* óMƒpƒPƒbƒg‘‚«‚İƒAƒhƒŒƒX */
    ne_rx_sub_len=0;    /* Ü‚è•Ô‚µ•ª‚Ì’·‚³ */
    ne_rx_remain_len=0; /* c‚è‚Ì’·‚³(Ü‚è•Ô‚µ‚ª‚È‚¢‚Æ‚«‚Í–{‘Ì‚Ì’·‚³‚Æ“¯‚¶) */

    ne_sizeof_test_pattern=20;

    //Ne2000 ‘¶İŠm”F
    reti = probe();
    if(reti != 0 ){
        _printf("Does Not Exist Ne2K!!!\n");
        return -1;
    }

    //‚±‚±‚ÅMACƒAƒhƒŒƒX‚ğ•\¦‚µ‚Ä‚İ‚é
    //int i;
    //printf("MAC ADR:");
    //for(i=0 ;i < 6 ; i++){
    //    printf("%x ",ether_mac_addr[i]);
    //}
    //printf("\n");

    //Ne2000 ‰Šú‰»
    nic_init();

    return 0;
}

/*!
    \brief  inputFrame
        NE2000 ƒf[ƒ^“ü—Íƒ‹[ƒ`ƒ“
        –{—ˆ‚ÍANE2000‚©‚ç‚ÌŠ„‚è‚İ‚É‚ÄƒR[ƒ‹‚³‚ê‚éB
        ƒeƒXƒgƒvƒƒOƒ‰ƒ€‚Å‚ÍAŒ±“I‚ÉMonaMain ‚ÅƒR[ƒ‹B
    \param  void
    \return void

    \author Yamami
    \date   create:2004/08/03 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/
void NE2000::inputFrame(void)
{

    uint8_t sts,*buf;
    //ƒoƒEƒ“ƒ_ƒŠƒŒƒWƒXƒ^ ‚ÆAƒJƒŒƒ“ƒgƒy[ƒWƒŒƒWƒXƒ^‚Í8ƒrƒbƒg•
    //ƒf[ƒ^‚ÉƒAƒNƒZƒX‚·‚éÛA8ƒrƒbƒgƒVƒtƒg‚µ‚Ä16ƒrƒbƒg•ƒAƒNƒZƒX‚ğs‚¤
    uint16_t  bnd,cpg;

    buf=frame_buf;

    // Page 0
    outp8( NE_P0_COMMAND, NE_CR_STA );
    // sts <- óMƒXƒe[ƒ^ƒXƒŒƒWƒXƒ^(Receive Status Reg)
    sts=inp8( NE_P0_RSR );

//Yamami ƒfƒoƒbƒO
//_printf("sts : %x\n",sts);

    if( ( sts & NE_RSTAT_OVER ) !=0 ){
        _printf("FIFO OverFlow\n");
        return; // óMFIFOƒI[ƒo[ƒtƒ[‚µ‚½
    }

    if( ( inp8( NE_P0_ISR ) & NE_ISR_OVW ) !=0 ){
        _printf("RING OverFlow\n");
        return; // óMƒŠƒ“ƒOƒoƒbƒtƒ@ƒI[ƒo[ƒtƒ[
    }

    //  óM¬Œ÷
    if( ( sts & NE_RSTAT_PRX ) ==0 ){
        _printf("Not Exist Packet \n");
        return; //  óMƒpƒPƒbƒg‚È‚µ
    }

    //ƒy[ƒW‚ğ–¾¦“I‚ÉØ‚è‘Ö‚¦‚Ä bnd ‚Æ cpg ‚ğ“Ç‚Ş
    outp8(NE_P0_COMMAND, NE_CR_PS0 | NE_CR_STA); /* Page 0 */
    bnd=inp8( NE_P0_BNRY ) + 1;      // bnd <-bnd
    //bnd=inp8( NE_P0_BNRY );      // bnd <-bnd ‚±‚±‚Å+1‚µ‚È‚¢
    outp8(NE_P1_COMMAND, NE_CR_PS1 | NE_CR_STA); /* Page 1 */
    cpg=inp8( NE_P1_CURR );          // cpg <- Current Page

    //Page0‚É–ß‚µ‚Ä‚¨‚­
    outp8( NE_P0_COMMAND, NE_CR_PS0 );


//Yamami ƒfƒoƒbƒO
//printf("bnd : %x\n",bnd);
//printf("cpg : %x\n",cpg);


    if( bnd == NE_RX_PAGE_STOP ){
        // if last page then set to start page
        bnd=NE_RX_PAGE_START;
    }
    if( cpg == NE_RX_PAGE_STOP ){
        /* if last page then set to start page */
        cpg=NE_RX_PAGE_START;
    }
    if( cpg == bnd ){        // Current Page = bound ?
        //printf("Not Exist Packet buffer \n");
        return;         // = ‚È‚ç ƒoƒbƒtƒ@ã‚ÉƒpƒPƒbƒg‚È‚µ
    }


    // bound+1 ƒy[ƒW‚Ìæ“ª4ƒoƒCƒg‚ğ“Ç‚İ‚Ş
    // Yamami «‚¾‚ÆA˜A‘±‚µ‚½•Ï”éŒ¾‚ªA˜A‘±‚µ‚½4ƒoƒCƒg‚Å‚ ‚é–‚É—Š‚Á‚Ä‚¢‚éHH
    // ƒoƒbƒtƒ@‚Éˆê’UƒŠ[ƒh‚µ‚ÄA‘ã“ü‚·‚éˆ—‚É•ÏX
    //ne_pio_readmem( bnd << 8, &ne_ringbuf_status, 4 );

    uint8_t bndBuf[4];
    ne_pio_readmem( bnd << 8, bndBuf, 4 );

// YamamiƒfƒoƒbƒO ƒŠ[ƒhƒAƒhƒŒƒX‚Ì•\¦
//printf("Read Src = bnd << 8 : %x\n",bnd << 8);

    ne_ringbuf_status = bndBuf[0]; /* Receive Status */
    ne_ringbuf_bound = bndBuf[1] & 0xFF; /* Next Packet Pointer */
    ne_ringbuf_len = bndBuf[3] * 256 + bndBuf[2];   /* Receive Byte Count */

    //ª‚±‚ê‚ªA‚»‚ê‚¼‚ê‹t‚Ì‚æ‚¤‚È‹C‚ª‚·‚éBƒGƒ“ƒfƒBƒAƒ“‚Ìˆá‚¢HH
    //  ‚±‚Ì–‚ÍARead/Write‘¤‚ğ•ÏX
    //ne_ringbuf_status = bndBuf[1]; /* Receive Status */
    //ne_ringbuf_bound = bndBuf[0] & 0xFF; /* Next Packet Pointer */
    //ne_ringbuf_len = bndBuf[2] * 256 + bndBuf[3];   /* Receive Byte Count */

// YamamiƒfƒoƒbƒO
//printf("ne_ringbuf_status : %x\n",ne_ringbuf_status);
//printf("ne_ringbuf_bound  : %x\n",ne_ringbuf_bound);
//printf("ne_ringbuf_len : %x\n",ne_ringbuf_len);

    ne_rx_start=(bnd << 8) + 4; // ƒpƒPƒbƒg–{‘Ì‚ÌŠJnƒAƒhƒŒƒX

    // CRC‚Ì•ª‚Ì’·‚³‚ğˆø‚­
    // CRC‚Ì•ª‚Ì’·‚³‚ğˆø‚­ ? CRC‚¶‚á‚È‚­‚Äne_ringbuf_*‚Ì4 uint8_t?
    frame_len=ne_ringbuf_len - 4; /* ƒpƒPƒbƒg–{‘Ì‚Ì’·‚³ */

    // óMI—¹Œã‚Ì‹«ŠEƒŒƒWƒXƒ^’l
    ne_rx_bound=ne_ringbuf_bound;

    if( ( ne_ringbuf_status & NE_RSTAT_PRX ) !=0 ){
                    // óM‚ª³íI—¹‚µ‚½
        if( frame_len >= ETHER_HEADER_SIZE ){
                    // Å’Z’·‚æ‚è’Z‚¢‚Æ‚«‚ÍƒGƒ‰[
            if( frame_len < ETHER_MAX_PACKET ) {
                    // Å‘å’·‚æ‚è’·‚¢‚Æ‚«‚ÍƒGƒ‰[

                ne_rx_remain_len=frame_len;

                // ƒpƒPƒbƒg‚Ìæ‚è‚İˆ—
                // Ü‚è•Ô‚µ•ª‚Ì’·‚³
                ne_rx_sub_len=NE_RX_PAGE_STOP * 256 - ne_rx_start;

// YamamiƒfƒoƒbƒO
//printf("frame_input 03 ne_rx_start=%x\n",ne_rx_start);
//printf("frame_input 04 ne_rx_sub_len=%x\n",ne_rx_sub_len);

                if( ne_rx_sub_len < frame_len ){
                    // óM‚·‚×‚«ƒpƒPƒbƒg‚ÍÜ‚è•Ô‚µ‚Ä‚¢‚é
                    // ‘O”¼•”‚Ì“Ç‚İ‚İ
                    ne_pio_readmem( ne_rx_start, buf, ne_rx_sub_len );
                    ne_rx_start=NE_RX_PAGE_START * 256;

                    // ‘‚«‚İƒAƒhƒŒƒX‚ÌXV
                    buf+=ne_rx_sub_len;
                    // c‚è‚Ì“Ç‚İ‚İ’·‚ÌXV
                    ne_rx_remain_len=frame_len - ne_rx_sub_len;
                }
                // ƒpƒPƒbƒg‚Ì“Ç‚İ‚İ
                ne_pio_readmem( ne_rx_start, buf, ne_rx_remain_len );

            }
            else{
                //printf("Error frame_len >= ETHER_MAX_PACKET \n");
            }
        }
        else{
            //printf("Error frame_len >= ETHER_HEADER_SIZE \n");
        }
    }
    else{
        //printf("Error ne_ringbuf_status & NE_RSTAT_PRX = 0 \n");
    }

    // Yamami ƒoƒEƒ“ƒ_ƒŠƒŒƒWƒXƒ^ ‚ÌXV
    bnd=ne_rx_bound;
    if( bnd == NE_RX_PAGE_START ){
        bnd=NE_RX_PAGE_STOP;
    }
    bnd--;
    outp8( NE_P0_BNRY, bnd );    // ‹«ŠEƒŒƒWƒXƒ^ = Ÿ‚Ìƒoƒbƒtƒ@ - 1

    //  Š„‚è‚İƒXƒe[ƒ^ƒXƒŒƒWƒXƒ^ƒNƒŠƒA
    outp8( NE_P0_ISR, 0xff );

    //H8 ‚æ‚è
    outp8(NE_P0_IMR, NE_IMR_PRXE); /* Packet Receive interrupt enable */

}



/*!
    \brief frame_output
        NE2000 ƒf[ƒ^o—Íƒ‹[ƒ`ƒ“
    \param  uint8_t *pkt [in] ƒf[ƒ^ƒpƒPƒbƒg‚Ö‚Ìƒ|ƒCƒ“ƒ^
    \param  uint8_t *mac [in] ‘—‚èæMACƒAƒhƒŒƒX‚Ö‚Ìƒ|ƒCƒ“ƒ^
    \param  uint32_t size [in] ƒpƒPƒbƒgƒTƒCƒY
    \param  uint16_t pid [in] ƒvƒƒgƒRƒ‹ID(ETHER_PROTO)
    \return void

    \author Yamami
    \date   create:2004/08/03 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/
void NE2000::outputFrame( uint8_t *pkt, uint8_t *mac, uint32_t size, uint16_t pid )
{

    uint32_t        ptx_type=0;
    uint32_t        ptx_size=0;
    uint8_t       *ptx_packet=0;
    uint8_t       *ptx_dest=0;


    // ‘—M‚ªŠ®—¹‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©ƒ`ƒFƒbƒN‚·‚é
    while( ( inp8( NE_P0_COMMAND ) & 0x04 ) !=0 );

//Yamami ƒfƒoƒbƒO
//printf("frame_output 01\n");

    ptx_dest=mac;
    ptx_size=size;
    ptx_packet=pkt;
    // ƒlƒbƒgƒ[ƒNƒoƒCƒgƒI[ƒ_[‚É•ÏŠ·‚·‚é
    // Yamami •ÏŠ·•s—vH
    ptx_type=(pid >> 8)+(pid << 8);

//Yamami ƒfƒoƒbƒO
//int i;
//for(i=0 ; i<2 ; i++){
//    printf("ptx[1] = %x \n",(uint8_t *)(&ptx_type + 1));
//}


    // Š„‚è‚İ‹Ö~
    // ‘—Mˆ—’†‚ÉóM‚·‚é‚ÆƒŒƒWƒXƒ^‚ª‹¶‚Á‚Ä‚µ‚Ü‚¤
    //disableInterrupt();
    disableNetwork();

    // ˆ¶æƒAƒhƒŒƒX‚Ì‘‚«‚İ
//    ne_pio_writemem( ptx_dest, NE_TX_PAGE_START << 8, 6 );
    // ‘—MŒ³ƒAƒhƒŒƒX‚Ì‘‚«‚İ
//    ne_pio_writemem( ether_mac_addr, ( NE_TX_PAGE_START << 8 ) + 6, 6 );
    // ƒvƒƒgƒRƒ‹ID‚Ì‘‚«‚İ
//    ne_pio_writemem( (uint8_t *)&ptx_type, ( NE_TX_PAGE_START << 8 ) + 12, 2 );
    // ƒf[ƒ^•”•ª‚Ì‘‚«‚İ
//    ne_pio_writemem( ptx_packet, ( NE_TX_PAGE_START << 8 ) + 14, ptx_size );
    ne_pio_writemem( ptx_packet, ( NE_TX_PAGE_START << 8 ) , ptx_size );

    ptx_size+=ETHER_HEADER_SIZE;

    // Å¬ƒpƒPƒbƒg’·‚æ‚è’Z‚¢‚©‚Ç‚¤‚©‚ğƒ`ƒFƒbƒN‚·‚é
    if( ptx_size < ETHER_MIN_PACKET ){
        ptx_size=ETHER_MIN_PACKET;
    }

    outp8( NE_P0_COMMAND, NE_CR_PS0 + NE_CR_RD2 + NE_CR_STA );

    // ‘—Mƒoƒbƒtƒ@—Ìˆæ‚Ìw’è
    outp8( NE_P0_TPSR, NE_TX_PAGE_START );

    // ‘—M’·‚Ìw’è
    outp8( NE_P0_TBCR0, ptx_size & 0xff);
    outp8( NE_P0_TBCR1, ptx_size >> 8 );

    // ‘—M–½—ß‚ğ”­s‚·‚é
    outp8( NE_P0_COMMAND, NE_CR_PS0 + NE_CR_TXP + NE_CR_RD2 + NE_CR_STA );

    // Š„‚è‚İ‹–‰Â
    //enableInterrupt();
    enableNetwork();

//Yamami ƒfƒoƒbƒO
//printf("frame_output 02\n");

    // ‘—M‚ªŠ®—¹‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©ƒ`ƒFƒbƒN‚·‚é
    // 2004/11/16 Yamami QEMU on Ne2000 ‚¾‚Æ‚±‚Ìƒ`ƒFƒbƒN‚ª‰i‰“‚É’Ê‚ç‚È‚¢‚æ‚¤‚È‚Ì‚Åƒ`ƒFƒbƒN‚µ‚È‚¢
    //while( ( inp8( NE_P0_COMMAND ) & 0x04 ) !=0 );

//Yamami ƒfƒoƒbƒO
//printf("frame_output 03\n");

}


/*!
    \brief nic_probe
        NE2000ŒŸ¸ƒ‹[ƒ`ƒ“
        ƒoƒbƒtƒ@ƒƒ‚ƒŠ‚É‘‚«‚İ‚Æ“Ç‚İ‚İ‚ğs‚¢ANE2000‚ª‘¶İ‚·‚é‚±‚Æ‚ğŠm”F‚·‚é
    \param  void
    \return void

    \author Yamami
    \date   create:2004/08/01 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/
int NE2000::probe(void)
{
    int i;

    /* ƒ\ƒtƒgƒEƒFƒAƒŠƒZƒbƒg */
    //outp8( NE_ASIC_RESET, inp8( NE_ASIC_RESET ) );
    /* ƒŠƒZƒbƒgŠ®—¹‚Ü‚Å‘Ò‚Â */
    //ne_wait_func(0);

    /* DMA ‚ğ‹­§’â~‚·‚éB */
    outp8( NE_P0_COMMAND, NE_CR_RD2 + NE_CR_STP );

    /* ’â~‚·‚é ‚Ü‚Å‘Ò‚Â */
    //ne_wait_func(0);

    //  ƒpƒPƒbƒg‚ªƒƒ‚ƒŠ‚É‘‚©‚ê‚È‚¢‚æ‚¤‚É‚·‚é
    outp8( NE_P0_RCR, NE_RCR_MON );

    // ƒ[ƒh•“]‘—Aƒ‹[ƒvƒoƒbƒNƒ‚[ƒh
    outp8( NE_P0_DCR, NE_DCR_WTS + NE_DCR_FT1 + NE_DCR_LS );

    // óMƒoƒbƒtƒ@ŠJnƒAƒhƒŒƒX
    // 64
    outp8( NE_P0_PSTART, NE_MEM_START );

    // óMƒoƒbƒtƒ@I—¹ƒAƒhƒŒƒX
    // 128
    outp8( NE_P0_PSTOP, NE_MEM_END );

    // ƒƒ‚ƒŠƒeƒXƒgƒpƒ^[ƒ“‘‚«‚İ
    ne_pio_writemem( (uint8_t *)ne_test_pattern, NE_MEM_START * NE_PAGE_SIZE, ne_sizeof_test_pattern );
    // ƒƒ‚ƒŠƒeƒXƒgƒpƒ^[ƒ““Ç‚İ‚İ
    ne_pio_readmem( NE_MEM_START * NE_PAGE_SIZE, ne_test_buffer, ne_sizeof_test_pattern );
    // ƒeƒXƒgƒpƒ^[ƒ“‚Ì”äŠr
    if( ne_bcompare( (uint8_t *)ne_test_pattern, ne_test_buffer, ne_sizeof_test_pattern )!=0 )
        return(1);  // •sˆê’v‚È‚çI—¹

    // EEPROM ƒf[ƒ^“Ç‚İ‚¾‚µ
    ne_pio_readmem( 0, ne_test_buffer, 16 );

    // ƒC[ƒTƒlƒbƒgƒAƒhƒŒƒXæ“¾
    for(i=0;i<11;i+=2)
        ether_mac_addr[i/2]=ne_test_buffer[i];

    // Š„‚è‚İƒXƒe[ƒ^ƒXƒŒƒWƒXƒ^ƒNƒŠƒA
    outp8( NE_P0_ISR, 0xff );

    return(0);
}


/*!
    \brief nic_init
        NE2000‰Šú‰»ƒ‹[ƒ`ƒ“
    \param  void
    \return void

    \author Yamami
    \date   create:2004/07/28 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/
void NE2000::nic_init(void)
{
    // Še•Ï”‚Ì‰Šú‰»
    int i;
    uint8_t c;

    //NICƒŠƒZƒbƒg
    c = inp8(NE_ASIC_RESET);
    outp8(NE_ASIC_RESET, c);

    //ƒŠƒZƒbƒgŠ®—¹‚Ü‚Å‘Ò‚Â
    sleep(300);

    // ƒŠƒ‚[ƒgDMA ’â~
    outp8( NE_P0_COMMAND, ne_cr_proto | NE_CR_STP );

    // FIFO ƒXƒŒƒbƒVƒ‡ƒ‹ƒh 8Byte,ƒŠƒ‚[ƒgDMA ©“®‰Šú‰»‹Ö~
    // 8086 ƒoƒCƒgƒI[ƒ_,16bit• DMA “]‘—
    // Page0_0Eh DATA CONFIGURATION REGISTER (DCR) 0EH (WRITE)
    //   7 6   5   4   3  2   1    0
    //   - FT1 FT0 ARM LS LAS BOS WTS
    //   0 1   0   0   1   0   0    1
    outp8( NE_P0_DCR, NE_DCR_FT1 + NE_DCR_WTS + NE_DCR_LS );

    // ƒŠƒ‚[ƒgDMA ƒoƒCƒgƒJƒEƒ“ƒ^ƒNƒŠƒA
    outp8( NE_P0_RBCR0, 0 );
    outp8( NE_P0_RBCR1, 0 );

    // ƒ‚ƒjƒ^ƒ‚[ƒh
    // (ƒpƒPƒbƒgóMˆ—‚ğ‚¨‚±‚È‚¤‚ªAƒoƒbƒtƒ@ƒŠƒ“ƒO‚Í‚µ‚È‚¢)
    // RECEIVE CONFIGURATION REGISTER (RCR) 0CH (WRITE)
    outp8( NE_P0_RCR, NE_RCR_MON );

    // “à•”ƒ‹[ƒvƒoƒbƒNƒ‚[ƒh
    // TRANSMIT CONFIGURATION REGISTER (TCR) 0DH (WRITE)
    outp8( NE_P0_TCR, NE_TCR_LB0 );

    // ‘—MƒŠƒ“ƒOƒoƒbƒtƒ@ŠJnƒAƒhƒŒƒX‚Ìİ’è
    // 64
    outp8( NE_P0_TPSR, NE_TX_PAGE_START );
    // óMƒŠƒ“ƒOƒoƒbƒtƒ@ŠJnƒAƒhƒŒƒX‚Ìİ’è
    // 70
    outp8( NE_P0_PSTART, NE_RX_PAGE_START );

    // óMƒŠƒ“ƒOƒoƒbƒtƒ@‹«ŠEƒAƒhƒŒƒX‚Ìİ’è
    // 70
    outp8( NE_P0_BNRY, NE_RX_PAGE_START );
    // óMƒŠƒ“ƒOƒoƒbƒtƒ@I—¹ƒAƒhƒŒƒX‚Ìİ’è
    // 128
    outp8( NE_P0_PSTOP, NE_RX_PAGE_STOP );

    // Š„‚è‚İƒXƒe[ƒ^ƒXƒŒƒWƒXƒ^‚ÌƒNƒŠƒA
    outp8( NE_P0_ISR, 0xff );
    //  Š„‚è‚İ‹–‰ÂğŒ‚Ìİ’è
    // Packet recieve successful
    outp8( NE_P0_IMR, NE_IMR_PRXE );

    //Yamami ‘SŠ„‚è‚İ‚ğ‹–‰Â‚µ‚Ä‚İ‚é
    //outp8( NE_P0_IMR, 0x7F );

    // Page 1 ‚Ìİ’è
    outp8( NE_P0_COMMAND, ne_cr_proto | ( NE_CR_PS1 + NE_CR_STP ) );

    // Ethernet ƒAƒhƒŒƒX‚Ìİ’è
    // ‚±‚±‚Åw’è‚µ‚½ƒAƒhƒŒƒX‚ÌƒpƒPƒbƒg‚ğó‚¯æ‚é
    for(i=0;i<6;i++){
        outp8( NE_P1_PAR0 + i, ether_mac_addr[i] );
    }

    // Å‰‚ÉóM‚µ‚½ƒpƒPƒbƒg‚ğŠi”[‚·‚éƒAƒhƒŒƒX‚Ìİ’è
    outp8( NE_P1_CURR, NE_RX_PAGE_START + 1 );

    /* ƒ}ƒ‹ƒ`ƒLƒƒƒXƒgƒŒƒWƒXƒ^‚Ìİ’è */
    outp8( NE_P1_MAR0, 0 );
    outp8( NE_P1_MAR0+1, 0 );
    outp8( NE_P1_MAR0+2, 0 );
    outp8( NE_P1_MAR0+3, 0 );
    outp8( NE_P1_MAR0+4, 0 );
    outp8( NE_P1_MAR0+5, 0 );
    outp8( NE_P1_MAR0+6, 0 );
    outp8( NE_P1_MAR0+7, 0 );

    // Page 0 ‚É‚à‚Ç‚·
    outp8( NE_P0_COMMAND, ne_cr_proto | NE_CR_STP );

    // óMƒpƒPƒbƒgƒtƒBƒ‹ƒ^‚Ìİ’è
    // ƒuƒ[ƒhƒLƒƒƒXƒg‚Æ©•ªˆ¶‚Ì‚İ‚ğƒƒ‚ƒŠ‚ÉŠi”[
    // accept broadcast
    outp8( NE_P0_RCR, NE_RCR_AB );
// ª ‚ğƒRƒƒ“ƒgƒAƒEƒg‚·‚é‚Æ
// Yamamiƒ‚ƒjƒ^ƒ‚[ƒh‚Ì‚Ü‚Ü‚Æ‚µ‚Ä‚İ‚é!!!!! –‚É‚È‚é

    //Yamami ƒvƒƒ~ƒXƒLƒƒƒXƒg‚Ì‚İó‚¯æ‚é‚æ‚¤‚É‚µ‚Ä‚İ‚é???? ˆá‚¤‚æ‚¤‚¾
    //outp8( NE_P0_RCR, NE_RCR_APROMIS );

    // NIC ‚ğƒAƒNƒeƒBƒu‚É‚·‚é
    outp8( NE_P0_COMMAND, ne_cr_proto | NE_CR_STA );

    // ƒ‹[ƒvƒoƒbƒNƒ‚[ƒh‚ğ”²‚¯‚Ä’Êí“®ìƒ‚[ƒh‚É“ü‚é
    outp8( NE_P0_TCR, 0 );
}





/*!
    \brief ne_pio_writemem
        ”ñŒöŠJƒ‹[ƒ`ƒ“
        Ne2000 ƒoƒbƒtƒ@ƒƒ‚ƒŠ‘‚«‚İ
    \param  uint8_t *src [in] “]‘—Œ³ƒAƒhƒŒƒX
    \param  uint32_t dest [in] “]‘—æƒAƒhƒŒƒX
    \param  uint32_t size [in] ’·‚³
    \return void

    \author Yamami
    \date   create:2004/08/02 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/
void NE2000::ne_pio_writemem( uint8_t *src, uint32_t dest, uint32_t size )
{
    uint32_t i;

    uint16_t writetmp;

    /* ƒXƒe[ƒ^ƒXƒŒƒWƒXƒ^ƒNƒŠƒA */
    outp8( NE_P0_COMMAND, NE_CR_RD2 + NE_CR_STA );
    outp8( NE_P0_ISR, NE_ISR_RDC);

    /* ’·‚³ */
    outp8( NE_P0_RBCR0, size & 0xff );
    outp8( NE_P0_RBCR1, size >> 8 );

    /* “]‘—æƒAƒhƒŒƒX */
    outp8( NE_P0_RSAR0, dest & 0xff );
    outp8( NE_P0_RSAR1, dest >> 8 );
    outp8( NE_P0_COMMAND, NE_CR_RD1 + NE_CR_STA );

//    for(i=0;i<size;i+=2){
//        outp8( PIO_ADATA, *(src+1) );
//        outp8( NE_ASIC_DATA, *src );
//        src+=2;
//    }


    // 2004/08/02 DATA‚Í16ƒrƒbƒg•‚Å‚â‚è‚Æ‚è‚·‚é‚Ì‚ÅAWord•ÏŠ·‚µ‚ÄI/O
    for(i = 0 ; i < size ; i+=2 , src+=2){
        //writetmp = (uint16_t)(*(src) << 8) + (uint16_t)*(src+1);
        //ƒŠƒgƒ‹ƒGƒ“ƒfƒBƒAƒ“‚È‚ç‚±‚¤HH
        writetmp = (uint16_t)(*(src + 1) << 8) + (uint16_t)*(src);
        outp16( NE_ASIC_DATA, writetmp );
    }

    /* wait */
    for(i=0;i<0xff;i++){
        if( ( inp8(NE_P0_ISR) & NE_ISR_RDC ) == 0 )
            break;
    }

}


/*!
    \brief ne_pio_readmem
        ”ñŒöŠJƒ‹[ƒ`ƒ“
         NE2000 ‚Ìƒƒ‚ƒŠ‚©‚ç“Ç‚İ‚¾‚µ
    \param  uint32_t src [in] “]‘—Œ³ƒAƒhƒŒƒX
    \param  uint8_t *dest [in] “]‘—æƒAƒhƒŒƒX
    \param  uint32_t size [in] ’·‚³
    \return void

    \author Yamami
    \date   create:2004/08/02 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/
void NE2000::ne_pio_readmem( uint32_t src, uint8_t *dest, uint32_t size )
{
    uint32_t i;

    uint16_t readtmp;

//Yamami ƒfƒoƒbƒO
//printf("ne_pio_readmem src=%x \n",src);

    // abort DMA, start NIC
    outp8( NE_P0_COMMAND, NE_CR_RD2 + NE_CR_STA );
    // length
    outp8( NE_P0_RBCR0, size & 0xff );
    outp8( NE_P0_RBCR1, size >> 8 );
    // source address
    outp8( NE_P0_RSAR0, src & 0xff );
    outp8( NE_P0_RSAR1, src >> 8 );
    outp8( NE_P0_COMMAND, NE_CR_RD0 + NE_CR_STA );

//    for(i=0;i<size;i+=2){
//        *dest=inp8( NE_ASIC_DATA );
//        *(dest+1)=inp8( PIO_ADATA );
//        dest+=2;
//    }

    // 2004/08/02 DATA‚Í16ƒrƒbƒg•‚Å‚â‚è‚Æ‚è‚·‚é‚Ì‚ÅAWord•ÏŠ·‚µ‚ÄI/O
    for(i = 0 ; i < size ; i+=2 , dest+=2){
        readtmp=inp16( NE_ASIC_DATA );
        //*dest=(uint8_t)(readtmp >> 8);
        //*(dest+1)=(uint8_t)(readtmp & 0xff);
        //ƒŠƒgƒ‹ƒGƒ“ƒfƒBƒAƒ“‚È‚ç‚±‚¤HH
        *(dest+1)=(uint8_t)(readtmp >> 8);
        *(dest)=(uint8_t)(readtmp & 0xff);
    }

}



/*!
    \brief ne_bcompare
        ”ñŒöŠJƒ‹[ƒ`ƒ“
         ƒoƒCƒiƒŠ”äŠrƒ‹[ƒ`ƒ“
    \param  uint8_t *src [in] ”äŠrŒ³ƒAƒhƒŒƒX
    \param  uint8_t *dest [in] ”äŠræƒAƒhƒŒƒX
    \param  uint32_t size [in] ’·‚³
    \return int Œ‹‰Ê:ˆê’v==0,•sˆê’v==0ˆÈŠO

    \author Yamami
    \date   create:2004/08/02 update:$Date: 2007-06-24 18:19:54 +0900 (æ—¥, 24  6æœˆ 2007) $
*/
int NE2000::ne_bcompare( uint8_t *src, uint8_t *dest, uint32_t size )
{
    uint32_t i;

    for(i=0;i<size;i++){
        if( src[i]!=dest[i] )
            return(1);
    }

    return(0);
}

uint32_t NE2000::getFrameBufferSize()
{
    return frame_len;
}
void NE2000::getFrameBuffer(uint8_t* buffer, uint32_t size)
{
    uint32_t maxSize = getFrameBufferSize();
    size = size < maxSize ? size : maxSize;
    memcpy(buffer, this->frame_buf, size);
}

void NE2000::getMacAddress(uint8_t* dest)
{
    memcpy(dest, ether_mac_addr, 6);
}
