/*!
    \file  apm.h
    \brief APM

    Copyright (c) 2004 Yume
    WITHOUT ANY WARRANTY

    \author  Yume
    \version $Revision: 2700 $
    \date   create:2004/01/17 update:$Date: 2005-07-17 22:27:24 +0900 (日, 17  7 2005) $
*/

#ifndef _MONA_APM_
#define _MONA_APM_

#include <sys/types.h>

typedef struct
{
	dword eax;
	dword ebx;
	dword ecx;
	dword edx;
	dword esi;
	dword edi;
}apm_bios_regs;

#pragma pack(2)
typedef struct
{
	dword cs32;
	dword eip;
	dword cs16;
	dword ds;
	word cs32_len;
	word cs16_len;
	dword ds_len;
	dword version; // high = major low = minor
	dword isSupported;
} APMInfo;
#pragma pack()

enum
{
	APM_INSTALL_CHECK,
	APM_REAL_CONNECT,
	APM_PROT16_CONNECT,
	APM_PROT32_CONNECT,
	APM_DISCONNECT,
	APM_CPU_IDLE,
	APM_CPU_BUSY,
	APM_SET_POWER_STATE,
	APM_ABLE_POWER_MANAGE,
	APM_RESTORE_DEFAULTS,
	APM_GET_POWER_STATE,
	APM_PM_EVENT,
	APM_GET_POWER_STATUS,
	APM_ABLE_DEVICE_POWER_MANAGE,
	APM_DRIVER_VERSION,
	APM_GAGE_POWER_MANAGE,
	APM_GET_CAPABILITIES,
	APM_RESUME_TIMER,
	APM_RESUME_ON_INDICATOR,
	APM_TIMER_BASED_REQUESTS,
	APM_OEM_APM_FUNCTIONS = 0x80,
};

void apm_init();
void apm_enable();
dword apm_bios(dword fn, apm_bios_regs* regs);

#endif
