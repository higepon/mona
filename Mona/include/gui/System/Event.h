// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_EVENT_H__
#define __SYSTEM_EVENT_H__

#include <gui/System/Object.h>
#include <gui/System/Pointer.h>
#include <gui/System/Collections/ArrayList.h>

#define DECLARE_DELEGATE(name, argtype) \
	class I##name : public System::Object { public: virtual void Invoke(_P<System::Object>, argtype) = 0; }; \
	template <class T> class name : public I##name { \
	private: \
		_P<T> ptr; void(T::*func)(_P<System::Object>, argtype); \
	public: \
		name(_P<T> p, void(T::*f)(_P<System::Object>, argtype)) : ptr(p), func(f) {} \
		virtual ~name() {} \
		virtual void Invoke(_P<System::Object> sender, argtype arg) { ((ptr.get())->*func)(sender, arg); } \
	}; \
	class name##_S : public I##name { \
	private: \
		void(*func)(_P<System::Object>, argtype); \
	public: \
		name##_S(void(*f)(_P<System::Object>, argtype)) : func(f) {} \
		virtual ~name##_S() {} \
		virtual void Invoke(_P<System::Object> sender, argtype arg) { (*func)(sender, arg); } \
	};

#define DECLARE_EVENT(handler, name, argtype) \
	private: \
		_P<System::Collections::ArrayList<_P<handler> > > name; \
	public: \
		void add_##name(_P<handler> delg) { \
			if (this->name == 0 /*NULL*/) this->name = new System::Collections::ArrayList<_P<handler> >; \
			this->name->Add(delg); } \
		void remove_##name(_P<handler> delg) { \
			if (this->name == 0 /*NULL*/) return; \
			this->name->Remove(delg); \
			if (this->name->get_Count() == 0) this->name.Unset(); } \
	protected: \
		void raise_##name(_P<System::Object> sender, argtype e) { \
			if (this->name == 0 /*NULL*/) return; \
			FOREACH_AL(_P<handler>, h, this->name) h->Invoke(sender, e); }

namespace System
{
	class EventArgs : public Object
	{
	public:
		virtual String get_TypeName() { return "System.EventArgs"; }
		EventArgs() {};
		virtual ~EventArgs() {};
		
		inline static _P<EventArgs> get_Empty() { return new EventArgs(); }
	};
	DECLARE_DELEGATE(EventHandler, _P<EventArgs>)
}

#endif  // __SYSTEM_EVENT_H__
