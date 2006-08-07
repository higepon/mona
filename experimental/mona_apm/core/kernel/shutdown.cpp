#include <sys/types.h>
#include "apm.h"
#include "global.h"
#include "shutdown.h"

dword shutdown(dword op, dword device)
{
	g_console->printf("Version: %x\n", g_apmInfo->version);
//	if( g_apmInfo->version == 0x0102 )
	{
		return shutdown_by_apm(op, device);
	}

	return (dword)-1;
}

dword shutdown_by_apm(dword op, dword device)
{
	apm_bios_regs regs;
	dword pstate;

	switch(op)
	{
		case SHUTDOWN_STANDBY: pstate = 1; break;
		case SHUTDOWN_SUSPEND: pstate = 2; break;
		case SHUTDOWN_HALT: pstate = 3; break;
		default: pstate = 0; break;
	}

	regs.eax = 0x5307;
	regs.ebx = device;
	regs.ecx = pstate;

	return apm_bios(0x07, &regs);
}
