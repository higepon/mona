/*!
    \file  mypciinf.h
    \brief Get PCI Information ヘッダファイル

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami
    \version $Revision$
    \date   create:2004/05/09 update:$Date$
*/
//2004/09/03 Yamami CVS 更新テスト

#ifndef _MY_PCI_INF
#define _MY_PCI_INF


#include <sys/types.h>
#include <monapi/CString.h>


//定数宣言
//I/Oポート CONFIG_ADDRESS
#define PCI_CONFIG_ADDRESS_ADDR 0x0CF8
//I/Oポート CONFIG_DATA
#define PCI_CONFIG_DATA_ADDR 0x0CFC

//PCIINFOファイル
#define PCIINFO_FILE "PCIINF.BZ2"

//改行コード
#define CHR_CRLF 0x0D0A


//エイリアス宣言
#define uchar unsigned char
#define int32 long
#define uint32 unsigned long
#define float32 float
#define int16 short
#define uint16 unsigned short


//関数宣言
int lookup_pci_hw( dword , dword , dword , dword , dword*);
MonAPI::CString getPciInfName( byte* , dword , MonAPI::CString* , MonAPI::CString* );

#endif
