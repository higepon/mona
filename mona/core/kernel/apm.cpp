#include <sys/types.h>
#include "apm.h"
#include "global.h"
#include "kernel.h"

typedef struct
{
	uint32_t offset;
	uint16_t segment __attribute__((packed));
} apm_bios_entry;

extern "C"
{
	apm_bios_entry apm_eip;
	uint16_t apm_bios_call(uint8_t fn, apm_bios_regs *regs);
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

uint32_t apm_bios(uint32_t fn, apm_bios_regs *regs)
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
