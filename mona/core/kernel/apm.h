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
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
}apm_bios_regs;

#pragma pack(2)
typedef struct
{
	uint32_t cs32;
	uint32_t eip;
	uint32_t cs16;
	uint32_t ds;
	uint16_t cs32_len;
	uint16_t cs16_len;
	uint32_t ds_len;
	uint32_t version; // high = major low = minor
	uint32_t isSupported;
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
uint32_t apm_bios(uint32_t fn, apm_bios_regs* regs);

#endif
