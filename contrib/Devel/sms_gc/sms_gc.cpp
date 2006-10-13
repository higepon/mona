#include "sms_gc/sms_gc.h"
#include "sms_gc/sms_gc_object.h"
#include "sms_gc/sms_ptr_dict.h"
#ifdef SMS_DEBUG
#include <stdio.h>
#endif
#ifdef MONA
#include <monapi.h>
#else
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#endif

static void* end_stack = NULL;
static void* start_heap = NULL;
static void* end_heap = NULL;
static sms_ptr_dict* manager = NULL;
static int collect;
static int regs_length = 0, regs_size = 0;
static void*** regs = NULL;
static void(*finalizers[SMS_GC_TYPE_MAX])(void*);

extern "C" void* sms_gc_get_end_stack() {
	return end_stack;
}

extern "C" sms_ptr_dict* sms_gc_new_ptr_dict() {
	return new sms_ptr_dict();
}

void sms_gc_init(void* stack) {
	if (manager != NULL)
		return;
	for (int i = 0; i < SMS_GC_TYPE_MAX; i++)
		finalizers[i] = NULL;
	manager = new sms_ptr_dict();
	end_stack = stack;
	collect = 16;
}

void sms_gc_set_finalizer(int type, void(*func)(void*))
{
	if (0 <= type && type < SMS_GC_TYPE_MAX)
		finalizers[type] = func;
}

void sms_gc_add(void* addr, int size, int type) {
	char* start_stack;
	asm("movl %%ebp, %0" : "=g"(start_stack));
	if (start_stack <= addr && addr < end_stack) {
#ifdef SMS_DEBUG
		printf("sms_gc_add: ignore stack = %p\n", addr);
#endif
		return;
	}
	if (manager->get_length() >= collect) sms_gc_collect();
	manager->add(addr, size, type);
	if (start_heap == NULL) {
		start_heap = addr;
		end_heap = addr;
		return;
	}
	if (start_heap > addr) start_heap = addr;
	if (end_heap < addr) end_heap = addr;
}

void sms_gc_remove(void* addr) {
	manager->remove(addr);
}

void* sms_gc_malloc(int size) {
	void* addr = malloc(size);
	if (manager != NULL) {
#ifdef SMS_DEBUG
		printf("sms_gc_malloc: %p\n", addr);
#endif
		sms_gc_add(addr, size, SMS_GC_TYPE_DEFAULT);
	}
	return addr;
}

void* sms_gc_malloc_type(int size, int type) {
	void* addr = malloc(size);
	if (manager != NULL) {
#ifdef SMS_DEBUG
		printf("sms_gc_malloc_type: %p\n", addr);
#endif
		sms_gc_add(addr, size, type);
	}
	return addr;
}

void sms_gc_free(void* addr) {
#ifdef SMS_DEBUG
	printf("sms_gc_free: %p\n", addr);
#endif
	sms_ptr_dict_memory* p = manager->get_data(addr);
	if (p == NULL) {
		free(addr);
	} else {
		memset(p->addr, 0, p->size);
		switch (p->type) {
			case SMS_GC_TYPE_CPP:
				delete (sms_gc_object*)addr;
				break;
			default:
				if (0 <= p->type && p->type < SMS_GC_TYPE_MAX && finalizers[p->type] != NULL)
					(*finalizers[p->type])(addr);
				free(addr);
				sms_gc_remove(addr);
				break;
		}
	}
}

sms_ptr_dict_memory* sms_gc_get_info(void* addr) {
	return manager->get_data(addr);
}

int sms_gc_get_size(void* addr) {
	sms_ptr_dict_memory* p = manager->get_data(addr);
	return p == NULL ? -1 : p->size;
}

static void sms_gc_collect_internal(void* start, int size) {
	char* p = (char*)start;
	size -= sizeof(void*); /**** BUG FIX ****/
	for (int i = 0; i <= size; i++, p++) {
		void* addr = *(void**)p;
		if (start_heap <= addr && addr <= end_heap) {
			sms_ptr_dict_memory* p = manager->get_data(addr);
			if (p == NULL || p->mark) continue;
			p->mark = true;
			if (p->type != SMS_GC_TYPE_IGNORE)
				sms_gc_collect_internal(p->addr, p->size);
		}
	}
}

void sms_gc_collect() {
#ifdef SMS_DEBUG
	printf("sms_gc_collect\n");
#endif
	for (sms_ptr_dict_linear* p = manager->get_first(); p != NULL; p = manager->get_next())
		p->unmark();

	char* start_stack;
	asm("movl %%ebp, %0" : "=g"(start_stack));
	sms_gc_collect_internal(start_stack, ((char*)end_stack) - start_stack);
	for (int i = 0; i < regs_length; i++) {
		sms_ptr_dict_memory* p = manager->get_data(*regs[i]);
		if (p == NULL || p->mark) continue;
		p->mark = true;
		if (p->type != SMS_GC_TYPE_IGNORE)
			sms_gc_collect_internal(p->addr, p->size);
	}

	for (sms_ptr_dict_linear* p = manager->get_first(); p != NULL; p = manager->get_next()) {
		for (int i = 0; p != NULL && i < p->get_length();) {
			sms_ptr_dict_memory* m = p->get_data(i);
			if (!m->mark) {
				sms_gc_free(m->addr);
				p = manager->get_current();
				i = 0;
			} else {
				i++;
			}
		}
	}

	if (manager->get_length() > collect / 2) collect *= 2;
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
	for (int i = 0; i < regs_length; i++)
		if (regs[i] == (void**)ptr)
			return;
	regs[regs_length++] = (void**)ptr;
}

void sms_gc_register_range(void* start, void* end){
	for (void** p = (void**)start; p <= end; p++)
		sms_gc_register(p);
}
