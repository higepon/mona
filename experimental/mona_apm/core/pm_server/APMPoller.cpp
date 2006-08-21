#include "APM.h"
#include <sys/types.h>
#include <monapi.h>
#include <monapi/messages.h>

using namespace MonAPI;

APMPoller::APMPoller(dword ptid)
{
	this->ptid = ptid;
	timer = set_timer(1000);
	acline = -1;
	battery = -1;
	battery_flag = -1;
	battery_life = -1;
}

APMPoller::~APMPoller()
{
	kill_timer(timer);
}

void APMPoller::poll()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case MSG_TIMER:
				this->EventProcess();
				this->getStatus();
				break;
			default:
				break;
		}
		syscall_mthread_yield_message();
	}
}

void APMPoller::EventProcess()
{
	int event, info, result;

	result = APM::GetPMEvent(&event, &info);
	printf("PMEvent: %x, %x, %x\n", result, event, info);
}

void APMPoller::getStatus()
{
	int result;
	int ac, bt, btf, btl;

	result = APM::GetPowerStatus(1, &ac, &bt, &btf, &btl);

	if( result )
	{
		printf("Error: %d: cannot get status\n", result);
	}

	this->acline = ac;
	this->battery = bt;
	this->battery_flag = btf;
	this->battery_life = btl;

	printf("AC line: %s\n", (this->acline == 0) ? "Off-line" :
				(this->acline == 1) ? "On-line"  :
				(this->acline == 2) ? "backup" : "Unknown");
	printf("Battery: %s\n", (this->battery == 0) ? "High" :
				(this->battery == 1) ? "Low" :
				(this->battery == 2) ? "Critical" :
				(this->battery == 3) ? "Charging" : "Unknown");
	printf("Battery life: %d\n", (this->battery != 0xFF) ?
					this->battery_life : -1);
}
