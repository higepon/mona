#ifndef __SMS_GC_H__
#define __SMS_GC_H__

#define SMS_GC_INIT() { void* ebp; asm("movl %%ebp, %0" : "=g"(ebp)); sms_gc_init(ebp); }

#ifdef __cplusplus
extern "C" {
#endif
void sms_gc_init(void* stack);
void sms_gc_add(void* addr, int size);
void sms_gc_remove(void* addr);
void* sms_gc_malloc(int size);
void sms_gc_free(void* addr);
void sms_gc_collect();
void sms_gc_register(void* ptr);
#ifdef __cplusplus
}
#endif

#endif
