#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi.h>
#include <monapi/io.h>
#include <monapi/syscall.h>
#include "ES1370Register.h"

using namespace MonAPI;

#define BIT_UP(reg, x) reg(reg()|x)
#define BIT_DOWN(reg, x) reg(reg()&~x)
#define BIT_TEST(reg, x) { bool ret; (reg() & x) ? ret = true : ret = false; return ret; }

ControlRegister::ControlRegister(ES1370 & es1370)
{
	this->es1370 = es1370;
}

ControlRegister::~ControlRegister()
{
}

dword ControlRegister::SelectControlRegister()
{
	return inp32(IOport+ControlRegister_REG_CONTROL);
}

void ControlRegister::SelectControlRegister(dword n)
{
	outp32(IOport+ControlRegister_REG_CONTROL, n);
}

dword ControlRegister::SerialControlRegister()
{
	return inp32(IOport+ControlRegister_REG_SERIAL_CONTROL);
}

void ControlRegister::SerialControlRegister(dword n)
{
	outp32(IOport+ControlRegister_REG_SERIAL_CONTROL, n);
}

void ControlRegister::ADCStop(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, CONTROL.ADC_STOP);
}

bool ControlRegister::ADCStop()
{
	return BitGetter(&ControlRegister::SelectControlRegister, ADC_STOP);
}

void ControlRegister::XCTL1(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, XCTL1);
}

bool ControlRegister::XCTL1()
{
	return BitGetter(&ControlRegister::SelectControlRegister, XCTL1);
}

void ControlRegister::PCLKDIV(dword x)
{
	dword n = SelectControlRegister();
	x &= ~0xFFFFF000;
	x <<= 16;
	n |= x;
	SelectControlRegister(n);
}

dword ControlRegister::PCLKDIV()
{
	dword n = SelectControlRegister();
	n &= ~0xF0000000;
	n >>= 16;
	return n;
}

void ControlRegister::MSFMTSEL(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, MSFMTSEL);
}

bool ControlRegister::MSFMTSEL()
{
	return BitGetter(&ControlRegister::SelectControlRegister, MSFMTSEL);
}

void ControlRegister::M_SBB(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, M_SBB);
}

bool ControlRegister::M_SBB()
{
	return BitGetter(&ControlRegister::SelectControlRegister, M_SBB);
}

void ControlRegister::DAC_SYNC(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, DAC_SYNC);
}

bool ControlRegister::DAC_SYNC()
{
	return BitGetter(&ControlRegister::SelectControlRegister, DAC_SYNC);
}

void ControlRegister::setSampleRate(int rate)
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
	n = SelectControlRegister();
	n = n & ~0x3000;
	n |= bit;
	SelectControlRegister(n);
}

void ControlRegister::CCB_INTRM(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, CCB_INTRM);
}

bool ControlRegister::CCB_INTRM()
{
	return BitGetter(&ControlRegister::SelectControlRegister, CCB_INTRM);
}

void ControlRegister::M_CB(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, M_CB);
}

bool ControlRegister::M_CB()
{
	return BitGetter(&ControlRegister::SelectControlRegister, M_CB);
}

void ControlRegister::XCTL0(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, XCTL0);
}

bool ControlRegister::XCTL0()
{
	return BitGetter(&ControlRegister::SelectControlRegister, XCTL0);
}

void ControlRegister::BREQ(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, BREQ);
}

bool ControlRegister::BREQ()
{
	return BitGetter(&ControlRegister::SelectControlRegister, BREQ);
}

void ControlRegister::DAC1Enable(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, DAC1_EN);
}

bool ControlRegister::DAC1Enable()
{
	return BitGetter(&ControlRegister::SelectControlRegister, DAC1_EN);
}

void ControlRegister::DAC2Enable(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, DAC2_EN);
}

bool ControlRegister::DAC2Enable()
{
	return BitGetter(&ControlRegister::SelectControlRegister, DAC2_EN);
}

void ControlRegister::ADCEnable(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, ADC_EN);
}

bool ControlRegister::ADCEnable()
{
	return BitGetter(&ControlRegister::SelectControlRegister, ADC_EN);
}

void ControlRegister::UARTEnable(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, UART_EN);
}

bool ControlRegister::UARTEnable()
{
	return BitGetter(&ControlRegister::SelectControlRegister, UART_EN);
}

void ControlRegister::JYSTKEnable(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, JYSTK_EN);
}

bool ControlRegister::JYSTKEnable()
{
	return BitGetter(&ControlRegister::SelectControlRegister, JYSTK_EN);
}

void ControlRegister::CDCEnable(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, CDC_EN);
}

bool ControlRegister::CDCEnable()
{
	return BitGetter(&ControlRegister::SelectControlRegister, CDC_EN);
}

void ControlRegister::SERRDisable(bool flag)
{
	BitSetter(&ControlRegister::SelectControlRegister, &ES1370::SelectControlRegister, flag, SERR_DISABLE);
}

bool ControlRegister::SERRDisable()
{
	return BitGetter(&ControlRegister::SelectControlRegister, SERR_DISABLE);
}

