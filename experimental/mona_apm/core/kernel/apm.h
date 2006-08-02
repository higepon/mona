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

#pragma pack(2)
typedef struct
{
	word version; // high = major low = minor
	word cs32;
	dword eip;
	word cs16;
	word ds;
	word cs32_len;
	word cs16_len;
	word ds_len;
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
word apm_set_power_state(word did, word state);
word apm_get_power_state(word did);
dword apm_bios(dword fn, dword ebx, dword ecx, dword edx, dword esi, dword edi);

#endif
