#include <pci/Pci.h>
#include "NicFactory.h"
#include "NE2000.h"
#include "MonAMDpcn.h"

// based on MonesLoader by Yamami

using namespace mones;

Nic* NicFactory::create()
{
    PciInf pciinfo;
    Pci* pcilib = new Pci();
    Nic* nic;

	pcilib->CheckPciExist(MonAMDpcn::VENDORID,MonAMDpcn::DEVICEID,&pciinfo);
	if( pciinfo.Exist== 0){
		// 0x04 is PCI_COMMAND
		dword val=pcilib->ReadConfig(0, pciinfo.DeviceNo, 0, 0x04,2);
		pcilib->WriteConfig(0,pciinfo.DeviceNo,0,0x04,2,val|0x4);

		nic=new MonAMDpcn();
		nic->setIRQ(pciinfo.IrqLine);
		nic->setIOBase(pciinfo.BaseAd);
		nic->init();
		delete pcilib;
		return nic;
	}

    //TODO 一時しのぎなので、マジックナンバーを用いる
    //QEMU上の、NE2000を探す
    pcilib->CheckPciExist(0x10EC,0x8029,&pciinfo);

    if (pciinfo.Exist == 0)
    {
        //見つかれば
        //NE2000のロード
        nic = new NE2000();

        //QEMU設定
        nic->setIRQ(9);
        nic->setIOBase(0xC100);
    }
    else{
        //見つからなければ
        //NE2000のロード
        nic = new NE2000();

        //Bochs設定
        nic->setIRQ(3);
        nic->setIOBase(0x240);
    }

    //NIC初期化
    if (nic->init()) {
	return NULL;
    }

    return nic;
}
