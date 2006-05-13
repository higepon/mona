#ifndef _MONA_MONES_NE2000CONST_
#define _MONA_MONES_NE2000CONST_

namespace mones {

// NE2000 各種定義 from ne2000.h
// イーサネット定数定義

#define     ETHER_HEADER_SIZE   14      /* ヘッダーサイズ */
#define     ETHER_MIN_PACKET    64      /* 最小パケットサイズ */
#define     ETHER_MAX_PACKET    1514    /* 最大パケットサイズ */


// NE2000 定数定義

// QEMU PCI NE2000
#define     NE_ASIC             0x10
#define     NE_ASIC_DATA        NE_ASIC
#define     NE_ASIC_RESET       NE_ASIC+ 0x0f


//Yamami
#define NE_MEM_START        64       /* バッファメモリ開始ページ番号    (アドレス4000h) */
#define NE_MEM_END          128      /* バッファメモリ終了ページ番号  (アドレス8000h) */
#define NE_PAGE_SIZE        256      /* １ページのバイト数 */
#define NE_TX_PAGE_START    0x40     /* =64 送信バッファ開始ページ番号 */
#define NE_RX_PAGE_START    0x46     /* =70 受信バッファ開始ページ番号 */
#define NE_RX_PAGE_STOP     0x80     /* =128 受信バッファ終了ページ番号 */

//オリジナル
//#define     NE_MEM_START        64   /* バッファメモリ開始ページ番号 (アドレス4000h) */
//#define     NE_MEM_END          128  /* バッファメモリ終了ページ番号  (アドレス8000h) */
//#define     NE_PAGE_SIZE        256  /* １ページのバイト数 */
//#define     NE_TX_PAGE_START    64   /* 送信バッファ開始ページ番号 */
//#define     NE_RX_PAGE_START    70   /* 受信バッファ開始ページ番号 */
//#define     NE_RX_PAGE_STOP     128  /* 受信バッファ終了ページ番号 */

//H8より
//#define NE_MEM_START        64       /* バッファメモリ開始ページ番号    (アドレス4000h) */
//#define NE_MEM_END      12           /* バッファメモリ終了ページ番号  (アドレス8000h) */
//#define NE_PAGE_SIZE        256      /* １ページのバイト数 */
//#define NE_TX_PAGE_START    0x40     /* 送信バッファ開始ページ番号 */
//#define NE_RX_PAGE_START    0x46     /* 受信バッファ開始ページ番号 */
//#define NE_RX_PAGE_STOP     0x80     /* 受信バッファ終了ページ番号 */



// NE2000 レジスタアドレス定義

#define     NE_P0_COMMAND       0
#define     NE_P0_PSTART        1
#define     NE_P0_PSTOP         2
#define     NE_P0_BNRY          3   /* Boundary Reg */
#define     NE_P0_TSR           4   /* Transmit Status Reg */
#define     NE_P0_TPSR          4   /* Transmit Page Start Address Reg */
#define     NE_P0_TBCR0         5   /* Transmit Byte Counter Reg */
#define     NE_P0_NCR           5   /* Nubmer of Collisions */
#define     NE_P0_TBCR1         6
#define     NE_P0_ISR           7   /* Interrupt Status Reg */
#define     NE_P0_RSAR0         8   /* Remote Start Address Reg */
#define     NE_P0_RSAR1         9
#define     NE_P0_RBCR0         0xa /* Remote Byte Count Reg */
#define     NE_P0_RBCR1         0xb
#define     NE_P0_RSR           0xc /* Receive Status Reg */
#define     NE_P0_RCR           0xc /* Receive Configuration Reg */
#define     NE_P0_TCR           0xd /* Transmit Configuration Reg */
#define     NE_P0_DCR           0xe /* Data Configuration Reg */
#define     NE_P0_IMR           0xf /* Interrupt Mask Reg */
#define     NE_P0_CNTR0         0xd
#define     NE_P0_CNTR1         0xe
#define     NE_P0_CNTR2         0xf

#define     NE_P1_COMMAND       0
#define     NE_P1_PAR0          1   /* Physical Addres Reg */
#define     NE_P1_PAR1          2
#define     NE_P1_PAR2          3
#define     NE_P1_PAR3          4
#define     NE_P1_PAR4          5
#define     NE_P1_PAR5          6
#define     NE_P1_CURR          7   /* Current Page Reg */
#define     NE_P1_MAR0          8   /* Multicast Address Reg */

// NE2000 レジスタ定数定義
#define     NE_CR_PS0           0           /* Page 0 Select */
#define     NE_CR_PS1           0x40        /* Page 1 Select */
#define     NE_CR_PS2           0x80        /* Page 2 Select */
#define     NE_CR_RD0           8           /* Remote DMA Control */
#define     NE_CR_RD1           0x10
#define     NE_CR_RD2           0x20
#define     NE_CR_TXP           4           /* Transmit packet */
#define     NE_CR_STA           2           /* start */
#define     NE_CR_STP           1           /* stop */

#define     NE_RCR_AB           4           /* accept broadcast */
#define     NE_RCR_APROMIS      0x10        /* PROMISCUOUS PHYSICAL: */
#define     NE_RCR_MON          0x20        /* monitor mode */

#define     NE_DCR_FT1          0x40        /* fifo */
#define     NE_DCR_LS           8           /* Loopback select */
#define     NE_DCR_WTS          1           /* Word Transfer select */

#define     NE_ISR_PRX          1           /* successful receive */
#define     NE_ISR_PTX          2           /* successful trasmit */
#define     NE_ISR_RXE          4           /* receive error */
#define     NE_ISR_TXE          8           /* transmit error */
#define     NE_ISR_OVW          0x10        /* overflow */
#define     NE_ISR_CNT          0x20        /* counter overflow */
#define     NE_ISR_RDC          0x40        /* Remote DMA complete */
#define     NE_ISR_RST          0x80        /* reset */

#define     NE_IMR_PRXE         1           /* successful receive */
#define     NE_IMR_PTXE         2           /* successful trasmit */
#define     NE_IMR_RXEE         4           /* receive error */
#define     NE_IMR_TXEE         8           /* transmit error */
#define     NE_IMR_OVWE         0x10        /* overflow */
#define     NE_IMR_CNTE         0x20        /* counter overflow */
#define     NE_IMR_RDCE         0x40        /* Remote DMA complete */

#define     NE_RSTAT_PRX        1           /* successful receive */
#define     NE_RSTAT_CRC        2           /* CRC error */
#define     NE_RSTAT_FAE        4           /* Frame alignment error */
#define     NE_RSTAT_OVER       8           /* overflow */

#define     NE_TSR_ABT          8           /* Transmit Aborted */

#define     NE_TCR_LB0          2           /* Internal loopback */

#define     ne_cr_proto         NE_CR_RD2
}; // namespace mones
#endif
