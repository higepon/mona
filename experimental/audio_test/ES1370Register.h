#ifndef _ES1370REG_H_
#define _ES1370REG_H_

#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi.h>

enum CONTROL
{
	SERR_DISABLE	= (1<<0),
	CDC_EN		= (1<<1),
	JYSTK_EN	= (1<<2),
	UART_EN		= (1<<3),
	ADC_EN		= (1<<4),
	DAC2_EN		= (1<<5),
	DAC1_EN		= (1<<6),
	BREQ		= (1<<7),
	XCTL0		= (1<<8),
	M_CB		= (1<<9),
	CCB_INTRM	= (1<<10),
	DAC_SYNC	= (1<<11),
	WTSRSEL		= (1<<12),
	M_SBB		= (1<<14),
	MSFMTSEL	= (1<<15),
	PCLKDIV		= (1<<16),
	OPEN		= (1<<29),
	XCTL1		= (1<<30),
	ADC_STOP	= (1<<31),
};

class ES1370;

class ES1370Register
{
public:
	void BitSetter(void (ES1370Register::*setter)(dword),
			dword (ES1370Register::*getter)(), bool flag, dword mask)
	{
		dword n;
		n = this->getter();
		if( flag ) n |= mask;
		else n &= ~ mask;
		this->setter(n);
	}
	bool BitGetter(dword (ES1370Register::*getter)(), dword mask)
	{
	}

};

class ControlRegister : ES1370Register
{
private:
	ES1370 & es1370;
public:
	ControlRegister(ES1370 & es1370);
	virtual ~ControlRegister();
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

#endif
