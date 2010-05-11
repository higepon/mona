#include <sys/types.h>
#include "apm.h"
#include "global.h"
#include "io.h"
#include "shutdown.h"

#if 0
void kill_all_proc()
{
	uint32_t *id_list;
	uint32_t id_num;
	uint32_t i;
	uint32_t myself;
	Thread *t;

	id_list = g_scheduler->GetAllThreadID(&id_num);
	myself = g_currentThread->thread->id;

	logprintf("\n");
	logprintf("CurrentThread: %x\n", myself);
	for( i = 0 ; i < id_num ; i++ )
	{
		if( id_list[i] == myself ) continue;
		logprintf("%x\n", id_list[i]);
		t = g_scheduler->Find(id_list[i]);
		if( t == NULL ) continue;
		g_scheduler->Kill(t);
	}
	id_list = g_scheduler->GetAllThreadID(&id_num);
	logprintf("n thread : %d\n", id_num);
}
#endif

uint32_t shutdown(uint32_t op, uint32_t device)
{
	if( op == SHUTDOWN_FEATURE )
	{
		return SHUTDOWN_FEATURE_APM;
	}

	if( op == SHUTDOWN_REBOOT )
	{
	    return shutdown_by_reboot();
	}

	if( g_apmInfo->version == 0x0102 )
	{
		return shutdown_by_apm(op, device);
	}

	return (uint32_t)-1;
}

uint32_t shutdown_by_apm(uint32_t op, uint32_t device)
{
	apm_bios_regs regs;
	uint32_t pstate;
	bool kill_threads = false;

	switch(op)
	{
		case SHUTDOWN_STANDBY: pstate = 1; break;
		case SHUTDOWN_SUSPEND: pstate = 2; break;
		case SHUTDOWN_HALT: pstate = 3; kill_threads = true; break;
		default: pstate = 0; break;
	}

//	if( kill_threads ) kill_all_proc();

	logprintf("shutdown by apm: Device = %d, OP = %d\n",device, op);

	regs.eax = 0x5307;
	regs.ebx = device;
	regs.ecx = pstate;

	return apm_bios(0x07, &regs);
}

uint32_t shutdown_by_reboot()
{
//	kill_all_proc();
	asm volatile("cli");

	for (;;) {
		if ((inp8(0x0064 /* PORT_KEYSTA */) & 0x02 /* KEYSTA_SEND_NOTREADY */) == 0) {
			break;
		}
	}
	outp8(0x0064 /* PORT_KEYCMD */, 0xfe);
	for (;;) {
		asm volatile("hlt");
	}

	return SHUTDOWN_REBOOT;
}
