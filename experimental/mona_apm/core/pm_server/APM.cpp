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
	int result;
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case MSG_PM_CPU_IDLE:
				result = CPUIdle();
				Message::reply(&msg, result);
				break;
			case MSG_PM_CPU_BUSY:
				result = CPUBusy();
				Message::reply(&msg, result);
				break;
			case MSG_PM_SET_POWER_STATE:
				result = SetPowerState(msg.arg1, msg.arg2);
				Message::reply(&msg, result);
				break;
			case MSG_PM_ENABLE_PM:
				result = EnablePowerManagement(msg.arg1);
				Message::reply(&msg, result);
				break;
			case MSG_PM_DISABLE_PM:
				result = DisablePowerManagement(msg.arg1);
				Message::reply(&msg, result);
				break;
			case MSG_PM_GET_POWER_STATUS:
				this->pm_getPowerStatus(&msg);
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

	if( APM::EnablePowerManagement(0x0001) )
	{
		printf("Error: EnablePowerManagement.\n");
	}
	if( APM::EngagePowerManagement(0x0001) )
	{
		printf("Error: EngagePowerManagement.\n");
	}
	if( APM::EnableDevicePowerManagement(0x0001) )
	{
		printf("Error: EnableDevicePowerManagement.\n");
	}

//	thread = new PMThread(&poller);
//	thread->start();
}

void APM::getStatus()
{
	int result;
	int ac, bt, btf, btl;

	result = APM::GetPowerStatus(1, &ac, &bt, &btf, &btl);

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

void APM::pm_getPowerStatus(MessageInfo *msg)
{
	unsigned int ac, bt, btf, btl, a1, a2;

	GetPowerStatus(msg->arg1, (int*)&ac, (int*)&bt, (int*)&btf, (int*)&btl);

	a1 = (ac<<8) | bt;
	a2 = (btf<<8)| btl;
	Message::reply(msg, a1, a2);
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

	return apm_bios_call(0x04, &regs);
}

int APM::CPUIdle()
{
	apm_bios_regs regs;
	return apm_bios_call(0x05, &regs);
}

int APM::CPUBusy()
{
	apm_bios_regs regs;
	return apm_bios_call(0x06, &regs);
}

int APM::EnablePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x5308;
	regs.ebx = (dword)pdid;
	regs.ecx = 1;

	return apm_bios_call(0x08, &regs);
}

int APM::DisablePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x5308;
	regs.ebx = (dword)pdid;
	regs.ecx = 0;

	return apm_bios_call(0x08, &regs);
}

int APM::GetPowerStatus(int pdid, int *ac, int *bt, int *bt_flag, int *bt_life)
{
	apm_bios_regs regs;
	int result;

	regs.ebx = (dword)pdid;

	result = apm_bios_call(0x0A, &regs);

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

	return apm_bios_call(0x0D, &regs);
}

int APM::DisableDevicePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x530D;
	regs.ebx = (dword)pdid;
	regs.ecx = 0;

	return apm_bios_call(0x0D, &regs);
}

int APM::EngagePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x530F;
	regs.ebx = (dword)pdid;
	regs.ecx = 1;

	return apm_bios_call(0x0F, &regs);
}

int APM::DisengagePowerManagement(int pdid)
{
	apm_bios_regs regs;

	regs.eax = 0x530F;
	regs.ebx = (dword)pdid;
	regs.ecx = 0;

	return apm_bios_call(0x0F, &regs);
}

int APM::GetPMEvent(int *event, int *info)
{
	apm_bios_regs regs;
	int result;

//	regs.eax = 0x530B;

	result = apm_bios_call(0x0B, &regs);

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

	return apm_bios_call(0x07, &regs);
}

int APM::GetPowerState(int pdid, int *state)
{
	apm_bios_regs regs;
	int result;

	regs.eax = 0x530C;
	regs.ebx = (dword)pdid;

	result = apm_bios_call(0x0C, &regs);

	*state = (int)regs.ecx;

	return result;
}

int APM::APMDriverVersion(int ver, int *cver)
{
	apm_bios_regs regs;
	int result;

	regs.ebx = 0;
	regs.ecx = (dword)ver;
	result = apm_bios_call(0x0E, &regs);


	*cver = (int)regs.ecx;

	return result;
}

void dumpRegs(apm_bios_regs *r)
{
	printf("EAX = %x, EBX = %x, ECX = %x, EDX = %x, ESI = %x, EDI = %x\n", r->eax, r->ebx, r->ecx, r->edx, r->esi, r->edi);
}

int poller()
{
	APMPoller *apmp;
	apmp = new APMPoller(syscall_get_tid());
	//apmp->poll();
	return 0;
}
