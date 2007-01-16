/*!
    \file  Pci.h
    \brief PCIライブラリ ヘッダ

    class Pci

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami 
    \version $Revision$
    \date   create:2004/10/15 update:$Date$
	$Id$
*/

#ifndef _LIB_PCI_INF
#define _LIB_PCI_INF


#include <sys/types.h>
#include <monapi/CString.h>

using namespace MonAPI;

/*! 
 *  \struct PciPacket
 *  \brief PciPacket構造体 共用体
 *   //PCI CONFIG_ADDRESSレジスタの値設定
 *   //bit0-1：0に固定
 *   //bit2-7：レジスタアドレス
 *   //bit8-10：機能番号
 *   //bit11-15：デバイス番号
 *   //bit16-23：バス番号
 *   //bit24-30：リザーブで、0に固定
 *   //bit31：イネーブルビットで、1に固定
 *
 *   //レジスタ ビット位置 31,30・・・・0(32ビット)
 */
typedef union
{
   uint32_t command;
   struct
   {
       unsigned reserved1 : 2;
       unsigned reg       : 6;
       unsigned function  : 3;
       unsigned device    : 5;
       unsigned bus       : 8;
       unsigned reserved2 : 7;
       unsigned enabled   : 1;
   } p;
} PciPacket;


/*! 
 *  \enum PciPacket
 *  \brief PCIデバイス各種宣言
 */
enum
{
   REG_CONFIG_ADDRESS = 0x0CF8,
   REG_CONFIG_DATA    = 0x0CFC,

   PCI_VENDOR_ID      = 0x00,
   PCI_DEVICE_ID      = 0x02,
   PCI_COMMAND        = 0x04,
   PCI_STATUS         = 0x06,
   PCI_REVISION       = 0x08,
   PCI_API            = 0x09,
   PCI_SUBCLASS       = 0x0a,
   PCI_BASECLASS      = 0x0b,
   PCI_HEADER         = 0x0e,
   PCI_BASE_ADDRESS1  = 0x10,
   PCI_BASE_ADDRESS2  = 0x14,
   PCI_BASE_ADDRESS3  = 0x18,
   PCI_BASE_ADDRESS4  = 0x1C,
   PCI_BASE_ADDRESS5  = 0x20,
   PCI_IRQ_LINE       = 0x3C,
   PCI_IRQ_PIN        = 0x3D
};


/*! 
 *  \struct PciInf
 *  \brief PciInf PCIデバイス情報構造体
 */
typedef struct
{
    int  Exist;       //存在確認情報(存在する=0  , 存在しない=0以外)
    uint8_t DeviceNo;
    uint16_t Vendor;
    uint16_t Device;
    CString VendorName;
    CString DeviceName;
    uint32_t BaseAd;
    uint32_t  IrqLine;
} PciInf;



//改行コード
#define CHR_CRLF 0x0D0A


//エイリアス宣言
#define uchar unsigned char
#define int32 long
#define uint32 unsigned long
#define float32 float
#define int16 short
#define uint16 unsigned short




/*!
    Pci class
*/
class Pci
{
    
  public:
    

    //コンストラクタ
    Pci();
    ~Pci();

    void CheckPciExist(uint16_t , uint16_t ,PciInf*);
    uint32_t ReadConfig(uint8_t , uint8_t , uint8_t , uint8_t , uint8_t );
    void WriteConfig(uint8_t ,uint8_t ,uint8_t ,uint8_t ,uint8_t ,uint32_t );
  private:
    
    MonAPI::CString getPciInfName( uint8_t* , uint32_t , MonAPI::CString* , MonAPI::CString* );
    bool IsLineSeparator(char );
};




#endif
