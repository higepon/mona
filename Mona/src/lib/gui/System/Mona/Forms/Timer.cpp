// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Timer.h>

#ifdef MONA
#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/cmessage.h>

#define MSG_GUI_TIMER 0x40f0

static dword my_tid = THREAD_UNKNOWN;

static void SleepThread()
{
	MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);
	
	MessageInfo msg;
	MonAPI::Message::receive(&msg, my_tid, MSG_GUI_TIMER);
	int interval = msg.arg1;
	dword target = msg.arg2;
	
	for (;;)
	{
		sleep(interval);
		MonAPI::Message::send(my_tid, MSG_GUI_TIMER, target);
	}
}
#endif

using namespace System;

namespace System { namespace Mona { namespace Forms
{
	Timer::Timer() : interval(100)
	{
#ifdef MONA
		this->sleep_tid = THREAD_UNKNOWN;
#endif
	}
	
	Timer::~Timer()
	{
		this->Dispose();
	}
	
	void Timer::Dispose()
	{
		this->Stop();
		this->Tick = NULL;
	}
	
	void Timer::Start()
	{
#ifdef MONA
		if (this->sleep_tid != THREAD_UNKNOWN) return;
		if (my_tid == THREAD_UNKNOWN) my_tid = syscall_get_tid();
		
		dword id = syscall_mthread_create((dword)SleepThread);
		syscall_mthread_join(id);
		MessageInfo msg;
		monapi_cmessage_receive_header_only(&msg, MSG_SERVER_START_OK);
		this->sleep_tid = msg.from;
		MonAPI::Message::send(this->sleep_tid, MSG_GUI_TIMER, this->interval, (dword)this);
#endif
	}
	
	void Timer::Stop()
	{
#ifdef MONA
		if (this->sleep_tid != THREAD_UNKNOWN)
		{
			syscall_kill_thread(this->sleep_tid);
		}
#endif
	}
	
	void Timer::OnTick(_P<EventArgs> e)
	{
		this->raise_Tick(this, e);
	}
}}}
