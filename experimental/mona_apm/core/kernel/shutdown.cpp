#include <sys/types.h>
#include "apm.h"
#include "global.h"
#include "shutdown.h"

#if 0
void kill_all_proc()
{
	dword *id_list;
	dword id_num;
	dword i;

	id_list = g_scheduler->GetAllThreadID(&id_num);

	for( i = 0 ; i < id_num ; i++ )
	{
		if( id_list[i] != g_currentThread->thread->id )
		{
			ThreadOperation::kill(id_list[i]);
		}
	}
}
#endif

dword shutdown(dword op, dword device)
{
	if( op == SHUTDOWN_FEATURE )
	{
		return SHUTDOWN_FEATURE_APM;
	}

	if( g_apmInfo->version == 0x0102 )
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

//	kill_all_proc();

	regs.eax = 0x5307;
	regs.ebx = device;
	regs.ecx = pstate;

	return apm_bios(0x07, &regs);
}
