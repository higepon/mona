#include <sys/types.h>
#include <monapi.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include "PowerManager.h"
#include "APM.h"

using namespace MonAPI;

typedef struct _s
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
	acline = -1;
	battery = -1;
	battery_flag = -1;
	battery_life = -1;
}

void APM::MessageLoop()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case MSG_TIMER:
				this->EventProcess();
				this->getStatus();
				break;
			default:
				break;
		}
	}
}

void APM::init()
{
	DisableDevicePowerManagement(1);
	DisengagePowerManagement(1);
	DisablePowerManagement(1);

	if( this->EnablePowerManagement(0x0001) )
	{
		printf("Error: EnablePowerManagement.\n");
	}
	if( this->EngagePowerManagement(0x0001) )
	{
		printf("Error: EngagePowerManagement.\n");
	}
	if( this->EnableDevicePowerManagement(0x0001) )
	{
		printf("Error: EnableDevicePowerManagement.\n");
	}


	set_timer(1000);
}

void APM::EventProcess()
{
	int event, info, result;

	result = this->GetPMEvent(&event, &info);
	printf("PMEvent: %x, %x, %x\n", result, event, info);
}

void APM::getStatus()
{
	int result;
	int ac, bt, btf, btl;

	result = this->GetPowerStatus(1, &ac, &bt, &btf, &btl);

	if( result )
	{
		printf("Error: %d: cannot get status\n", result);
	}

	this->acline = ac;
	this->battery = bt;
	this->battery_flag = btf;
	this->battery_life = btl;

	printf("AC line: %s\n", (this->acline == 0) ? "Off-line" :
				(this->acline == 1) ? "On-line"  :
				(this->acline == 2) ? "backup" : "Unknown");
	printf("Battery: %s\n", (this->battery == 0) ? "High" :
				(this->battery == 1) ? "Low" :
				(this->battery == 2) ? "Critical" :
				(this->battery == 3) ? "Charging" : "Unknown");
	printf("Battery life: %d\n", (this->battery != 0xFF) ?
					this->battery_life : -1);
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

int APM::GetPowerStatus(int pdid, int *ac, int *bt, int *bt_flag, int *bt_life)
{
	apm_bios_regs regs;
	int result;

	regs.ebx = (dword)pdid;

	result = this->apm_bios_call(0x0A, &regs);

	dumpRegs(&regs);

	*ac = regs.ebx >> 8;
	*bt = regs.ebx & ~0xFF;
	*bt_flag = regs.ecx >> 8;
	*bt_life = regs.ecx & ~0xFF;

	return result;
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

//	regs.eax = 0x530B;

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

int APM::APMDriverVersion(int ver, int *cver)
{
	apm_bios_regs regs;
	int result;

	regs.ebx = 0;
	regs.ecx = (dword)ver;
	result = this->apm_bios_call(0x0E, &regs);


	*cver = (int)regs.ecx;

	return result;
}

void dumpRegs(apm_bios_regs *r)
{
	printf("EAX = %x, EBX = %x, ECX = %x, EDX = %x, ESI = %x, EDI = %x\n", r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi);
}
