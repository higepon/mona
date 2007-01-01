#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi.h>
#include <monapi/io.h>
#include <monapi/syscall.h>
#include "ES1370.h"

using namespace MonAPI;

typedef struct _frameregister2
{
	unsigned short buffersize;
	unsigned short currentcount;
} FrameRegister2;

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
		printf("msg.header = %x\n", msg.header);
		printf("msg.arg1 = %x\n", msg.arg1);

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
	syscall_set_irq_receiver(this->pciinfo.IrqLine, false);
//	syscall_set_irq_receiver(0, false);
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

	result = inp32(baseIO+ES1370_REG_DAC1_FRAMECNT);
	printf("DAC1 Frame Count & Size: %x\n", result);
}

void ES1370Driver::setMempage(int p)
{
	outp32(baseIO+ES1370_REG_MEMPAGE, (dword)p);
}

void ES1370Driver::enableDAC1Channel()
{
	outp32(baseIO+ES1370_REG_CONTROL,inp32(baseIO+ES1370_REG_CONTROL)|ES1370_DAC1_EN);
}

void ES1370Driver::disableDAC1Channel()
{
	outp32(baseIO+ES1370_REG_CONTROL,inp32(baseIO+ES1370_REG_CONTROL)&~ES1370_DAC1_EN);
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
	SerialControlRegister(SerialControlRegister()&~ES1370_P1_PAUSE);
}

void ES1370Driver::stopDAC1()
{
	SerialControlRegister(SerialControlRegister()|ES1370_P1_PAUSE);
}

void ES1370Driver::playData(void* pm, size_t size)
{
	byte *dmabuf;

	printf("%x\n", pm);
	dmabuf = monapi_allocate_dma_memory(size);
	printf("DMA Address = %x\n", dmabuf);

	memcpy(dmabuf, pm, size);

	disableDAC1Channel();
	writeControlRegister(readControlRegister()&~1);
	setMempage(ES1370_PAGE_DAC&0x0f);
	setSampleRate(44100);
//	setMonoMode(DAC1, 8);
	setStereoMode(DAC1, 8);
	setLoopMode(DAC1);
//	setStopMode(DAC1);
	printf("size = %x\n", size);
	DAC1FrameRegister(dmabuf, size);
	SerialControlRegister(SerialControlRegister()&~(ES1370_P1_LOOP_SEL|ES1370_P1_PAUSE|ES1370_P1_SCT_RLD|ES1370_P1_S_EB|ES1370_P1_S_MB));
//	SCT_RLD();
	startDAC1();
	enableInterrupt(DAC1);
	enableDAC1Channel();
	printf("playing...\n");
	MessageLoop();

	sleep(1000);

	stopDAC1();
	dumpRegisters();

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
	int n;
	FrameRegister2 fr;
	fr.currentcount = 0;
	fr.buffersize = (unsigned short)size;
	memcpy(&n, &fr, sizeof(int));
	outp32(baseIO+ES1370_REG_DAC1_FRAMEADR, (dword)p);
	outp32(baseIO+ES1370_REG_DAC1_FRAMECNT, (dword)n);
}

void ES1370Driver::setStereoMode(Channel ch, int bits)
{
	if( bits == 16 )
	{
		switch(ch)
		{
		case DAC1:
			SerialControlRegister(SerialControlRegister()|ES1370_P1_S_EB|ES1370_P1_S_MB);
			break;
		case DAC2:
			SerialControlRegister(SerialControlRegister()|ES1370_P2_S_EB|ES1370_P2_S_MB);
			break;
		case ADC:
			break;
		}
	}
	else if( bits == 8 )
	{
		switch(ch)
		{
		case DAC1:
			SerialControlRegister(SerialControlRegister()|ES1370_P1_S_MB);
			break;
		case DAC2:
			SerialControlRegister(SerialControlRegister()|ES1370_P2_S_MB);
			break;
		case ADC:
			break;
		}
	}

}

void ES1370Driver::setMonoMode(Channel ch, int bits)
{
	if( bits == 16 )
	{
		switch(ch)
		{
		case DAC1:
			SerialControlRegister(SerialControlRegister()|ES1370_P1_S_EB);
			break;
		case DAC2:
			SerialControlRegister(SerialControlRegister()|ES1370_P2_S_EB);
			break;
		case ADC:
			break;
		}
	}
	else if( bits == 8 )
	{
		switch(ch)
		{
		case DAC1:
			SerialControlRegister(SerialControlRegister()&~(ES1370_P1_S_MB|ES1370_P1_S_EB));
			break;
		case DAC2:
			SerialControlRegister(SerialControlRegister()&~(ES1370_P2_S_MB|ES1370_P2_S_EB));
			break;
		case ADC:
			break;
		}
	}

}


void ES1370Driver::setLoopMode(Channel ch)
{
	switch(ch)
	{
	case DAC1:
		SerialControlRegister(SerialControlRegister()&~ES1370_P1_LOOP_SEL);
		break;
	}
}

void ES1370Driver::setStopMode(Channel ch)
{
	switch(ch)
	{
	case DAC1:
		SerialControlRegister(SerialControlRegister()|ES1370_P1_LOOP_SEL);
		break;
	}
}

void ES1370Driver::enableInterrupt(Channel ch)
{
	switch(ch)
	{
	case DAC1:
	SerialControlRegister(SerialControlRegister()|ES1370_P1_INTR_EN);
		break;
	case DAC2:
	SerialControlRegister(SerialControlRegister()|ES1370_P2_INTR_EN);
		break;
	}
}

void ES1370Driver::SCT_RLD()
{
	SerialControlRegister(SerialControlRegister()|ES1370_P1_SCT_RLD);
}
