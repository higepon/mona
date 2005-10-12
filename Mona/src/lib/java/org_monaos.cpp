#include "org/monaos/IO.h"
#include "org/monaos/MessageInfo.h"
#include "org/monaos/Message.h"
#include "org/monaos/Screen.h"
#include <gcj/cni.h>
#include <monapi.h>
#include <sys/types.h>

// ============================================================
// org.monaos.IO
// ============================================================

jbyte org::monaos::IO::inp8 (jint port) {
	jbyte ret;
	asm volatile ("inb %%dx, %%al": "=a"(ret): "d"(port));
	return ret;
}

void org::monaos::IO::outp8 (jint port, jbyte value) {
	asm volatile ("outb %%al, %%dx": :"d" (port), "a" (value));
}

jshort org::monaos::IO::inp16 (jint port) {
	jshort ret;
	asm volatile ("inw %%dx, %%ax": "=a"(ret): "d"(port));
	return ret;
}

void org::monaos::IO::outp16 (jint port, jshort value) {
	asm volatile ("outw %%ax, %%dx": :"d" (port), "a" (value));
}

jint org::monaos::IO::inp32 (jint port) {
	jint ret;
	asm volatile ("inl %%dx, %%eax": "=a"(ret): "d"(port));
	return ret;
}

void org::monaos::IO::outp32 (jint port, jint value) {
	asm volatile ("outl %%eax, %%dx": :"d" (port), "a" (value));
}

void org::monaos::IO::setIRQ (jint irq, jboolean enabled, jboolean auto_ir2) {
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
}

// ============================================================
// org.monaos.Message
// ============================================================

jint org::monaos::Message::send(jint tid, ::org::monaos::MessageInfo *info) {
	::MessageInfo minfo;
	minfo.header = info->header;
	minfo.header = info->arg1;
	minfo.header = info->arg2;
	minfo.header = info->arg3;
	minfo.header = info->from;
	return MonAPI::Message::send(tid, &minfo);
}

jint org::monaos::Message::send(jint tid, jint header, jint arg1, jint arg2, jint arg3) {
	return MonAPI::Message::send(tid, header, arg1, arg2, arg3);
}

jint org::monaos::Message::receive (::org::monaos::MessageInfo *info) {
	::MessageInfo minfo;
	int result = MonAPI::Message::receive(&minfo);
	info->header = minfo.header;
	info->arg1 = minfo.arg1;
	info->arg2 = minfo.arg2;
	info->arg3 = minfo.arg3;
	info->from = minfo.from;
	return result;
}

jint org::monaos::Message::reply(::org::monaos::MessageInfo *info, jint arg2, jint arg3) {
	::MessageInfo minfo;
	int result = MonAPI::Message::reply(&minfo, arg2, arg3);
	info->header = minfo.header;
	info->arg1 = minfo.arg1;
	info->arg2 = minfo.arg2;
	info->arg3 = minfo.arg3;
	info->from = minfo.from;
	return result;
}

jint org::monaos::Message::peek(::org::monaos::MessageInfo *info, jint index) {
	::MessageInfo minfo;
	int result = MonAPI::Message::peek(&minfo, index, 0);
	info->header = minfo.header;
	info->arg1 = minfo.arg1;
	info->arg2 = minfo.arg2;
	info->arg3 = minfo.arg3;
	info->from = minfo.from;
	return result;
}

jboolean org::monaos::Message::exist() {
	return MonAPI::Message::exist();
}

jint org::monaos::Message::sendReceive(::org::monaos::MessageInfo *info, jint header, jint tid, jint arg1, jint arg2, jint arg3) {
	::MessageInfo minfo;
	minfo.header = info->header;
	minfo.header = info->arg1;
	minfo.header = info->arg2;
	minfo.header = info->arg3;
	minfo.header = info->from;
	return MonAPI::Message::sendReceive(&minfo, header, tid, arg1, arg2, arg3);
}

// ============================================================
// org.monaos.Screen
// ============================================================

jint org::monaos::Screen::getWidth() {
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.x;
}

jint org::monaos::Screen::getHeight() {
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.y;
}

jint org::monaos::Screen::getBpp() {
	volatile ScreenInfo info;
	syscall_get_vram_info(&info);
	return info.bpp;
}
