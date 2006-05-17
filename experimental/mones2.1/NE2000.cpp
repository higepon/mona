///////////////////////////////
//This file is derived form NE2000.cpp by Mr. Yamami.
///////////////////////////////

#include "NE2000.h"

using namespace mones;

NE2000::NE2000()
{
    ne_ringbuf_status=0;
    ne_ringbuf_bound=0;
    ne_ringbuf_len=0;
    ne_rx_start=0;      /* 受信パケット本体の開始アドレス */
    frame_len=0;        /* 受信パケット本体の長さ */
    ne_rx_bound=0;      /* 受信後の境界レジスタ値 */
    ne_rx_write_p=0;    /* 受信パケット書き込みアドレス */
    ne_rx_sub_len=0;    /* 折り返し分の長さ */
    ne_rx_remain_len=0; /* 残りの長さ(折り返しがないときは本体の長さと同じ) */
}

NE2000::~NE2000()
{

}

int NE2000::init(void)
{
    // DMA を強制停止する。 
    w_reg( NE_P0_COMMAND, NE_CR_RD2|NE_CR_STP );
    //  パケットがメモリに書かれないようにする
    w_reg( NE_P0_RCR, NE_RCR_MON );
    // ワード幅転送、ループバックモード
    w_reg( NE_P0_DCR, NE_DCR_WTS | NE_DCR_FT1 | NE_DCR_LS );
    // 受信バッファ開始アドレス
    w_reg( NE_P0_PSTART, NE_MEM_START );
    // 受信バッファ終了アドレス
    w_reg( NE_P0_PSTOP, NE_MEM_END );
    // EEPROM データ読みだし
    byte buf[16];
    ne_pio_readmem( 0, buf, 16 );
    // イーサネットアドレス取得
    for(int i=0;i<6;i++){
        macaddress[i]=buf[2*i];
        printf("%x.",macaddress[i]);
    }
    printf("\n");
    // 割り込みステータスレジスタクリア
    w_reg( NE_P0_ISR, 0xff );
    //NICリセット
    w_reg(NE_ASIC_RESET, r_reg(NE_ASIC_RESET));
    //リセット完了まで待つ
    sleep(300);
    // リモートDMA 停止
    w_reg( NE_P0_COMMAND, NE_CR_RD2 | NE_CR_STP );
    // FIFO スレッショルド 8Byte,リモートDMA 自動初期化禁止
    // 8086 バイトオーダ,16bit幅 DMA 転送
    // Page0_0Eh DATA CONFIGURATION REGISTER (DCR) 0EH (WRITE)
    //   7 6   5   4   3  2   1    0
    //   - FT1 FT0 ARM LS LAS BOS WTS
    //   0 1   0   0   1   0   0    1
    w_reg( NE_P0_DCR, NE_DCR_FT1 + NE_DCR_WTS + NE_DCR_LS );

    // リモートDMA バイトカウンタクリア
    w_reg( NE_P0_RBCR0, 0 );
    w_reg( NE_P0_RBCR1, 0 );

    // モニタモード
    // (パケット受信処理をおこなうが、バッファリングはしない)
    // RECEIVE CONFIGURATION REGISTER (RCR) 0CH (WRITE)
    w_reg( NE_P0_RCR, NE_RCR_MON );

    // 内部ループバックモード
    // TRANSMIT CONFIGURATION REGISTER (TCR) 0DH (WRITE)
    w_reg( NE_P0_TCR, NE_TCR_LB0 );

    // 送信リングバッファ開始アドレスの設定
    // 64
    w_reg( NE_P0_TPSR, NE_TX_PAGE_START );
    // 受信リングバッファ開始アドレスの設定
    // 70
    w_reg( NE_P0_PSTART, NE_RX_PAGE_START );

    // 受信リングバッファ境界アドレスの設定
    // 70
    w_reg( NE_P0_BNRY, NE_RX_PAGE_START );
    // 受信リングバッファ終了アドレスの設定
    // 128
    w_reg( NE_P0_PSTOP, NE_RX_PAGE_STOP );

    // 割り込みステータスレジスタのクリア
    w_reg( NE_P0_ISR, 0xff );
    //  割り込み許可条件の設定
    w_reg( NE_P0_IMR, NE_IMR_PRXE | NE_IMR_PTXE );

    // Page 1 の設定
    w_reg( NE_P0_COMMAND, NE_CR_RD2 | ( NE_CR_PS1 + NE_CR_STP ) );

    // Ethernet アドレスの設定
    for(int i=0;i<6;i++){
        w_reg( NE_P1_PAR0 + i, macaddress[i] );
    }

    // 最初に受信したパケットを格納するアドレスの設定
    w_reg( NE_P1_CURR, NE_RX_PAGE_START + 1 );

    /* マルチキャストレジスタの設定 */
    for(int i=0;i<8;i++)
        w_reg( NE_P1_MAR0+i, 0 );

    // Page 0 にもどす
    w_reg( NE_P0_COMMAND, NE_CR_RD2 | NE_CR_STP );

    // 受信パケットフィルタの設定
    // ブロードキャストと自分宛のみをメモリに格納
    // accept broadcast
    w_reg( NE_P0_RCR, NE_RCR_AB );
    // NIC をアクティブにする
    w_reg( NE_P0_COMMAND, NE_CR_RD2 | NE_CR_STA );

    // ループバックモードを抜けて通常動作モードに入る
    w_reg( NE_P0_TCR, 0 );
    printf("initalize completed.\n");
    return 0;
}

int NE2000::interrupt() 
{
    byte ret=0x0000;
    byte val = r_reg(NE_P0_ISR);
    if( val & NE_ISR_PRX){
        rxihandler();
        ret |= RX_INT;
    }
    if (val & NE_ISR_PTX){
        txihandler();
        ret |= TX_INT;
    }
    if( val & (NE_ISR_RXE|NE_ISR_TXE|NE_ISR_OVW|NE_ISR_CNT|NE_ISR_RST)){
        ret |= ER_INT;
    }
    w_reg( NE_P0_ISR, 0xFF );
    //Interrupt was masked by OS handler.
    enableNetwork(); //Now be enabled here. 
    return ret;
}

int NE2000::rxihandler()//void NE2000::inputFrame(void)
{
    // Page 0
    w_reg( NE_P0_COMMAND, NE_CR_STA );
    byte sts=r_reg( NE_P0_ISR );//Recive Stats Register.
    if( ( sts & NE_RSTAT_OVER ) !=0 ){
        printf("RX FIFO OverFlow\n");
        return ER_INT;
    }
    if( ( r_reg( NE_P0_ISR ) & NE_ISR_OVW ) !=0 ){
        printf("RX RING OverFlow\n");
        return ER_INT;
    }
    if( ( sts & NE_RSTAT_PRX ) ==0 ){
        printf("Not Exist RX Packet \n");
        return ER_INT;
    }

    w_reg(NE_P0_COMMAND, NE_CR_PS0 | NE_CR_STA); // Page 0
    word bnd=r_reg( NE_P0_BNRY ) + 1;      // Boundary
    w_reg(NE_P1_COMMAND, NE_CR_PS1 | NE_CR_STA); // Page 1
    word cpg=r_reg( NE_P1_CURR );          //Current Page
    //Page0に戻しておく
    w_reg( NE_P0_COMMAND, NE_CR_PS0 );

    if( bnd == NE_RX_PAGE_STOP ){
        // if last page then set to start page
        bnd=NE_RX_PAGE_START;
    }
    if( cpg == NE_RX_PAGE_STOP ){
        /* if last page then set to start page */
        cpg=NE_RX_PAGE_START;
    }
    if( cpg == bnd ){        // Current Page = bound ?
        printf("Not Exist RX Packet buffer \n");
        return ER_INT;
    }

    // bound+1 ページの先頭4バイトを読み込む
    byte bndBuf[4];
    ne_pio_readmem( bnd << 8, bndBuf, 4 );

    ne_ringbuf_status = bndBuf[0]; /* Receive Status */
    ne_ringbuf_bound = bndBuf[1] & 0xFF; /* Next Packet Pointer */
    ne_ringbuf_len = bndBuf[3] * 256 + bndBuf[2];   /* Receive Byte Count */

    ne_rx_start=(bnd << 8) + 4; // パケット本体の開始アドレス

    // CRCの分の長さを引く
    frame_len=ne_ringbuf_len - 4; /* パケット本体の長さ */

    // 受信終了後の境界レジスタ値
    ne_rx_bound=ne_ringbuf_bound;

    if( ( ne_ringbuf_status & NE_RSTAT_PRX ) !=0 ){//受信正常終了
        if((ETHER_HEADER_SIZE <= frame_len)  && (frame_len < ETHER_MAX_PACKET )) {
            ne_rx_remain_len=frame_len;
            // パケットの取り込み処理
            // 折り返し分の長さ
            ne_rx_sub_len=NE_RX_PAGE_STOP * 256 - ne_rx_start;
            //byte* buf=frame_buf;
            Ether* frame= new Ether;
            byte* buf=(byte*)frame;
            if( ne_rx_sub_len < frame_len ){
                // 受信すべきパケットは折り返している
                // 前半部の読み込み
                ne_pio_readmem( ne_rx_start, buf, ne_rx_sub_len );
                ne_rx_start=NE_RX_PAGE_START * 256;

                // 書き込みアドレスの更新
                buf+=ne_rx_sub_len;
                // 残りの読み込み長の更新
                ne_rx_remain_len=frame_len - ne_rx_sub_len;
            }
            // パケットの読み込み
            ne_pio_readmem( ne_rx_start, buf, ne_rx_remain_len );            
            frame->payloadsize=frame_len;
            rxFrameList.add(frame);
        }
    }
    // Yamami バウンダリレジスタ の更新
    bnd=ne_rx_bound;
    if( bnd == NE_RX_PAGE_START ){
        bnd=NE_RX_PAGE_STOP;
    }
    bnd--;
    w_reg( NE_P0_BNRY, bnd );    // 境界レジスタ = 次のバッファ - 1

    //  割り込みステータスレジスタクリア
    w_reg( NE_P0_ISR, 0xff );
 
    w_reg(NE_P0_IMR, NE_IMR_PRXE); /* Packet Receive interrupt enable */
    enableNetwork();
    return 0;
}

int NE2000::txihandler()
{
    return 0;
}

void NE2000::Send(Ether* frame)
{
    dword ptx_size=frame->payloadsize;
    byte* ptx_packet=(byte*)frame;
    // 送信が完了しているかどうかチェックする
    while( ( r_reg( NE_P0_COMMAND ) & 0x04 ) !=0 );
    // 送信処理中に受信するとレジスタが狂ってしまう
    disableNetwork();

    ne_pio_writemem( ptx_packet, ( NE_TX_PAGE_START << 8 ) , ptx_size );
    ptx_size+=ETHER_HEADER_SIZE;

    // 最小パケット長より短いかどうかをチェックする
    if( ptx_size < ETHER_MIN_PACKET ){
        ptx_size=ETHER_MIN_PACKET;
    }

    w_reg( NE_P0_COMMAND, NE_CR_PS0 + NE_CR_RD2 + NE_CR_STA );

    // 送信バッファ領域の指定
    w_reg( NE_P0_TPSR, NE_TX_PAGE_START );

    // 送信長の指定
    w_reg( NE_P0_TBCR0, ptx_size & 0xff);
    w_reg( NE_P0_TBCR1, ptx_size >> 8 );

    // 送信命令を発行する
    w_reg( NE_P0_COMMAND, NE_CR_PS0 + NE_CR_TXP + NE_CR_RD2 + NE_CR_STA );

    printf("send.\n");  
    delete frame;
    // 割り込み許可
    enableNetwork();
}

void NE2000::ne_pio_writemem( byte *src, dword dest, dword size )
{
    /* ステータスレジスタクリア */
    w_reg( NE_P0_COMMAND, NE_CR_RD2 + NE_CR_STA );
    w_reg( NE_P0_ISR, NE_ISR_RDC);
    // 長さ
    w_reg( NE_P0_RBCR0, size & 0xff );
    w_reg( NE_P0_RBCR1, size >> 8 );
    // 転送先アドレス
    w_reg( NE_P0_RSAR0, dest & 0xff );
    w_reg( NE_P0_RSAR1, dest >> 8 );
    w_reg( NE_P0_COMMAND, NE_CR_RD1 + NE_CR_STA );
    for(dword i = 0 ; i < size ; i+=2){
        w_regw( NE_ASIC_DATA, *(word*)(src+i) );
    }
    //wait 
    for(dword i=0;i<0xff;i++){
        if( ( r_reg(NE_P0_ISR) & NE_ISR_RDC ) == 0 )
            break;
    }
}

void NE2000::ne_pio_readmem( dword src, byte *dest, dword size )
{
    // abort DMA, start NIC
    w_reg( NE_P0_COMMAND, NE_CR_RD2 + NE_CR_STA );
    // length
    w_reg( NE_P0_RBCR0, size & 0xff );
    w_reg( NE_P0_RBCR1, size >> 8 );
    // source address
    w_reg( NE_P0_RSAR0, src & 0xff );
    w_reg( NE_P0_RSAR1, src >> 8 );
    w_reg( NE_P0_COMMAND, NE_CR_RD0 + NE_CR_STA );
    for(dword i = 0 ; i < size ; i+=2 , dest+=2){
        *(word*)dest=r_regw( NE_ASIC_DATA );
    }
}
