// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Forms/Timer.h>

#ifdef MONA
#include <map>
std::map<dword, System::Mona::Forms::Timer*> mapTimers;
#endif

using namespace System;

namespace System { namespace Mona { namespace Forms
{
	Timer::Timer() : interval(100)
	{
#ifdef MONA
		this->sleep_tid = 0;
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
		if (this->sleep_tid != 0) return;
		
		this->sleep_tid = set_timer(this->interval);
		mapTimers[this->sleep_tid] = this;
#endif
	}
	
	void Timer::Stop()
	{
#ifdef MONA
		if (this->sleep_tid != 0)
		{
			mapTimers.erase(this->sleep_tid);
			kill_timer(this->sleep_tid);
			this->sleep_tid = 0;
		}
#endif
	}
	
	void Timer::OnTick(_P<EventArgs> e)
	{
		this->raise_Tick(this, e);
	}
}}}
