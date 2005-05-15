/*!
    \file  mypciinf.cpp
    \brief Get PCI Information

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X License

    \author  Yamami
    \version $Revision$
    \date   create:2004/05/09 update:$Date$
*/

#include <monapi.h>
#include <monapi/messages.h>
#include <monapi/io.h>
#include <monapi/CString.h>
#include "pciinfo.h"

using namespace MonAPI;

int MonaMain(List<char*>* pekoe) {

    int reti;
    dword c_deviceid;
    dword InValue;

    word Vendor;
    word Device;

    syscall_get_io();

    CString bundlePath = MonAPI::System::getBundlePath();
    //pciinfoファイルをオープン
    monapi_cmemoryinfo* pciinfData = monapi_call_file_decompress_bz2_file(bundlePath + "/" + PCIINFO_FILE, MONAPI_TRUE);
    //エラー処理
    if(pciinfData == NULL){
        printf("PCI DATA FILE OPEN ERROR !!!\n");
    }

    printf("PCI Information !!!\n");

    //バス番号0について、デバイス番号を0～31のそれぞれについて、ベンダーIDを読み出す
    for(c_deviceid = 0; c_deviceid < 32 ; c_deviceid++ ){
        reti = lookup_pci_hw( c_deviceid , 0, 0 ,0, &InValue);
        if (InValue != 0xFFFFFFFF){
            //ベンダーIDの取得
            Vendor = InValue & 0x0000FFFF;
            Device = InValue >> 16;
            //ベンダー名称/デバイス名称の取得
            CString VendorName;
            CString DeviceName;

            CString Dummy = getPciInfName(pciinfData->Data , InValue , &VendorName , &DeviceName);
            printf("DeviceNo=%d , Vendor=%x VendorName=%s Device=%x DeviceName=%s \n" ,c_deviceid , Vendor , (const char*)VendorName , Device , (const char*)DeviceName );
        }
    }


    monapi_cmemoryinfo_dispose(pciinfData);
    monapi_cmemoryinfo_delete(pciinfData);


    return 0;
}



/*!
    \brief lookup_pci_hw
        PCIデバイス探し
    \param  dword deviceid [in] デバイス番号
    \param  dword func [in] ファンクション番号
    \param  dword bus [in] バス番号
    \param  dword regaddr [in] レジスタアドレス
    \param  dword * InValue [OUT] 取得レジスタの値
    \return int 結果

    \author Yamami
    \date   create:2004/05/15 update:2004/05/15
*/

int lookup_pci_hw( dword deviceid , dword func , dword bus , dword regaddr , dword *InValue)
{

    dword   outvalue;
    dword   outport;
    dword   inport;
    dword   tmp;

    //printf("lookup_pci_hw !!!\n");

    tmp = 0;
    //PCI CONFIG_ADDRESSレジスタの値設定
    //bit0-1：0に固定
    //bit2-7：レジスタアドレス

    //bit8-10：機能番号
    //bit11-15：デバイス番号

    //bit16-23：バス番号
    //bit24-30：リザーブで、0に固定
    //bit31：イネーブルビットで、1に固定
    //レジスタ ビット位置 31,30・・・・0(32ビット)

    //;イネーブルビットと、バス番号をセットする。
    outvalue = 0x8000;
    outvalue = outvalue | bus;
    outvalue = outvalue << 16;

    //デバイス番号、機能番号設定
    tmp = deviceid;
    tmp = tmp << 3;
    tmp = tmp + func;
    tmp = tmp << 8;

    outvalue = outvalue + tmp;

    tmp = 0;
    //レジスタアドレスの指定
    tmp = regaddr;
    tmp = tmp << 2;

    outvalue = outvalue + tmp;


    //I/Oポートの指定 (0x0cf8、32bit、Read/Write可)
    outport = PCI_CONFIG_ADDRESS_ADDR;

    //ここで、デバッグプリントをしてみる。
    //printf("Value= %x \n" , outvalue);
    //printf("Port= %x \n" , outport);

    outp32(outport,outvalue);


    //CONFIG_DATAレジスタ（0x0cfc～0x0cff、任意のサイズ、Read/Write可）
    inport = PCI_CONFIG_DATA_ADDR;
    *InValue = inp32(inport);

    //イネーブルビットのクリア
    outp32(outport,0);

    return 0;
}




/*!
    \brief IsLineSeparator
        セパレータ判定

    \param  char ch [IN] キャラクタ
    \return bool セパレータ文字ならtrue 以外ならfalse
    \author Tino氏
    \date   create:2004/06/111 update:2004/06/11
*/

inline bool IsLineSeparator(char ch)
{
    return ch == '\r' || ch == '\n' || ch == '\0';
}


/*!
    \brief getPciInfName
        PCI情報名称(ベンダー、デバイス)情報取得
        PCIレジスタ値 を引数にとり、PCIINF.TXT から該当するベンダー名称ベンダ名称を取得する。。
    \param  byte* PciInfData [IN] PCI情報データ
    \param  dword InValue [IN] PCIレジスタ値(ベンダーCD & デバイスCD)
    \param  CString& VendorName [OUT] ベンダー名称
    \param  CString& DeviceName [OUT] デバイス名称

    \return MonAPI::CString dummy(ダミー戻り値)

    \author Yamami
    \date   create:2004/05/16 update:2004/06/11
*/

CString getPciInfName( byte* PciInfData, dword InValue , CString* VendorName , CString* DeviceName){

    word Vendor;
    word Device;

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
