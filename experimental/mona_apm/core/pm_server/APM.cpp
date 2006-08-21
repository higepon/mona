#include <sys/types.h>
#include <monapi.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include "PowerManager.h"
#include "APM.h"

using namespace MonAPI;

enum
{
	APMP_NOTIFY,
};

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
}

void APM::MessageLoop()
{
	for (MessageInfo msg;;)
	{
				printf("APMP_NOTIFY\n");
		if (Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case APMP_NOTIFY:
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

	thread = new PMThread(&poller);
	thread->start();
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
