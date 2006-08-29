#include <sys/types.h>
#include "apm.h"
#include "global.h"
#include "kernel.h"

typedef struct
{
	dword offset;
	word segment __attribute__((packed));
} apm_bios_entry;

extern "C"
{
	apm_bios_entry apm_eip;
	word apm_bios_call(byte fn, apm_bios_regs *regs);
}

void apm_init(void)
{
	apm_eip.offset = g_apmInfo->eip;
	apm_eip.segment = 0x40;

/*
	g_console->printf("apm_eip = %x\n", apm_eip.offset);
	g_console->printf("apm_des = %x\n", apm_eip.segment);
*/

	//apm_enable();
}

dword apm_bios(dword fn, apm_bios_regs *regs)
{
	return apm_bios_call(0x5300|fn, regs) & 0xFF;
}

void apm_enable()
{
	apm_bios_regs regs;

	regs.eax = 0x5308;
	regs.ebx = 1;
	regs.ecx = 1;
	apm_bios(0x08, &regs);

	regs.eax = 0x530D;
	regs.ebx = 1;
	regs.ecx = 1;
	apm_bios(0x0D, &regs);

	regs.eax = 0x530F;
	regs.ebx = 1;
	regs.ecx = 1;
	apm_bios(0x0F, &regs);

	return;
}
