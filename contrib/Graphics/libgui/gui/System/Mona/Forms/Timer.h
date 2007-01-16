// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __SYSTEM_MONA_FORMS_TIMER_H__
#define __SYSTEM_MONA_FORMS_TIMER_H__

#ifdef MONA
#include <sys/types.h>
#endif
#include <gui/System/Pointer.h>
#include <gui/System/Event.h>

namespace System { namespace Mona { namespace Forms
{
	class Timer : public Object
	{
		friend class Application;
		
	private:
		int interval;
#ifdef MONA
		uint32_t sleep_tid;
#endif
		
	public:
		virtual String get_TypeName() { return "System.Mona.Forms.Timer"; }
		Timer();
		virtual ~Timer();
		virtual void Dispose();
		
		void Start();
		void Stop();
		
		inline int get_Interval() { return this->interval; }
		inline void set_Interval(int interval) { this->interval = interval; }
		
	protected:
		virtual void OnTick(_P<EventArgs> e);
		
		DECLARE_EVENT(System::IEventHandler, Tick, _P<System::EventArgs>)
	};
}}}

#endif  // __SYSTEM_MONA_FORMS_TIMER_H__
