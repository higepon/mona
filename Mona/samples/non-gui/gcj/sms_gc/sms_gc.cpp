#include "sms_gc.h"
#include "sms_ptr_dict.h"
#ifdef SMS_DEBUG
#include <stdio.h>
#endif
#ifdef MONA
#include <monapi.h>
#else
#include <malloc.h>
#endif

typedef unsigned int* IntPtr;

static bool initialized = false;
static void* end_stack = NULL;
static void* start_heap = NULL;
static void* end_heap = NULL;
static sms_ptr_dict manager;
static int collect;
static int regs_length = 0, regs_size = 0;
static void*** regs = NULL;

void sms_gc_init(void* stack) {
	end_stack = stack;
	initialized = true;
	collect = 16;
}

void sms_gc_add(void* addr, int size, int type) {
	if (manager.get_length() >= collect) sms_gc_collect();
	manager.add(addr, size, type);
	if (start_heap == NULL) {
		start_heap = addr;
		end_heap = addr;
		return;
	}
	if (start_heap > addr) start_heap = addr;
	if (end_heap < addr) end_heap = addr;
}

void sms_gc_remove(void* addr) {
	manager.remove(addr);
}

void* sms_gc_malloc(int size) {
	void* addr = malloc(size);
	if (initialized) {
#ifdef SMS_DEBUG
		printf("sms_gc_malloc: %p\n", addr);
#endif
		sms_gc_add(addr, size, 0);
	}
	return addr;
}

void sms_gc_free(void* addr) {
#ifdef SMS_DEBUG
	printf("sms_gc_free: %p\n", addr);
#endif
	free(addr);
	sms_gc_remove(addr);
}

static void sms_gc_collect_internal(void* start, int size) {
	char* p = (char*)start;
	for (int i = 0; i < size; i++, p++) {
		void* data = (void*)*(IntPtr)p;
		if (start_heap <= data && data <= end_heap) {
			sms_ptr_dict_memory* p = manager.get_data(data);
			if (p == NULL || p->mark) continue;
			p->mark = true;
			sms_gc_collect_internal(p->addr, p->size);
		}
	}
}

void sms_gc_collect() {
#ifdef SMS_DEBUG
	printf("sms_gc_collect\n");
#endif
	for (sms_ptr_dict_linear* p = manager.get_first(); p != NULL; p = manager.get_next())
		p->unmark();

	char* start_stack;
	asm("movl %%ebp, %0" : "=g"(start_stack));
	sms_gc_collect_internal(start_stack, ((char*)end_stack) - start_stack);
	for (int i = 0; i < regs_length; i++) {
		sms_ptr_dict_memory* p = manager.get_data(*regs[i]);
		if (p == NULL || p->mark) continue;
		p->mark = true;
		sms_gc_collect_internal(p->addr, p->size);
	}

	for (sms_ptr_dict_linear* p = manager.get_first(); p != NULL; p = manager.get_next()) {
		for (int i = 0, plen; i < (plen = p->get_length());) {
			sms_ptr_dict_memory* m = p->get_data(i);
			if (!m->mark) {
				sms_gc_free(m->addr);
				if (plen == 1) break;
			} else {
				i++;
			}
		}
	}

	if (manager.get_length() > collect / 2) collect *= 2;
}

void sms_gc_register(void* ptr) {
#ifdef SMS_DEBUG
	printf("sms_gc_register: %p\n", ptr);
#endif
	if (regs_size == 0) {
		regs_size = 16;
		regs = (void***)malloc(regs_size * sizeof(void**));
	} else if (regs_size == regs_length) {
		regs_size *= 2;
		regs = (void***)realloc(regs, regs_size * sizeof(void**));
	}
	regs[regs_length++] = (void**)ptr;
}
