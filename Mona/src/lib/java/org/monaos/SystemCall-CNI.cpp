// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <org/monaos/SystemCall.h>
#include <org/monaos/PsInfo.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>
#endif

jint
org::monaos::SystemCall::sleep (jint tick)
{
#ifdef MONA
	return syscall_sleep(tick);
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::set_timer (jint tick)
{
#ifdef MONA
	return syscall_set_timer(tick);
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::kill_timer (jint id)
{
#ifdef MONA
	return syscall_kill_timer(id);
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::kill_thread (jint id)
{
#ifdef MONA
	return syscall_kill_thread(id);
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::get_cursor ()
{
#ifdef MONA
	int x, y;
	syscall_get_cursor(&x, &y);
	return (x << 16 | y);
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::get_io ()
{
#ifdef MONA
	return syscall_get_io();
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::get_pid ()
{
#ifdef MONA
	return syscall_get_pid();
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::get_tid ()
{
#ifdef MONA
	return syscall_get_tid();
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::clear_screen ()
{
#ifdef MONA
	return syscall_clear_screen();
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::set_cursor (jint x, jint y)
{
#ifdef MONA
	return syscall_set_cursor(x, y);
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::set_irq_receiver (jint irq)
{
#ifdef MONA
	return syscall_set_irq_receiver(irq);
#else
	return 0;
#endif
}




jint
org::monaos::SystemCall::set_ps_dump ()
{
#ifdef MONA
	return syscall_set_ps_dump();
#else
	return 0;
#endif
}


jint
org::monaos::SystemCall::read_ps_dump (::org::monaos::PsInfo *info)
{
#ifdef MONA
	::PsInfo psinfo;
	int result = syscall_read_ps_dump(&psinfo);
	info->name = _Jv_NewStringUTF(psinfo.name);
	info->state = psinfo.state;
	info->cr3 = psinfo.cr3;
	info->eip = psinfo.eip;
	info->esp = psinfo.esp;
	info->tid = psinfo.tid;
	return result;
#else
	return 0;
#endif
}


