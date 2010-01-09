#ifndef _MONAPI_SYSCALL_
#define _MONAPI_SYSCALL_

#include <sys/types.h>

typedef void (FuncVoid)();
typedef int (FuncMain)(int argc, char* argv[]);

#ifdef __cplusplus
#include <sys/List.h>


typedef int (FuncMonaMain)(List<char*>*);

extern void invokeFuncList(FuncVoid** list, const char* file, int line);
extern bool isInDLL(FuncVoid** ctors);
extern int MonaMain(List<char*>* pekoe);
extern void setupArguments(List<char*>* arg);

extern "C" {
#endif
extern void setConstructorList(FuncVoid** crots);
extern int user_start();
extern int sleep(uint32_t ms);
extern int set_timer(uint32_t ms);
extern int  kill_timer(uint32_t id);
extern int heavy();
extern int print(const char*, int direct);
extern int kill();
extern int exit(int error);
extern int mthread_create(void (*f)(void));
extern int mthread_create_with_arg(void __fastcall(*f)(void*), void* p);
extern int mthread_kill(uint32_t id);
extern int syscall_test(uint32_t laddress);
extern int syscall_sleep(uint32_t tick);
extern int syscall_log_print(const char* msg);
extern int syscall_set_timer(uint32_t tick);
extern int syscall_kill_timer(uint32_t id);
extern int syscall_print(const char*);
extern int syscall_kill();
extern int syscall_send(uint32_t id, MessageInfo* message);
extern int syscall_receive(MessageInfo* message);
extern int syscall_exist_message();
extern int syscall_mthread_create(void (*f)(void));
extern int syscall_mthread_create_with_arg(void __fastcall(*f)(void*), void* p);
extern int syscall_mthread_kill(uint32_t id);
extern uintptr_t syscall_mthread_self();
extern intptr_t syscall_condition_create(cond_t* cond);
extern intptr_t syscall_condition_destroy(cond_t* cond);
extern intptr_t syscall_condition_notify_all(cond_t* cond);
extern intptr_t syscall_condition_wait(cond_t* cond, mutex_t* mutex);
extern intptr_t syscall_condition_wait_timeout(cond_t* cond, mutex_t* mutex, intptr_t timeoutMsec);
extern intptr_t syscall_mutex_create(mutex_t* mutex);
extern intptr_t syscall_mutex_fetch(mutex_t* dest, mutex_t* mutex);
extern intptr_t syscall_mutex_try_lock(mutex_t* mutex);
extern intptr_t syscall_mutex_lock(mutex_t*);
extern intptr_t syscall_mutex_lock_timeout(mutex_t* id, intptr_t timeout);
extern intptr_t syscall_mutex_unlock(mutex_t* id);
extern intptr_t syscall_mutex_destroy(mutex_t* id);
extern intptr_t syscall_semaphore_create(uint32_t n, uint32_t handle);
extern intptr_t syscall_semaphore_trydown(intptr_t id);
extern intptr_t syscall_semaphore_down (intptr_t id );
extern intptr_t syscall_semaphore_up(intptr_t id);
extern intptr_t syscall_semaphore_destroy(intptr_t id);
extern uint64_t syscall_now_in_nanosec();
extern int syscall_get_vram_info(volatile ScreenInfo* info);
extern int syscall_get_cursor(int* x, int* y);
extern int syscall_set_cursor(int x, int y);

extern int syscall_map(uint32_t pid, uint32_t sharedId, uint32_t linearAddress, uint32_t size);
extern int syscall_map2(MappingInfo* info);
extern int syscall_unmap2(uint32_t sharedId);
extern int syscall_lookup_main_thread(const char* name);
extern uint32_t syscall_get_pid();
extern uint32_t syscall_get_tid();
extern int syscall_get_arg_count();
extern int syscall_get_arg(char* buf, int n);
extern int syscall_mthread_yield_message();
extern int syscall_get_date(KDate* date);
extern int syscall_get_io();
extern int syscall_set_ps_dump();
extern int syscall_read_ps_dump(PsInfo* info);
extern int syscall_load_process_image(LoadProcessInfo* info);
extern int syscall_kill_thread(uint32_t tid);
extern int syscall_get_physical_address(uint32_t linearAddress);

extern uint32_t syscall_memory_map_create(uint32_t size);
extern uint32_t syscall_memory_map_get_size(uint32_t id);
extern int syscall_memory_map_map(uint32_t id, uint32_t address);
extern int syscall_memory_map_unmap(uint32_t id);

extern uint8_t* syscall_allocate_dma_memory(int size); // size should be 4096 * x
extern uint32_t syscall_deallocate_dma_memory(void* address, int size); //size should be 4096 * x

extern uint32_t syscall_lookup(const char* name);
extern uint32_t syscall_get_tick();
extern int syscall_get_kernel_version(char* buf, uint32_t size);
extern int syscall_clear_screen();
extern int syscall_peek(MessageInfo* message, int index, int flags);

extern int syscall_set_irq_receiver(int irq, int maskInterrupt);
extern int syscall_has_irq_receiver(int irq);
extern int syscall_remove_irq_receiver(int irq);
extern int syscall_get_memory_info(MemoryInfo* INFO);
extern int syscall_free_pages(uint32_t address, uint32_t size);
extern int syscall_change_base_priority(uint32_t priority);

extern int syscall_set_dll_segment_writable();
extern int syscall_set_dll_segment_notshared(int index);

extern int syscall_shutdown(int op, int device);
extern int syscall_receive_packet(uint8_t* frame);
extern int syscall_send_packet(uint8_t* pkt, uint8_t* mac, uint32_t size, uint16_t pid);
extern int syscall_set_watch_point(void* address, int flag);
extern int syscall_remove_watch_point();


extern int syscall_allocate_contiguous(uint32_t laddress, int pageNum);
extern void syscall_deallocate_contiguous(uint32_t laddress, int pageNum);

extern void* malloc(unsigned long size);
extern void* calloc(unsigned long n, unsigned long s);
extern void* realloc(void* address, unsigned long size);
extern void free(void * address);
extern void __cxa_pure_virtual();
extern void _pure_virtual(void);
extern void __pure_virtual(void);
extern int atexit( void (*func)(void));

extern void putCharacter(char ch, int direct);
extern void putInt(size_t n, int base, int direct);
extern int printf(const char *format, ...);
extern void _printf(const char *format, ...);
extern void logprintf(const char* format, ...);
extern void _logprintf(const char* format, ...);
#define Log logprintf("%s#%s:%d:", __FILE__, __func__, __LINE__), logprintf
extern size_t _power(size_t x, size_t y);

#ifdef DEBUG
#define MONAPI_WARN(...) _printf(__VA_ARGS__), _printf(" %s:%d\n", __FILE__, __LINE__)
#else
#define MONAPI_WARN(...) /* */
#endif

#ifdef __cplusplus
}

extern void* operator new(size_t size);
extern void  operator delete(void* address);
extern void* operator new(size_t size, void* base);
extern void operator delete(void *base, void* address);
extern void* operator new[](size_t size, void* base);
extern void operator delete[](void* base, void* address);
#endif

/* syscall arguments enum */
enum {
    SYS_NO_MASK_INTERRUPT = 0,
    SYS_MASK_INTERRUPT,
};

/* key info */
#define VK_SPACE      0x20
#define VK_BACKSPACE  0x21
#define VK_ENTER      0x22
#define VK_PERIOD     0x30
#define VK_SLASH      0x2F
#define VK_A          0x61
#define VK_B          0x62
#define VK_C          0x63
#define VK_D          0x64
#define VK_E          0x65
#define VK_F          0x66
#define VK_G          0x67
#define VK_H          0x68
#define VK_I          0x69
#define VK_J          0x6A
#define VK_K          0x6B
#define VK_L          0x6C
#define VK_M          0x6D
#define VK_N          0x6E
#define VK_O          0x6F
#define VK_P          0x70
#define VK_Q          0x71
#define VK_R          0x72
#define VK_S          0x73
#define VK_T          0x74
#define VK_U          0x75
#define VK_V          0x76
#define VK_W          0x77
#define VK_X          0x78
#define VK_Y          0x79
#define VK_Z          0x7A
#define VK_0          0x7B
#define VK_1          0x7C
#define VK_2          0x7D
#define VK_3          0x7E
#define VK_4          0x7F
#define VK_5          0x80
#define VK_6          0x81
#define VK_7          0x82
#define VK_8          0x83
#define VK_9          0x84
#define VK_TEN_0      0x85
#define VK_TEN_1      0x86
#define VK_TEN_2      0x87
#define VK_TEN_3      0x88
#define VK_TEN_4      0x89
#define VK_TEN_5      0x8A
#define VK_TEN_6      0x8B
#define VK_TEN_7      0x8C
#define VK_TEN_8      0x8D
#define VK_TEN_9      0x8E
#define VK_TEN_MINUS  0x8F
#define VK_TEN_PLUS   0x90
#define VK_TEN_PERIOD 0x91

#endif
