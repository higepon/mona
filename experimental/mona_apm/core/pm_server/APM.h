#ifndef _APM_H_
#define _APM_H_

#include "PowerManager.h"

class APM
{
public:
	APM();
	void MessageLoop();
	void init();
	void EventProcess();
private:
	int apm_bios_call(int, void*);
	int InterfaceDisconnect();
	int EnablePowerManagement(int);
	int DisablePowerManagement(int);
	int EnableDevicePowerManagement(int);
	int DisableDevicePowerManagement(int);
	int EngagePowerManagement(int);
	int DisengagePowerManagement(int);
	int GetPMEvent(int*, int*);
	int GetPowerState(int, int*);
	int SetPowerState(int, int);
};

#endif
