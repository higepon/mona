#ifndef __SMS_GC_H__
#define __SMS_GC_H__

#define SMS_GC_INIT() { void* ebp; asm("movl %%ebp, %0" : "=g"(ebp)); sms_gc_init(ebp); }

#define SMS_GC_TYPE_DEFAULT    0
#define SMS_GC_TYPE_IGNORE     1
#define SMS_GC_TYPE_CPP     1000
#define SMS_GC_TYPE_JAVA    2000

#ifdef __cplusplus
extern "C" {
#endif
void sms_gc_init(void* stack);
void sms_gc_add(void* addr, int size, int type);
void sms_gc_remove(void* addr);
void* sms_gc_malloc(int size);
void* sms_gc_malloc_type(int size, int type);
void sms_gc_free(void* addr);
int sms_gc_get_size(void* addr);
void sms_gc_collect();
void sms_gc_register(void* ptr);
void sms_gc_register_range(void* start, void* end);
#ifdef __cplusplus
}
#endif

#endif
