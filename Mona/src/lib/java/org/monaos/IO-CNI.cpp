// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <org/monaos/IO.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#endif

jbyte
org::monaos::IO::inp8 (jint port)
{
#ifdef MONA
	jbyte ret;
	asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
	return ret;
#else
	return 0;
#endif
}


void
org::monaos::IO::outp8 (jint port, jbyte value)
{
#ifdef MONA
	asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
#endif
}


jshort
org::monaos::IO::inp16 (jint port)
{
#ifdef MONA
	jshort ret;
	asm volatile ("inw %%dx, %%ax": "=a"(ret): "d"(port));
	return ret;
#else
	return 0;
#endif
}


void
org::monaos::IO::outp16 (jint port, jshort value)
{
#ifdef MONA
	asm volatile ("outw %%ax, %%dx": :"d" (port), "a" (value));
#endif
}


jint
org::monaos::IO::inp32 (jint port)
{
#ifdef MONA
	jint ret;
	asm volatile ("inl %%dx, %%eax": "=a"(ret): "d"(port));
	return ret;
#else
	return 0;
#endif
}


void
org::monaos::IO::outp32 (jint port, jint value)
{
#ifdef MONA
	asm volatile ("outl %%eax, %%dx": :"d" (port), "a" (value));
#endif
}


void
org::monaos::IO::set_irq (jint irq, jboolean enabled, jboolean auto_ir2)
{
#ifdef MONA
	if (irq >= 0 && irq <= 7) {
		if (enabled) {
			outp8(0x21, (inp8(0x21) & ~(1 << irq)));
		} else {
			outp8(0x21, (inp8(0x21) | (1 << irq)));
		}
	} else if (irq >= 8 && irq <= 15) {
		if (enabled) {
			outp8(0xa1, inp8(0xa1) & ~(1 << (irq - 8)));
		} else {
			outp8(0xa1, inp8(0xa1) | (1 << (irq - 8)));
		}
		if (enabled && auto_ir2) {
			outp8(0x21, (inp8(0x21) & ~(1 << 2)));
		}
	}
#endif
}


