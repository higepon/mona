#include "APM.h"
#include <sys/types.h>
#include <monapi.h>
#include <monapi/messages.h>

using namespace MonAPI;

APMPoller::APMPoller(dword ptid)
{
	this->ptid = ptid;
	timer = set_timer(1000);
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


