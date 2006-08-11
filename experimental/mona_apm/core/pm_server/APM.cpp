#include <sys/types.h>
#include <monapi.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include "PowerManager.h"
#include "APM.h"

using namespace MonAPI;

typedef struct
{
	dword eax;
	dword ebx;
	dword ecx;
	dword edx;
	dword esi;
	dword edi;
} apm_bios_regs;

APM::APM()
{
}

void APM::MessageLoop()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case MSG_TIMER:
				EventProcess();
				break;
			default:
				break;
		}
	}
}

void APM::init()
{

	this->EnablePowerManagement(0x0001);
	this->EngagePowerManagement(0x0001);
	this->EnableDevicePowerManagement(0x0001);

	set_timer(1000);
}

void APM::EventProcess()
{
	int event, info, result;

	result = this->GetPMEvent(&event, &info);
	printf("PMEvent: %x, %x, %x\n", result, event, info);
}

int APM::apm_bios_call(int fn, void *p)
{
	dword result;
	SYSCALL_2(SYSTEM_CALL_APM_BIOS, result, fn, p);
	return (int)result;
}

int APM::InterfaceDisconnect()
{
	apm_bios_regs regs;

	regs.eax = 0x5304;
	regs.ebx = 0;

	return this->apm_bios_call(0x04, &regs);
}

int APM::EnablePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x5308;
	regs.ebx = (dword)pdid;
	regs.ecx = 1;

	return this->apm_bios_call(0x08, &regs);
}

int APM::DisablePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x5308;
	regs.ebx = (dword)pdid;
	regs.ecx = 0;

	return this->apm_bios_call(0x08, &regs);
}

int APM::EnableDevicePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x530D;
	regs.ebx = (dword)pdid;
	regs.ecx = 1;

	return this->apm_bios_call(0x0D, &regs);
}

int APM::DisableDevicePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x530D;
	regs.ebx = (dword)pdid;
	regs.ecx = 0;

	return this->apm_bios_call(0x0D, &regs);
}

int APM::EngagePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x530F;
	regs.ebx = (dword)pdid;
	regs.ecx = 1;

	return this->apm_bios_call(0x0F, &regs);
}

int APM::DisengagePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x530F;
	regs.ebx = (dword)pdid;
	regs.ecx = 0;

	return this->apm_bios_call(0x0F, &regs);
}

int APM::GetPMEvent(int *event, int *info)
{
	apm_bios_regs regs;
	int result;

	regs.eax = 0x530B;

	result = this->apm_bios_call(0x0B, &regs);

	*event = (int)regs.ebx;
	*info = (int)regs.ecx;

	return result;
}

int APM::SetPowerState(int pdid, int state)
{
	apm_bios_regs regs;

	regs.eax = 0x5307;
	regs.ebx = (dword)pdid;
	regs.ecx = (dword)state;

	return this->apm_bios_call(0x07, &regs);
}

int APM::GetPowerState(int pdid, int *state)
{
	apm_bios_regs regs;
	int result;

	regs.eax = 0x530C;
	regs.ebx = (dword)pdid;

	result = this->apm_bios_call(0x0C, &regs);

	*state = (int)regs.ecx;

	return result;
}
