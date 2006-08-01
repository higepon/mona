#include <sys/types.h>
#include "apm.h"
#include "global.h"
#include "kernel.h"

/* APMの関数群はCPL=0で実行しなければならない */

typedef struct
{
	word ax;
	word bx;
	word cx;
	word dx;
	word si;
	word di;
}apm_bios_regs;

typedef struct
{
	dword offset;
	word segment __attribute__((packed));
} apm_bios_entry;

extern "C"
{
	apm_bios_entry apm_eip;
	apm_bios_entry *apm_ml;
	word apm_cs;
	word apm_bios_call(byte fn, apm_bios_regs *regs,apm_bios_entry apm_ent);
}

void apm_init(void)
{
	apm_cs = g_apmInfo->cs32;
	apm_eip.offset = g_apmInfo->eip;
	apm_eip.segment = 0x40;
	apm_ml = &apm_eip;

	g_console->printf("apm_eip = %x\n", apm_eip.offset);
	g_console->printf("apm_des = %x\n", apm_eip.segment);
	g_console->printf("apm_cs  = %x\n", apm_cs);
	g_console->printf("apm_ml = %x\n", apm_ml);
}

/*
word apm_bios_call(byte fn, apm_bios_regs *regs)
{
	word ret = 0;

	asm volatile("push %ebp");
	asm volatile("movl 8(%ebp), %eax");
	asm volatile("movw 2(%eax), %bx");
	asm volatile("movw 4(%eax), %cx");
	asm volatile("movw 6(%eax), %dx");
	asm volatile("movw 8(%eax), %si");
	asm volatile("movw 10(%eax), %di");

	asm volatile("movl $0x40, %eax");
	asm volatile("movl %eax, %ds");
	asm volatile("movl %eax, %es");
	asm volatile("movl 8(%ebp), %eax");
	asm volatile("movb $0x53, %ah");

	asm volatile("lcall *%fs:_apm_eip");
	asm volatile("movl $0x10, %ebp");
	asm volatile("movl %ebp, %ds");
	asm volatile("movl %ebp, %es");
	asm volatile("pop %ebp");
	asm volatile("jc Lapm_bios_call_error");

	asm volatile("movl 8(%ebp), %eax");
	asm volatile("movw %ax, (%eax)");
	asm volatile("movw %bx, 2(%eax)");
	asm volatile("movw %cx, 4(%eax)");
	asm volatile("movw %dx, 6(%eax)");
	asm volatile("movw %si, 8(%eax)");
	asm volatile("movw %di, 8(%eax)");
	asm volatile("jmp Lapm_bios_call_end");

	asm volatile("Lapm_bios_call_error:");
	asm volatile("cbw");
	asm volatile("cwde");
	asm volatile("movw %ax, -4(%ebp)");
	asm volatile("Lapm_bios_call_end:");
	return ret;
}
*/

word apm_set_power_state(word did, word state)
{
	apm_bios_regs regs;

	regs.ax = 0x5307;
	regs.bx = did;
	regs.cx = state;
	regs.dx = 0;
	regs.di = 0;
	regs.si = 0;

	return apm_bios_call(0x5307, &regs, apm_eip);
}

word apm_get_power_state(word did)
{
	apm_bios_regs regs;

	regs.ax = 0x530C;
	regs.bx = did;

	g_console->printf("Calling apm function.");
	apm_bios_call(0x530C, &regs, apm_eip);
	return regs.cx;
}
