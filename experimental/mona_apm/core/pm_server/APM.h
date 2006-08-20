#ifndef _APM_H_
#define _APM_H_

#include "PowerManager.h"

typedef struct _s apm_bios_regs;

class APM
{
public:
	APM();
	void MessageLoop();
	void init();
	void EventProcess();
	void getStatus();
private:
	int apm_bios_call(int, void*);
	int InterfaceDisconnect();
	int GetPowerStatus(int pdid, int*, int*, int*, int*);
	int EnablePowerManagement(int);
	int DisablePowerManagement(int);
	int EnableDevicePowerManagement(int);
	int DisableDevicePowerManagement(int);
	int EngagePowerManagement(int);
	int DisengagePowerManagement(int);
	int GetPMEvent(int*, int*);
	int GetPowerState(int, int*);
	int SetPowerState(int, int);
	int APMDriverVersion(int, int*);

	int acline;
	int battery;
	int battery_flag;
	int battery_life;
};


void dumpRegs(apm_bios_regs*);

#endif
