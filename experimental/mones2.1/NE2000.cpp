
///////////////////////////////
//This file is derived form NE2000.cpp by Mr. Yamami.
///////////////////////////////

#include "NE2000.h"
#include "NE2000const.h"

using namespace mones;

Ether::Frame* NE2000::Recv(int n)
{
	return NULL;
}

void NE2000::Send(Ether::Frame* frame)
{

}

int NE2000::interrupt()
{
	return 0;
}

NE2000::NE2000()
{
	printf("I have NE2000\n");
    ne_ringbuf_status=0;
    ne_ringbuf_bound=0;
    ne_ringbuf_len=0;

    ne_rx_start=0;      /* 受信パケット本体の開始アドレス */
    frame_len=0;        /* 受信パケット本体の長さ */
    ne_rx_bound=0;      /* 受信後の境界レジスタ値 */
    ne_rx_write_p=0;    /* 受信パケット書き込みアドレス */
    ne_rx_sub_len=0;    /* 折り返し分の長さ */
    ne_rx_remain_len=0; /* 残りの長さ(折り返しがないときは本体の長さと同じ) */

    ne_sizeof_test_pattern=20;
}

NE2000::~NE2000()
{

}

int NE2000::init()
{
    int reti = probe();    //Ne2000 存在確認
    if(reti != 0 ){
        printf("Does Not Exist Ne2K!!!\n");
        return -1;
    }

    nic_init();    //Ne2000 初期化
    return 0;
}

void NE2000::inputFrame(void)
{

    byte sts,*buf;
    //バウンダリレジスタ と、カレントページレジスタは8ビット幅
    //データにアクセスする際、8ビットシフトして16ビット幅アクセスを行う
    word  bnd,cpg;

    buf=frame_buf;

    // Page 0
    outp8( NE_P0_COMMAND, NE_CR_STA );
    // sts <- 受信ステータスレジスタ(Receive Status Reg)
    sts=inp8( NE_P0_RSR );

//Yamami デバッグ
//printf("sts : %x\n",sts);

    if( ( sts & NE_RSTAT_OVER ) !=0 ){
        printf("FIFO OverFlow\n");
        return; // 受信FIFOオーバーフローした
    }

    if( ( inp8( NE_P0_ISR ) & NE_ISR_OVW ) !=0 ){
        printf("RING OverFlow\n");
        return; // 受信リングバッファオーバーフロー
    }

    //  受信成功
    if( ( sts & NE_RSTAT_PRX ) ==0 ){
        printf("Not Exist Packet \n");
        return; //  受信パケットなし
    }

    //ページを明示的に切り替えて bnd と cpg を読む
    outp8(NE_P0_COMMAND, NE_CR_PS0 | NE_CR_STA); /* Page 0 */
    bnd=inp8( NE_P0_BNRY ) + 1;      // bnd <-bnd
    //bnd=inp8( NE_P0_BNRY );      // bnd <-bnd ここで+1しない
    outp8(NE_P1_COMMAND, NE_CR_PS1 | NE_CR_STA); /* Page 1 */
    cpg=inp8( NE_P1_CURR );          // cpg <- Current Page

    //Page0に戻しておく
    outp8( NE_P0_COMMAND, NE_CR_PS0 );


//Yamami デバッグ
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
        return;         // = なら バッファ上にパケットなし
    }


    // bound+1 ページの先頭4バイトを読み込む
    // Yamami ↓だと、連続した変数宣言が、連続した4バイトである事に頼っている？？
    // バッファに一旦リードして、代入する処理に変更
    //ne_pio_readmem( bnd << 8, &ne_ringbuf_status, 4 );

    byte bndBuf[4];
    ne_pio_readmem( bnd << 8, bndBuf, 4 );

// Yamamiデバッグ リードアドレスの表示
//printf("Read Src = bnd << 8 : %x\n",bnd << 8);

    ne_ringbuf_status = bndBuf[0]; /* Receive Status */
    ne_ringbuf_bound = bndBuf[1] & 0xFF; /* Next Packet Pointer */
    ne_ringbuf_len = bndBuf[3] * 256 + bndBuf[2];   /* Receive Byte Count */

    //↑これが、それぞれ逆のような気がする。エンディアンの違い？？
    //  この事は、Read/Write側を変更
    //ne_ringbuf_status = bndBuf[1]; /* Receive Status */
    //ne_ringbuf_bound = bndBuf[0] & 0xFF; /* Next Packet Pointer */
    //ne_ringbuf_len = bndBuf[2] * 256 + bndBuf[3];   /* Receive Byte Count */

// Yamamiデバッグ
//printf("ne_ringbuf_status : %x\n",ne_ringbuf_status);
//printf("ne_ringbuf_bound  : %x\n",ne_ringbuf_bound);
//printf("ne_ringbuf_len : %x\n",ne_ringbuf_len);

    ne_rx_start=(bnd << 8) + 4; // パケット本体の開始アドレス

    // CRCの分の長さを引く
    // CRCの分の長さを引く ? CRCじゃなくてne_ringbuf_*の4 byte?
    frame_len=ne_ringbuf_len - 4; /* パケット本体の長さ */

    // 受信終了後の境界レジスタ値
    ne_rx_bound=ne_ringbuf_bound;

    if( ( ne_ringbuf_status & NE_RSTAT_PRX ) !=0 ){
                    // 受信が正常終了した
        if( frame_len >= ETHER_HEADER_SIZE ){
                    // 最短長より短いときはエラー
            if( frame_len < ETHER_MAX_PACKET ) {
                    // 最大長より長いときはエラー

                ne_rx_remain_len=frame_len;

                // パケットの取り込み処理
                // 折り返し分の長さ
                ne_rx_sub_len=NE_RX_PAGE_STOP * 256 - ne_rx_start;

// Yamamiデバッグ
//printf("frame_input 03 ne_rx_start=%x\n",ne_rx_start);
//printf("frame_input 04 ne_rx_sub_len=%x\n",ne_rx_sub_len);

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

    // Yamami バウンダリレジスタ の更新
    bnd=ne_rx_bound;
    if( bnd == NE_RX_PAGE_START ){
        bnd=NE_RX_PAGE_STOP;
    }
    bnd--;
    outp8( NE_P0_BNRY, bnd );    // 境界レジスタ = 次のバッファ - 1

    //  割り込みステータスレジスタクリア
    outp8( NE_P0_ISR, 0xff );

    //H8 より
    outp8(NE_P0_IMR, NE_IMR_PRXE); /* Packet Receive interrupt enable */

}

void NE2000::outputFrame( byte *pkt, byte *mac, dword size, word pid )
{

    dword        ptx_type=0;
    dword        ptx_size=0;
    byte       *ptx_packet=0;
    byte       *ptx_dest=0;


    // 送信が完了しているかどうかチェックする
    while( ( inp8( NE_P0_COMMAND ) & 0x04 ) !=0 );

//Yamami デバッグ
//printf("frame_output 01\n");

    ptx_dest=mac;
    ptx_size=size;
    ptx_packet=pkt;
    // ネットワークバイトオーダーに変換する
    // Yamami 変換不要？
    ptx_type=(pid >> 8)+(pid << 8);

//Yamami デバッグ
//int i;
//for(i=0 ; i<2 ; i++){
//    printf("ptx[1] = %x \n",(byte *)(&ptx_type + 1));
//}


    // 割り込み禁止
    // 送信処理中に受信するとレジスタが狂ってしまう
    //disableInterrupt();
    disableNetwork();

    // 宛先アドレスの書き込み
//    ne_pio_writemem( ptx_dest, NE_TX_PAGE_START << 8, 6 );
    // 送信元アドレスの書き込み
//    ne_pio_writemem( ether_mac_addr, ( NE_TX_PAGE_START << 8 ) + 6, 6 );
    // プロトコルIDの書き込み
//    ne_pio_writemem( (byte *)&ptx_type, ( NE_TX_PAGE_START << 8 ) + 12, 2 );
    // データ部分の書き込み
//    ne_pio_writemem( ptx_packet, ( NE_TX_PAGE_START << 8 ) + 14, ptx_size );
    ne_pio_writemem( ptx_packet, ( NE_TX_PAGE_START << 8 ) , ptx_size );

    ptx_size+=ETHER_HEADER_SIZE;

    // 最小パケット長より短いかどうかをチェックする
    if( ptx_size < ETHER_MIN_PACKET ){
        ptx_size=ETHER_MIN_PACKET;
    }

    outp8( NE_P0_COMMAND, NE_CR_PS0 + NE_CR_RD2 + NE_CR_STA );

    // 送信バッファ領域の指定
    outp8( NE_P0_TPSR, NE_TX_PAGE_START );

    // 送信長の指定
    outp8( NE_P0_TBCR0, ptx_size & 0xff);
    outp8( NE_P0_TBCR1, ptx_size >> 8 );

    // 送信命令を発行する
    outp8( NE_P0_COMMAND, NE_CR_PS0 + NE_CR_TXP + NE_CR_RD2 + NE_CR_STA );

    // 割り込み許可
    //enableInterrupt();
    enableNetwork();

//Yamami デバッグ
//printf("frame_output 02\n");

    // 送信が完了しているかどうかチェックする
    // 2004/11/16 Yamami QEMU on Ne2000 だとこのチェックが永遠に通らないようなのでチェックしない
    //while( ( inp8( NE_P0_COMMAND ) & 0x04 ) !=0 );

//Yamami デバッグ
//printf("frame_output 03\n");

}

int NE2000::probe(void)
{
    int i;

    /* ソフトウェアリセット */
    //outp8( NE_ASIC_RESET, inp8( NE_ASIC_RESET ) );
    /* リセット完了まで待つ */
    //ne_wait_func(0);

    /* DMA を強制停止する。 */
    outp8( NE_P0_COMMAND, NE_CR_RD2 + NE_CR_STP );

    /* 停止する頃まで待つ */
    //ne_wait_func(0);

    //  パケットがメモリに書かれないようにする
    outp8( NE_P0_RCR, NE_RCR_MON );

    // ワード幅転送、ループバックモード
    outp8( NE_P0_DCR, NE_DCR_WTS + NE_DCR_FT1 + NE_DCR_LS );

    // 受信バッファ開始アドレス
    // 64
    outp8( NE_P0_PSTART, NE_MEM_START );

    // 受信バッファ終了アドレス
    // 128
    outp8( NE_P0_PSTOP, NE_MEM_END );

    // メモリテストパターン書き込み
    ne_pio_writemem( (byte *)ne_test_pattern, NE_MEM_START * NE_PAGE_SIZE, ne_sizeof_test_pattern );
    // メモリテストパターン読み込み
    ne_pio_readmem( NE_MEM_START * NE_PAGE_SIZE, ne_test_buffer, ne_sizeof_test_pattern );
    // テストパターンの比較
    if( ne_bcompare( (byte *)ne_test_pattern, ne_test_buffer, ne_sizeof_test_pattern )!=0 )
        return(1);  // 不一致なら終了

    // EEPROM データ読みだし
    ne_pio_readmem( 0, ne_test_buffer, 16 );

    // イーサネットアドレス取得
    for(i=0;i<11;i+=2)
        ether_mac_addr[i/2]=ne_test_buffer[i];

    // 割り込みステータスレジスタクリア
    outp8( NE_P0_ISR, 0xff );

    return(0);
}

void NE2000::nic_init(void)
{
    // 各変数の初期化
    int i;
    byte c;

    //NICリセット
    c = inp8(NE_ASIC_RESET);
    outp8(NE_ASIC_RESET, c);

    //リセット完了まで待つ
    sleep(300);

    // リモートDMA 停止
    outp8( NE_P0_COMMAND, ne_cr_proto | NE_CR_STP );

    // FIFO スレッショルド 8Byte,リモートDMA 自動初期化禁止
    // 8086 バイトオーダ,16bit幅 DMA 転送
    // Page0_0Eh DATA CONFIGURATION REGISTER (DCR) 0EH (WRITE)
    //   7 6   5   4   3  2   1    0
    //   - FT1 FT0 ARM LS LAS BOS WTS
    //   0 1   0   0   1   0   0    1
    outp8( NE_P0_DCR, NE_DCR_FT1 + NE_DCR_WTS + NE_DCR_LS );

    // リモートDMA バイトカウンタクリア
    outp8( NE_P0_RBCR0, 0 );
    outp8( NE_P0_RBCR1, 0 );

    // モニタモード
    // (パケット受信処理をおこなうが、バッファリングはしない)
    // RECEIVE CONFIGURATION REGISTER (RCR) 0CH (WRITE)
    outp8( NE_P0_RCR, NE_RCR_MON );

    // 内部ループバックモード
    // TRANSMIT CONFIGURATION REGISTER (TCR) 0DH (WRITE)
    outp8( NE_P0_TCR, NE_TCR_LB0 );

    // 送信リングバッファ開始アドレスの設定
    // 64
    outp8( NE_P0_TPSR, NE_TX_PAGE_START );
    // 受信リングバッファ開始アドレスの設定
    // 70
    outp8( NE_P0_PSTART, NE_RX_PAGE_START );

    // 受信リングバッファ境界アドレスの設定
    // 70
    outp8( NE_P0_BNRY, NE_RX_PAGE_START );
    // 受信リングバッファ終了アドレスの設定
    // 128
    outp8( NE_P0_PSTOP, NE_RX_PAGE_STOP );

    // 割り込みステータスレジスタのクリア
    outp8( NE_P0_ISR, 0xff );
    //  割り込み許可条件の設定
    // Packet recieve successful
    outp8( NE_P0_IMR, NE_IMR_PRXE );

    //Yamami 全割り込みを許可してみる
    //outp8( NE_P0_IMR, 0x7F );

    // Page 1 の設定
    outp8( NE_P0_COMMAND, ne_cr_proto | ( NE_CR_PS1 + NE_CR_STP ) );

    // Ethernet アドレスの設定
    // ここで指定したアドレスのパケットを受け取る
    for(i=0;i<6;i++){
        outp8( NE_P1_PAR0 + i, ether_mac_addr[i] );
    }

    // 最初に受信したパケットを格納するアドレスの設定
    outp8( NE_P1_CURR, NE_RX_PAGE_START + 1 );

    /* マルチキャストレジスタの設定 */
    outp8( NE_P1_MAR0, 0 );
    outp8( NE_P1_MAR0+1, 0 );
    outp8( NE_P1_MAR0+2, 0 );
    outp8( NE_P1_MAR0+3, 0 );
    outp8( NE_P1_MAR0+4, 0 );
    outp8( NE_P1_MAR0+5, 0 );
    outp8( NE_P1_MAR0+6, 0 );
    outp8( NE_P1_MAR0+7, 0 );

    // Page 0 にもどす
    outp8( NE_P0_COMMAND, ne_cr_proto | NE_CR_STP );

    // 受信パケットフィルタの設定
    // ブロードキャストと自分宛のみをメモリに格納
    // accept broadcast
    outp8( NE_P0_RCR, NE_RCR_AB );
// ↑ をコメントアウトすると
// Yamamiモニタモードのままとしてみる!!!!! 事になる

    //Yamami プロミスキャストのみ受け取るようにしてみる???? 違うようだ
    //outp8( NE_P0_RCR, NE_RCR_APROMIS );

    // NIC をアクティブにする
    outp8( NE_P0_COMMAND, ne_cr_proto | NE_CR_STA );

    // ループバックモードを抜けて通常動作モードに入る
    outp8( NE_P0_TCR, 0 );
}

void NE2000::ne_pio_writemem( byte *src, dword dest, dword size )
{
    dword i;

    word writetmp;

    /* ステータスレジスタクリア */
    outp8( NE_P0_COMMAND, NE_CR_RD2 + NE_CR_STA );
    outp8( NE_P0_ISR, NE_ISR_RDC);

    /* 長さ */
    outp8( NE_P0_RBCR0, size & 0xff );
    outp8( NE_P0_RBCR1, size >> 8 );

    /* 転送先アドレス */
    outp8( NE_P0_RSAR0, dest & 0xff );
    outp8( NE_P0_RSAR1, dest >> 8 );
    outp8( NE_P0_COMMAND, NE_CR_RD1 + NE_CR_STA );

//    for(i=0;i<size;i+=2){
//        outp8( PIO_ADATA, *(src+1) );
//        outp8( NE_ASIC_DATA, *src );
//        src+=2;
//    }


    // 2004/08/02 DATAは16ビット幅でやりとりするので、Word変換してI/O
    for(i = 0 ; i < size ; i+=2 , src+=2){
        //writetmp = (word)(*(src) << 8) + (word)*(src+1);
        //リトルエンディアンならこう？？
        writetmp = (word)(*(src + 1) << 8) + (word)*(src);
        outp16( NE_ASIC_DATA, writetmp );
    }

    /* wait */
    for(i=0;i<0xff;i++){
        if( ( inp8(NE_P0_ISR) & NE_ISR_RDC ) == 0 )
            break;
    }

}


void NE2000::ne_pio_readmem( dword src, byte *dest, dword size )
{
    dword i;

    word readtmp;

//Yamami デバッグ
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

    // 2004/08/02 DATAは16ビット幅でやりとりするので、Word変換してI/O
    for(i = 0 ; i < size ; i+=2 , dest+=2){
        readtmp=inp16( NE_ASIC_DATA );
        //*dest=(byte)(readtmp >> 8);
        //*(dest+1)=(byte)(readtmp & 0xff);
        //リトルエンディアンならこう？？
        *(dest+1)=(byte)(readtmp >> 8);
        *(dest)=(byte)(readtmp & 0xff);
    }

}

int NE2000::ne_bcompare( byte *src, byte *dest, dword size )
{
    for(dword i=0;i<size;i++){
        if( src[i]!=dest[i] )
            return(1);
    }
    return(0);
}

void NE2000::getFrameBuffer(byte* buffer, dword size)
{
    dword maxSize = getFrameBufferSize();
    size = size < maxSize ? size : maxSize;
    memcpy(buffer, this->frame_buf, size);
}

