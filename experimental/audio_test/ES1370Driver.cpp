#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi.h>
#include <monapi/io.h>
#include <monapi/syscall.h>
#include "ES1370.h"

using namespace MonAPI;

ES1370Driver::ES1370Driver()
{
	this->pci = new Pci;
}

ES1370Driver::~ES1370Driver()
{
	delete this->pci;
}

bool ES1370Driver::existDevice()
{
	pci->CheckPciExist(ES1370_VENDOR_ID, ES1370_DEVICE_ID, &this->pciinfo);
	if( pciinfo.Exist != 0 )
	{
		return false;
	}

	printf("ES1370 is exist.\n");
	printf("DeviceNo = %x\n", pciinfo.DeviceNo);
	printf("Vendor = %x\n", pciinfo.Vendor);
	printf("Device = %x\n", pciinfo.Device);
	printf("BaseAddress = %x\n", pciinfo.BaseAd);
	printf("IrqLine = %x\n", pciinfo.IrqLine);

	baseIO = pciinfo.BaseAd&~1;

	return true;
}

void ES1370Driver::MessageLoop()
{
	MessageInfo msg;
	while(1)
	{
		if( Message::receive(&msg) ) continue;

		switch(msg.header)
		{
		case MSG_INTERRUPTED:
			printf("MSG_INTERRUPTED\n");
			break;
		default:
			break;
		}
	}
}

void ES1370Driver::setIRQ()
{
	syscall_set_irq_receiver(this->pciinfo.IrqLine, MONAPI_FALSE);
}

void ES1370Driver::unsetIRQ()
{
	syscall_remove_irq_receiver(this->pciinfo.IrqLine);
}

void ES1370Driver::readConf()
{
	dword result;
	result = pci->ReadConfig(0, pciinfo.DeviceNo, 0, 0x00, 2);
	printf("Vendor: %x\n", result);
	result = pci->ReadConfig(0, pciinfo.DeviceNo, 0, 0x04, 2);
//	this->pci->WriteConfig(0, pciinfo.DeviceNo, 0, 0x04, 2, result|1);
	result = pci->ReadConfig(0, pciinfo.DeviceNo, 0, 0x04, 2);
	printf("Command: %x\n", result);
	result = pci->ReadConfig(0, pciinfo.DeviceNo, 0, 0x06, 2);
	printf("Status: %x\n", result);
	result = pci->ReadConfig(0, pciinfo.DeviceNo, 0, 0x08, 4);
	printf("Class Code & Revision ID: %x\n", result);
	result = pci->ReadConfig(0, pciinfo.DeviceNo, 0, 0x0D, 1);
	printf("Latency Timer: %x\n", result);

	result = pci->ReadConfig(0, pciinfo.DeviceNo, 0, 0x10, 4);
	printf("BaseAddress: %x\n", result);

}

void ES1370Driver::dumpRegisters()
{
	dword result;

	result = inp32(baseIO+ES1370_REG_CONTROL);
	printf("Control: %x\n", result);

	result = inp32(baseIO+ES1370_REG_STATUS);
	printf("Status: %x\n", result);

	result = inp8(baseIO+ES1370_REG_UART_DATA);
	printf("UART Data: %x\n", result);

	result = inp8(baseIO+ES1370_REG_UART_STATUS);
	printf("UART Status: %x\n", result);

	result = inp32(baseIO+ES1370_REG_MEMPAGE);
	printf("Memory Page: %x\n", result);

	result = inp32(baseIO+ES1370_REG_SERIAL_CONTROL);
	printf("Serial Interface Control: %x\n", result);

	result = inp32(baseIO+ES1370_REG_DAC1_FRAMEADR);
	printf("DAC1 Frame Address: %x\n", result);
}

void ES1370Driver::setMempage(int p)
{
	outp32(baseIO+ES1370_REG_MEMPAGE, (dword)p);
}

void ES1370Driver::enableDAC1Channel()
{
	outp32(baseIO+ES1370_REG_CONTROL,inp32(baseIO+ES1370_REG_CONTROL)|0x40);
}

void ES1370Driver::setSampleRate(int rate)
{
	int bit = 0;
	dword n;
	switch(rate)
	{
		case 5512: bit = 0; break;
		case 11025:bit = 1; break;
		case 22050:bit = 2; break;
		case 44100:bit = 3; break;
		default: break;
	}
	n = inp32(baseIO+ES1370_REG_CONTROL);
	n = n & ~0x3000;
	n |= bit;
	outp32(baseIO+ES1370_REG_CONTROL, n);
}

void ES1370Driver::startDAC1()
{
	dword n = SerialControlRegister();
	n = n & ~0x0800;
	SerialControlRegister(n);
}

void ES1370Driver::playData(void* pm, size_t size)
{
	byte *dmabuf;

	printf("%x\n", *(int*)pm);
	dmabuf = monapi_allocate_dma_memory(size);
	printf("DMA Address = %x\n", dmabuf);

	memcpy(dmabuf, pm, size);

	enableDAC1Channel();
	setMempage(12);
	setSampleRate(44100);
	DAC1FrameRegister(dmabuf, size);
	startDAC1();

	sleep(10);

	monapi_deallocate_dma_memory(dmabuf, size);
}

dword ES1370Driver::readControlRegister()
{
	return inp32(baseIO+ES1370_REG_CONTROL);
}

void ES1370Driver::writeControlRegister(dword n)
{
	outp32(baseIO+ES1370_REG_CONTROL, n);
}

dword ES1370Driver::SerialControlRegister()
{
	return inp32(baseIO+ES1370_REG_SERIAL_CONTROL);
}

void ES1370Driver::SerialControlRegister(dword n)
{
	outp32(baseIO+ES1370_REG_SERIAL_CONTROL, n);
}

void ES1370Driver::DAC1FrameRegister(void *p, size_t size)
{
	outp32(baseIO+0x30, (dword)p);
	outp32(baseIO+0x32, (dword)size);
}
