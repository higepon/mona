
/*!
    \file   Pci.cpp
    \brief  PCIライブラリ PCIクラス

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/10/15 update:$Date$
	$Id$
*/

/*! \class Pci
 *  \brief PCIライブラリ PCIクラス
 */


#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include <monapi/CString.h>
#include <monalibc/stdio.h>

#include <pci/Pci.h>

extern "C" int dllmain();
int dllmain()
{
}


//PCIINFOファイル
#define PCIINFO_FILE "libPci.a"

using namespace MonAPI;




/*!
    \brief initialize
         Pci コンストラクタ
    \author Yamami
    \date   create:2004/10/15 update:$Date$
*/
Pci::Pci()
{
    //I/O取得
    syscall_get_io();

}


/*!
    \brief initialize
         Pci デスクトラクタ
    \author Yamami
    \date   create:2004/10/15 update:$Date$
*/
Pci::~Pci() 
{
    //ここで、I/Oを解除したいが・・・

}




/*!
    \brief CheckPciExist
         Pciデバイス存在確認。指定したVendor/DeviceのPCIデバイスの存在を確認する。
    \author Yamami
    \param  uint16_t Vendor [in] ベンダーID
    \param  uint16_t Device [in] デバイスID
    \return PciInf構造体へのポインタ
    \date   create:2004/10/15 update:$Date$
*/
void Pci::CheckPciExist(uint16_t ChkVendor , uint16_t ChkDevice ,PciInf* RetPciInf) 
{
    
    uint8_t DeviceNo;
    uint32_t Vendor_Dev;

    uint16_t Vendor;
    uint16_t Device;

    uint32_t BaseAd;
    uint32_t  IrqLine;

    RetPciInf->isExist = false;

    //Yamami!!! 2004/10/18 PCI情報ファイルは、バンドルでは無く、独立させる方向で
//     CString bundlePath = MonAPI::System::getBundlePath();
//     //pciinfoファイルをオープン
//     monapi_cmemoryinfo* pciinfData = monapi_call_file_decompress_bz2_file(bundlePath + "/" + PCIINFO_FILE, MONAPI_TRUE);
//     //エラー処理
//     if(pciinfData == NULL){
//         printf("PCI DATA FILE OPEN ERROR !!!\n");
//     }

    //バス番号0について、デバイス番号を0〜31のそれぞれについて、ベンダーIDを読み出す
    for(DeviceNo = 0; DeviceNo < 32 ; DeviceNo++ ){
        //ReadConfig を用いてVendorの取得
        Vendor = ReadConfig(0, DeviceNo, 0, PCI_VENDOR_ID, 2);
        
        if (Vendor != 0xFFFF){
            Device = ReadConfig(0, DeviceNo, 0, PCI_DEVICE_ID, 2);
            if (Device != 0xFFFF && ChkVendor == Vendor && ChkDevice == Device){
                //デバイスが存在する。
                //デバイスとベンダーを結合 getPciInfName にインターフェースをあわせる。
                Vendor_Dev = Vendor + (uint32_t)(Device << 16);
                
                //Baseアドレスの取得
                BaseAd = ReadConfig(0, DeviceNo, 0, PCI_BASE_ADDRESS1, 4);
                //IRQラインの取得  google PCI IRQ取得で検索
                IrqLine = ReadConfig(0, DeviceNo, 0, PCI_IRQ_LINE, 1);
                //ベンダー名称/デバイス名称の取得
                CString VendorName;
                CString DeviceName;

                //            CString Dummy = getPciInfName(pciinfData->Data , Vendor_Dev , &VendorName , &DeviceName);
                
                //返却値生成
                RetPciInf->isExist = true;
                RetPciInf->deviceNo = DeviceNo;
                RetPciInf->vendor = Vendor;
                RetPciInf->device = Device;
                RetPciInf->vendorName = VendorName;
                RetPciInf->deviceName = DeviceName;
                RetPciInf->baseAdress = BaseAd;
                RetPciInf->irqLine = IrqLine;
                
                //見つかった場合は、即ループを抜ける
                break;
            }
        }
    }

    //ファイル後処理
    //monapi_cmemoryinfo_dispose(pciinfData);
    //monapi_cmemoryinfo_delete(pciinfData);

    //Return
    //return &RetPciInf;
    
}

void Pci::WriteConfig(uint8_t bus,uint8_t device,uint8_t function,uint8_t reg,uint8_t readSize,uint32_t val)
{
   PciPacket packet;
   packet.p.enabled   = 1;
   packet.p.bus       = bus;
   packet.p.device    = device;
   packet.p.function  = function;
   packet.p.reg       = (reg & 0xFC)>>2;
   packet.p.reserved1 = 0;
   packet.p.reserved2 = 0;

   outp32(REG_CONFIG_ADDRESS,packet.command);
   for(int i =0;i<readSize;i++){
	  outp8(REG_CONFIG_DATA+ ((reg+i) & 0x0003), (uint8_t)((val>>(i*8)) & 0xFF ));
   }
   packet.p.enabled=0;
   outp32(REG_CONFIG_ADDRESS,packet.command);
   return ;
}


/*!
    \brief ReadConfig
        PCIデバイス情報取得
    \param  uint8_t bus [in] バス番号
    \param  uint8_t deviceid [in] デバイス番号
    \param  uint8_t func [in] ファンクション番号
    \param  uint8_t reg [in] レジスタアドレス
    \param  uint8_t readSize [I] 取得サイズ
    \return uint32_t 取得レジスタの値

    \author Yamami
    \date   create:2004/05/15 update:2004/05/15
*/
uint32_t Pci::ReadConfig(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg, uint8_t readSize)
{
   uint32_t result;
   PciPacket packet;

   packet.p.enabled   = 1;
   packet.p.bus       = bus;
   packet.p.device    = device;
   packet.p.function  = function;
   //packet.p.reg       = reg & ~3;
   packet.p.reg       = (reg & ~3) >> 2;
   packet.p.reserved1 = 0;
   packet.p.reserved2 = 0;

   /* set request and enable */
   outp32(REG_CONFIG_ADDRESS, packet.command);

   switch (readSize)
   {
   case 1:
       result = inp8(REG_CONFIG_DATA + (reg & 3));
       break;

   case 2:
       result = inp16(REG_CONFIG_DATA + (reg & 3));
       break;

   case 4:
       result = inp32(REG_CONFIG_DATA);
       break;

   default:
       result = 0xFFFFFFFF;
       break;
   }

   packet.p.enabled = 0;
   outp32(REG_CONFIG_ADDRESS, packet.command);
   return result;
}



/*!
    \brief IsLineSeparator
        セパレータ判定

    \param  char ch [IN] キャラクタ
    \return bool セパレータ文字ならtrue 以外ならfalse
    \author Tino氏
    \date   create:2004/06/111 update:2004/06/11
*/
inline bool Pci::IsLineSeparator(char ch)
{
    return ch == '\r' || ch == '\n' || ch == '\0';
}


/*!
    \brief getPciInfName
        PCI情報名称(ベンダー、デバイス)情報取得
        PCIレジスタ値 を引数にとり、PCIINF.TXT から該当するベンダー名称ベンダ名称を取得する。。
    \param  uint8_t* PciInfData [IN] PCI情報データ
    \param  uint32_t InValue [IN] PCIレジスタ値(ベンダーCD & デバイスCD)
    \param  CString& VendorName [OUT] ベンダー名称
    \param  CString& DeviceName [OUT] デバイス名称

    \return MonAPI::CString dummy(ダミー戻り値)

    \author Yamami
    \date   create:2004/05/16 update:2004/06/11
*/
CString Pci::getPciInfName( uint8_t* PciInfData, uint32_t InValue , CString* VendorName , CString* DeviceName){

    uint16_t Vendor;
    uint16_t Device;

    //ベンダーIDの取得
    Vendor = InValue & 0x0000FFFF;
    Device = InValue >> 16;

    char VendorHex[5];    //ベンダーCD格納用
    char DeviceHex[5];    //ベンダーCD格納用
    sprintf(VendorHex, "%04X", Vendor);
    sprintf(DeviceHex, "%04X", Device);

    //NULLまで繰り返し
    while (*PciInfData != 0){

        //printf("%c",*PciInfData);

        //まず、ベンダー名称を探す
        //ラインセパレータなら次の文字へ
        if (IsLineSeparator(*PciInfData)){
            PciInfData++;
        }
        else if (strncmp((const char*)PciInfData, VendorHex, 4) == 0){
            const char* ps = (const char*)(PciInfData + 5), * pe = ps;
            for (; !IsLineSeparator(*pe); pe++);

            //return CString(ps, pe - ps);
            //ベンダー情報確定
            *VendorName = CString(ps, pe - ps);

            //printf("%s\n",(const char*)VendorName);

            //ベンダー名以下のデバイスを検索する。
            //1行読み飛ばし
            for (; !IsLineSeparator(*PciInfData); PciInfData++);
            while (*PciInfData != 0){
                if (IsLineSeparator(*PciInfData)){
                    PciInfData++;

                    //ラインセパレータの次がタブなら、デバイス情報
                    if (strncmp((const char*)PciInfData, "\t", 1) == 0){
                        PciInfData++;
                        if (strncmp((const char*)PciInfData, DeviceHex, 4) == 0){
                            const char* ps = (const char*)(PciInfData + 5), * pe = ps;
                            for (; !IsLineSeparator(*pe); pe++);
                            *DeviceName = CString(ps, pe - ps);

                            //printf("%s\n",(const char*)DeviceName);

                            return "dumy";
                        }
                        else{
                            //次のデバイスへ
                            for (; !IsLineSeparator(*PciInfData); PciInfData++);
                        }
                    }
                    //ベンダーが変更されるまで読んで見つからなかったら、不明とする。
                    else{
                        *DeviceName = "???";
                        //printf("%s\n",(const char*)DeviceName);
                        return "dumy";
                    }
                }
                else{
                    printf("tobashi\n");

                    //どれにも該当しない行は読み飛ばし
                    for (; !IsLineSeparator(*PciInfData); PciInfData++);
                }

            }
        }
        else{
            //どれにも該当しない行は読み飛ばし
            for (; !IsLineSeparator(*PciInfData); PciInfData++);
        }
    }

    // 不明
    *VendorName = "???";
    *DeviceName = "???";
    return "???";
}
