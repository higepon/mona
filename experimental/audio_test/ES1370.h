#ifndef _ES1370_H_
#define _ES1370_H_

#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi.h>
#include "ES1370Register.h"

enum
{
	ES1370_VENDOR_ID	= 0x1274,
	ES1370_DEVICE_ID	= 0x5000,

	ES1370_REG_CONTROL	= 0x00,
	ES1370_REG_STATUS	= 0x04,
	ES1370_REG_UART_DATA	= 0x08,
	ES1370_REG_UART_STATUS	= 0x09,
	ES1370_REG_UART_CONTROL	= 0x09,
	ES1370_REG_UART_TEST	= 0x0a,
	ES1370_REG_MEMPAGE	= 0x0c,
	ES1370_REG_CODEC	= 0x10,
	ES1370_REG_SERIAL_CONTROL = 0x20,
	ES1370_REG_DAC1_SCOUNT	= 0x24,
	ES1370_REG_DAC2_SCOUNT	= 0x28,
	ES1370_REG_ADC_SCOUNT	= 0x2c,
	ES1370_REG_DAC1_FRAMEADR = 0xc30,
	ES1370_REG_DAC1_FRAMECNT = 0xc34,
	ES1370_REG_DAC2_FRAMEADR = 0xc38,
	ES1370_REG_DAC2_FRAMECNT = 0xc3c,
	ES1370_REG_ADC_FRAMEADR = 0xd30,
	ES1370_REG_ADC_FRAMECNT = 0xd34,
	ES1370_REG_PHANTOM_FRAMEADR = 0xd38,
	ES1370_REG_PHANTOM_FRAMECNT = 0xd3c,

	ES1370_SERR_DISABLE = (1<<0),
	ES1370_CDC_EN = (1<<1),
	ES1370_JYSTK_EN = (1<<2),
	ES1370_UART_EN = (1<<3),
	ES1370_ADC_EN = (1<<4),
	ES1370_DAC2_EN = (1<<5),
	ES1370_DAC1_EN = (1<<6),
	ES1370_BREQ = (1<<7),
	ES1370_XCTL0 = (1<<8),
	ES1370_M_CB = (1<<9),
	ES1370_CCB_INTRM = (1<<10),
	ES1370_DAC_SYNC = (1<<11),
	ES1370_WTSRSEL = (1<<12),
	ES1370_M_SBB = (1<<14),
	ES1370_MSFMTSEL = (1<<15),
	ES1370_PCLKDIV = (1<<16),
	ES1370_OPEN = (1<<29),
	ES1370_XCTL1 = (1<<30),
	ES1370_ADC_STOP = (1<<31),
};

class ES1370
{
private:
	dword IOport;
	PciInf pciInfo;

	void BitSetter(void (ES1370Register::*setter)(dword),
			dword (ES1370Register::*getter)(), bool flag, dword mask);
	bool BitGetter(dword (ES1370Register::*getter)(), dword mask);

	inline dword SelectControlRegister();
	inline void SelectControlRegister(dword n);
	inline dword SerialControlRegister();
	inline void SerialControlRegister(dword n);
public:
	ES1370(PciInf info);
	virtual ~ES1370();
	void ADCStop(bool stop);
	bool ADCStop();
	void XCTL1(bool);
	bool XCTL1();
	void PCLKDIV(dword);
	dword PCLKDIV();
	void MSFMTSEL(bool);
	bool MSFMTSEL();
	void M_SBB(bool);
	bool M_SBB();
	void DAC_SYNC(bool);
	bool DAC_SYNC();
	void CCB_INTRM(bool);
	bool CCB_INTRM();
	void M_CB(bool);
	bool M_CB();
	void XCTL0(bool);
	bool XCTL0();
	void BREQ(bool);
	bool BREQ();
	void DAC1Enable(bool);
	bool DAC1Enable();
	void DAC2Enable(bool);
	bool DAC2Enable();
	void ADCEnable(bool);
	bool ADCEnable();
	void UARTEnable(bool);
	bool UARTEnable();
	void JYSTKEnable(bool);
	bool JYSTKEnable();
	void CDCEnable(bool);
	bool CDCEnable();
	void SERRDisable(bool);
	bool SERRDisable();
	void setSampleRate(int);
};

class ES1370Driver
{
private:
	PciInf pciinfo;
	Pci *pci;
	ES1370 *es1370;

	dword baseIO;

	inline dword readControlRegister();
	inline void writeControlRegister(dword n);
	inline dword SerialControlRegister();
	inline void SerialControlRegister(dword n);
public:
	ES1370Driver();
	virtual ~ES1370Driver();

	bool existDevice();
	void MessageLoop();
	void setIRQ();
	void unsetIRQ();

	void setMempage(int p);
	void DAC1FrameRegister(void* p, size_t size);
	inline void enableDAC1Channel();
	void setSampleRate(int rate);
	void startDAC1();

	void readConf();
	void dumpRegisters();
	void playData(void* p, size_t size);
};

#endif
