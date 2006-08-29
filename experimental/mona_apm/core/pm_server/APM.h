#ifndef _APM_H_
#define _APM_H_

#include <sys/types.h>

typedef struct _s apm_bios_regs;

class APM
{
public:
	APM();
	void MessageLoop();
	void init();
	void getStatus();
	void pm_getPowerStatus(MessageInfo *);

	static int InterfaceDisconnect();
	static int CPUIdle();
	static int CPUBusy();
	static int GetPowerStatus(int pdid, int*, int*, int*, int*);
	static int EnablePowerManagement(int);
	static int DisablePowerManagement(int);
	static int EnableDevicePowerManagement(int);
	static int DisableDevicePowerManagement(int);
	static int EngagePowerManagement(int);
	static int DisengagePowerManagement(int);
	static int GetPMEvent(int*, int*);
	static int GetPowerState(int, int*);
	static int SetPowerState(int, int);
	static int APMDriverVersion(int, int*);
private:
	static int apm_bios_call(int, void*);

	int acline;
	int battery;
	int battery_flag;
	int battery_life;
};

void dumpRegs(apm_bios_regs*);

#endif
