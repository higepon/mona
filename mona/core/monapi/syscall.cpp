/*
    Title: System calls

    Primitive system calls.
*/

#include <monapi.h>
#include <monapi/messages.h>

using namespace MonAPI;

inline intptr_t syscall0(intptr_t syscall_number)
{
    intptr_t ret = 0;
    asm volatile("movl $%c1, %%ebx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(ret)
                 :"g"(syscall_number)
                 :"ebx"
                 );
    return ret;
}

inline intptr_t syscall1(intptr_t syscall_number, intptr_t arg1)
{
    intptr_t ret = 0;
    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(ret)
                 :"g"(syscall_number), "m"(arg1)
                 :"ebx", "esi"
                 );
    return ret;
}

inline intptr_t syscall2(intptr_t syscall_number, intptr_t arg1, intptr_t arg2)
{
    intptr_t ret = 0;
    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(ret)
                 :"g"(syscall_number), "m"(arg1), "m"(arg2)
                 :"ebx", "esi", "ecx"
                 );
    return ret;
}

inline intptr_t syscall3(intptr_t syscall_number, intptr_t arg1, intptr_t arg2, intptr_t arg3)
{
    intptr_t ret = 0;
    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "movl %4  , %%edi \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(ret)
                 :"g"(syscall_number), "m"(arg1), "m"(arg2), "m"(arg3)
                 :"ebx", "esi", "ecx", "edi"
                 );
    return ret;
}

inline intptr_t syscall4(intptr_t syscall_number, intptr_t arg1, intptr_t arg2, intptr_t arg3, intptr_t arg4)
{
    intptr_t ret = 0;
    asm volatile("movl $%c1, %%ebx \n"
                 "movl %2  , %%esi \n"
                 "movl %3  , %%ecx \n"
                 "movl %4  , %%edi \n"
                 "movl %5  , %%edx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(ret)
                 :"g"(syscall_number), "m"(arg1), "m"(arg2), "m"(arg3), "m"(arg4)
                 :"ebx", "esi", "ecx", "edi", "edx"
                 );
    return ret;
}


/*----------------------------------------------------------------------
    system call wrappers
----------------------------------------------------------------------*/
int sleep(uint32_t ms) {

    uint32_t tick = ms / KERNEL_TIMER_INTERVAL_MSEC;

    if (tick <= 0)
    {
        tick = 1;
    }

    return syscall_sleep(tick);
}

int set_timer(uint32_t ms)
{
    uint32_t tick = ms / KERNEL_TIMER_INTERVAL_MSEC;

    if (tick <= 0)
    {
        tick = 1;
    }
    return syscall_set_timer(tick);
}

int kill_timer(uint32_t id)
{
    return syscall_kill_timer(id);
}

#define PRINT_BUFFER_SIZE 128
int print(const char* msg, int direct)
{
    int len = strlen(msg);
    if (direct == 1)
    {
        char buf[PRINT_BUFFER_SIZE];
        if (len >= PRINT_BUFFER_SIZE)
        {
            memcpy(buf, msg, PRINT_BUFFER_SIZE - 1);
            buf[PRINT_BUFFER_SIZE - 1] = '\0';
            syscall_print("print overflow direct==1");
        }
        else
        {
            memcpy(buf, msg, len + 1);
        }
        syscall_print(buf);
    }
    else if (direct == 0)
    {
        monapi_stdout_write((uint8_t*)msg, strlen(msg) + 1);
    }
    else if (direct == 2)
    {
        char buf[PRINT_BUFFER_SIZE];
        if (len >= PRINT_BUFFER_SIZE)
        {
            memcpy(buf, msg, PRINT_BUFFER_SIZE - 1);
            buf[PRINT_BUFFER_SIZE - 1] = '\0';
            syscall_print("print overflow direct==2");
        }
        else
        {
            memcpy(buf, msg, len + 1);
        }
        syscall_log_print(buf);
    }
    return 0;
}

int kill() {
    return syscall_kill();
}

int exit(int error)
{
    if (MonAPI::Message::send(monapi_get_server_thread_id(ID_PROCESS_SERVER),
                              MSG_PROCESS_TERMINATED, MonAPI::System::getThreadID(), error) != M_OK) {
        printf("Error %s:%d\n", __FILE__, __LINE__);
    }
    return syscall_kill();
}

int mthread_create(void (*f)(void))
{
    return syscall_mthread_create(f);
}


int mthread_create_with_arg(void __fastcall(*f)(void*), void* arg)
{
    return syscall_mthread_create_with_arg(f, arg);
}

int mthread_kill(uint32_t id)
{
    return syscall_mthread_kill(id);
}

/*----------------------------------------------------------------------
    for C++
----------------------------------------------------------------------*/
int atexit( void (*func)(void)) {
    return -1;
}

void __cxa_pure_virtual() {
    print("__cxa_pure_virtual called\n", 1);
}

void _pure_virtual() {
    print("_pure_virtual called\n", 1);
}

void __pure_virtual() {
    print("_pure_virtual called\n", 1);
}

/*----------------------------------------------------------------------
    printf
----------------------------------------------------------------------*/
int printf(const char *format, ...) {

    char buf[128];
    int bufpos = 0;

    void** list = (void **)((void *)&format);

    list++;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            if (bufpos > 0) {
                buf[bufpos] = '\0';
                print(buf, 0);
                bufpos = 0;
            }
            i++;

            switch (format[i]) {
              case 's':
                  print((char *)*list, 0);
                  list++;
                  break;
              case 'd':
                  putInt(*(int*)list, 10, 0);
                  list++;
                  break;
              case 'x':
                  print("0x", 0);
                  putInt(*(int*)list, 16, 0);
                  list++;
                  break;
              case 'c':
                  putCharacter((char)*(int*)(list), 0);
                  list++;
                  break;
              case '%':
                  putCharacter('%', 0);
                  break;
              case '\0':
                  i--;
                  break;
            }
        } else {
            buf[bufpos] = format[i];
            bufpos++;
            if (bufpos == 127) {
                buf[bufpos] = '\0';
                print(buf, 0);
                bufpos = 0;
            }
        }
    }

    if (bufpos > 0) {
        buf[bufpos] = '\0';
        print(buf, 0);
    }
    return 0;
}

void putCharacter(char ch, int direct)
{
    char str[2];
    str[0] = ch;
    str[1] = '\0';
    print(str, direct);
}

void putInt(size_t n, int base, int direct)
{
    static char buf[256];
    int geta;
    int num = n;

    if (base != 16)
    {
        if (n == 0)
        {

            putCharacter('0', direct);
            return;
        }
        for (geta = 0; num; num /= 10, geta++);
        if ((int)n < 0)
        {
            geta++;
            base *= -1;
        }
    }
    else
    {
        geta = 8;
    }

    char* p = ltona(n, buf, geta, base);

    for (; *p != '\0'; p++)
    {
        putCharacter(*p, direct);
    }
}

void _printf(const char* format, ...)
{
    int direct = 1;
    // output directry
    static char buf[128];
    int bufpos = 0;
    void** list = (void**)&format;
    list++;
    for (int i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%')
        {
            if (bufpos > 0)
            {
                buf[bufpos] = '\0';
                print(buf, direct);
                bufpos = 0;
            }
            i++;

            switch (format[i]) {
              case 's':
                  print((char*)*list, direct);
                  list++;
                  break;
              case 'd':
                  putInt(*(int*)list, 10, direct);
                  list++;
                  break;
              case 'x':
                  print("0x", direct);
                  putInt(*(int*)list, 16, direct);
                  list++;
                  break;
              case 'c':
                  putCharacter((char)*(int*)(list), direct);
                  list++;
                  break;
              case '%':
                  putCharacter('%', direct);
                  break;
              case '\0':
                  i--;
                  break;
            }
        }
        else
        {
            buf[bufpos] = format[i];
            bufpos++;
            if (bufpos == 127) {
                buf[bufpos] = '\0';
                print(buf, direct);
                bufpos = 0;
            }
        }
    }

    if (bufpos > 0)
    {
        buf[bufpos] = '\0';
        print(buf, direct);
    }
}

void _logprintf(const char* format, ...)
{
    int direct = 2;
    // output directry
    static char buf[128];
    int bufpos = 0;
    void** list = (void**)&format;
    list++;
    for (int i = 0; format[i] != '\0'; i++)
    {
        if (format[i] == '%')
        {
            if (bufpos > 0)
            {
                buf[bufpos] = '\0';
                print(buf, direct);
                bufpos = 0;
            }
            i++;

            switch (format[i]) {
              case 's':
                  print((char*)*list, direct);
                  list++;
                  break;
              case 'd':
                  putInt(*(int*)list, 10, direct);
                  list++;
                  break;
              case 'x':
                  print("0x", direct);
                  putInt(*(int*)list, 16, direct);
                  list++;
                  break;
              case 'c':
                  putCharacter((char)*(int*)(list), direct);
                  list++;
                  break;
              case '%':
                  putCharacter('%', direct);
                  break;
              case '\0':
                  i--;
                  break;
            }
        }
        else
        {
            buf[bufpos] = format[i];
            bufpos++;
            if (bufpos == 127) {
                buf[bufpos] = '\0';
                print(buf, direct);
                bufpos = 0;
            }
        }
    }

    if (bufpos > 0)
    {
        buf[bufpos] = '\0';
        print(buf, direct);
    }
}

/*----------------------------------------------------------------------
    System call
----------------------------------------------------------------------*/
int syscall_mthread_create(void (*f)(void))
{
    return syscall2(SYSTEM_CALL_MTHREAD_CREATE, (uint32_t)f, 0);
}

typedef struct st_thread_entry
{
    void __fastcall(*func)(void*);
    void* arg;
};


static void __fastcall thread_entry(void* arg)
{
    st_thread_entry* entry = (st_thread_entry*)arg;
    (*(entry->func))(entry->arg);
    _printf("\n\n************************************ thread exited"); // for debug.
    delete entry;
    exit(0);
}

int syscall_mthread_create_with_arg(void __fastcall(*func)(void*), void* arg)
{
    st_thread_entry* entry = new st_thread_entry;
    entry->func = func;
    entry->arg = arg;
    return syscall2(SYSTEM_CALL_MTHREAD_CREATE, (intptr_t)thread_entry, (intptr_t)entry);
}

int syscall_mthread_kill(uint32_t id)
{
    return syscall1(SYSTEM_CALL_MTHREAD_KILL, id);
}

uintptr_t syscall_mthread_self()
{
    return syscall0(SYSTEM_CALL_MTHREAD_SELF);
}

int syscall_sleep(uint32_t tick)
{
    return syscall1(SYSTEM_CALL_MTHREAD_SLEEP, tick);
}

int syscall_print(const char* msg)
{
    return syscall1(SYSTEM_CALL_PRINT, (intptr_t)msg);
}

int syscall_kill()
{
    intptr_t result = syscall0(SYSTEM_CALL_KILL);

    /* not reached */
    return result;
}

int syscall_send(uint32_t pid, MessageInfo* message)
{
    return syscall2(SYSTEM_CALL_SEND, pid, (intptr_t)message);
}

int syscall_receive(MessageInfo* message)
{
    return syscall1(SYSTEM_CALL_RECEIVE, (intptr_t)message);
}

/*
   function: syscall_condition_create

   Creates a new condition variable.

   Parameters:

     cond - cond_t

   Returns:

     <M_OK>.

*/
intptr_t syscall_condition_create(cond_t* cond)
{
    intptr_t ret = syscall0(SYSTEM_CALL_CONDITION_CREATE);
    *cond = ret;
    return M_OK;
}


/*
   function: syscall_condition_notify_all

   Unblocks all the threads waiting on the condition.

   Parameters:

     cond - cond_t created by <syscall_condition_create>.

   Returns:
     Returns <M_OK> if all the theads are successfully unblocked, or <M_BAD_CONDITION_ID> if condition is invalid.

*/
intptr_t syscall_condition_notify_all(cond_t* cond)
{
    intptr_t cond_id = *cond;
    return syscall1(SYSTEM_CALL_CONDITION_NOTIFY_ALL, cond_id);
}


/*
   function: syscall_condition_wait

   Waits on the condition.

   Parameters:

     cond - cond_t created by <syscall_condition_create>.
     mutex - mutex created by <syscall_mutex_create>.

   Returns:
     Returns <M_OK> if the waiting thread is successfully unblocked, <M_BAD_CONDITION_ID> if condition is invalid or <M_BAD_MUTEX_ID> if mutex is invalid.

*/
intptr_t syscall_condition_wait(cond_t* cond, mutex_t* mutex)
{
    intptr_t cond_id = *cond;
    intptr_t mutex_id = *mutex;
    intptr_t ret = syscall2(SYSTEM_CALL_CONDITION_WAIT, cond_id, mutex_id);
    syscall_mutex_lock(mutex);
    if (ret == M_EVENT_CONDITION_NOTIFY) {
        return M_OK;
    } else {
        return ret;
    }
}


inline static intptr_t msec_to_tick(intptr_t msec)
{
    intptr_t tick = msec / KERNEL_TIMER_INTERVAL_MSEC;
    if (tick <= 0) {
        tick = 1;
    }
    return tick;
}


/*
   function: syscall_condition_wait_timeout

   Waits on the condition until ublocked by a notify or timed out.

   Parameters:

     cond - cond_t created by <syscall_condition_create>.
     mutex - mutex_t created by <syscall_mutex_create>.
     timeoutMsec - timeout in msec.

   Returns:

     Returns <M_OK> if the waiting thread is successfully unblocked. <M_TIMED_OUT>, when timeout. <M_BAD_CONDITION_ID> if condition is invalid or <M_BAD_MUTEX_ID> if mutex is invalid.

*/
intptr_t syscall_condition_wait_timeout(cond_t* cond, mutex_t* mutex, intptr_t timeoutMsec)
{
    intptr_t tick = msec_to_tick(timeoutMsec);
    intptr_t cond_id = *cond;
    intptr_t mutex_id = *mutex;
    intptr_t ret = syscall3(SYSTEM_CALL_CONDITION_WAIT_TIMEOUT, cond_id, mutex_id, tick);
    if (ret == M_EVENT_CONDITION_NOTIFY) {
        return M_OK;
    } else {
        return ret;
    }
}


/*
   function: syscall_condition_destroy

   Destroy the condition.

   Parameters:

     cond - cond_t created by <syscall_condition_create>.

   Returns:
     Returns <M_OK> if the condition is successfully destoryed, or <M_BAD_CONDITION_ID> if condition is invalid.

*/
intptr_t syscall_condition_destroy(cond_t* cond)
{
    intptr_t cond_id = *cond;
    return syscall1(SYSTEM_CALL_CONDITION_DESTROY, cond_id);
}


/*
   function: syscall_mutex_create

   Creates a new mutex.


   Parameters:

     mutex - mutex_t.

   Returns:

     <M_OK>.

*/
intptr_t syscall_mutex_create(mutex_t* mutex)
{
    int type = MUTEX_CREATE_NEW;
    int mutex_id = syscall1(SYSTEM_CALL_MUTEX_CREATE, type);
    *mutex = mutex_id;
    return M_OK;
}

/*
   function: syscall_mutex_fetch

   Fetch a mutex by a exsisting mutex_t, the mutex may be created by other process.

   Parameters:

     dest  - mutex_t.
     mutex - mutex_t created by <syscall_mutex_create>.

   Returns:

     Returns M_OK or <M_BAD_MUTEX_ID> if mutex is invalid.

*/
intptr_t syscall_mutex_fetch(mutex_t* dest, mutex_t* mutex)
{
    intptr_t mutex_id = *mutex;
    ASSERT(mutex_id != MUTEX_CREATE_NEW);
    intptr_t ret = syscall1(SYSTEM_CALL_MUTEX_CREATE, mutex_id);
    if (ret > 0) {
        *dest = ret;
        return M_OK;
    } else {
        return ret;
    }
}


/*
   function: syscall_mutex_try_lock

   Try to lock the mutex. When the mutex is locked by other process, returns <M_BUSY>.

   Parameters:

     mutex - mutex_t created by <syscall_mutex_create>.

   Returns:

     Returns <M_OK> if the mutex is successfully locked. <M_BUSY> if the mutex is locked by other process. <M_BAD_MUTEX_ID> if mutex is invalid.

*/
intptr_t syscall_mutex_try_lock(mutex_t* mutex)
{
    intptr_t mutex_id = *mutex;
    return syscall1(SYSTEM_CALL_MUTEX_TRY_LOCK, mutex_id);
}


/*
   function: syscall_mutex_lock_timeout

   Block the callee thread until get a lock or timeout.

   Parameters:

     mutex - mutex_t created by <syscall_mutex_create>.
     timeoutMsec - timeout in msec.

   Returns:

     <M_OK>, when the thread gets a lock. <M_TIMED_OUT>, when timeout. <M_BAD_MUTEX_ID> if mutex is invalid.

*/
intptr_t syscall_mutex_lock_timeout(mutex_t* mutex, intptr_t timeoutMsec)
{
    intptr_t tick = msec_to_tick(timeoutMsec);
    intptr_t mutex_id = *mutex;
    intptr_t ret = syscall2(SYSTEM_CALL_MUTEX_LOCK, mutex_id, tick);
    if (ret == M_EVENT_MUTEX_UNLOCKED) {
        return M_OK;
    } else {
        return ret;
    }
}

/*
   function: syscall_mutex_lock

   Block the callee thread until get a lock.

   Parameters:

     mutex - mutex_t created by <syscall_mutex_create>.

   Returns:

     Returns <M_OK> if the mutex is successfully locked, or <M_BAD_MUTEX_ID> if mutex is invalid.

*/
intptr_t syscall_mutex_lock(mutex_t* mutex)
{
    intptr_t noTimeout = 0;
    intptr_t mutex_id = *mutex;
    intptr_t ret = syscall2(SYSTEM_CALL_MUTEX_LOCK, mutex_id, noTimeout);
    if (ret == M_EVENT_MUTEX_UNLOCKED) {
        return M_OK;
    } else {
        return ret;
    }
}

/*
   function: syscall_mutex_unlock

   unlock the mutex.

   Parameters:

     mutex - mutex_t created by <syscall_mutex_create>.

   Returns:
     Returns <M_OK> if the mutex is successfully unlocked, or <M_BAD_MUTEX_ID> if mutex is invalid.
*/
intptr_t syscall_mutex_unlock(mutex_t* mutex)
{
    intptr_t mutex_id = *mutex;
    return syscall1(SYSTEM_CALL_MUTEX_UNLOCK, mutex_id);
}

/*
   function: syscall_mutex_destroy

   Destroy the mutex.

   Parameters:

     mutex - mutex_t created by <syscall_mutex_create>.

   Returns:
     Returns <M_OK> if the mutex is successfully destoryed, <M_RELEASED> if mutex is released, but someone has referance to the mutex, or <M_BAD_MUTEX_ID> if mutex is invalid.

*/
intptr_t syscall_mutex_destroy(mutex_t* mutex)
{
    intptr_t mutex_id = *mutex;
    return syscall1(SYSTEM_CALL_MUTEX_DESTROY, mutex_id);
}

/*
   function: syscall_mutex_count

   Returns number of mutexes on the system. Used for test.


   Returns:

     Number of mutexes.
*/
intptr_t syscall_mutex_count()
{
    int type = MUTEX_CREATE_NEW;
    return syscall0(SYSTEM_CALL_MUTEX_COUNT);
}

intptr_t syscall_semaphore_create(uint32_t n, uint32_t handle)
{
    return syscall2(SYSTEM_CALL_SEMAPHORE_CREATE, n, handle);
}

intptr_t syscall_semaphore_trydown(intptr_t id)
{
    return syscall1(SYSTEM_CALL_SEMAPHORE_TRYDOWN, id);
}

intptr_t syscall_semaphore_down (intptr_t id )
{
    return syscall1(SYSTEM_CALL_SEMAPHORE_DOWN, id);
}

intptr_t syscall_semaphore_up(intptr_t id)
{
    return syscall1(SYSTEM_CALL_SEMAPHORE_UP, id);
}

intptr_t syscall_semaphore_destroy(intptr_t id)
{
    return syscall1(SYSTEM_CALL_SEMAPHORE_DESTROY, id);
}

uint32_t syscall_lookup(const char* name)
{
    return syscall1(SYSTEM_CALL_LOOKUP, (intptr_t)name);
}

int syscall_lookup_main_thread(const char* name)
{
    return syscall1(SYSTEM_CALL_LOOKUP_MAIN_THREAD, (intptr_t)name);
}

int syscall_get_vram_info(volatile ScreenInfo* info)
{
    return syscall1(SYSTEM_CALL_GET_VRAM_INFO, (intptr_t)info);
}

int syscall_get_cursor(int* x, int* y)
{
    return syscall2(SYSTEM_CALL_GET_CURSOR, (intptr_t)x, (intptr_t)y);
}

int syscall_set_cursor(int x, int y)
{
    return syscall2(SYSTEM_CALL_SET_CURSOR, x, y);
}

uint32_t syscall_get_pid()
{
    return syscall0(SYSTEM_CALL_GET_PID);
}

uint32_t syscall_get_tid()
{
    return syscall0(SYSTEM_CALL_GET_TID);
}

int syscall_get_arg_count()
{
    return syscall0(SYSTEM_CALL_ARGUMENTS_NUM);
}

int syscall_get_arg(char* buf, int n)
{
    return syscall2(SYSTEM_CALL_GET_ARGUMENTS, (intptr_t)buf, n);
}

int syscall_mthread_yield_message()
{
    return syscall0(SYSTEM_CALL_MTHREAD_YIELD_MESSAGE);
}

int syscall_get_date(KDate* date)
{
    return syscall1(SYSTEM_CALL_DATE, (intptr_t)date);
}

int syscall_get_io()
{
    return syscall0(SYSTEM_CALL_GET_IO);
}

int syscall_exist_message()
{
    return syscall0(SYSTEM_CALL_EXIST_MESSAGE);
}

uint32_t syscall_memory_map_create(uint32_t size)
{
    return syscall1(SYSTEM_CALL_MEMORY_MAP_CREATE, size);
}

/*
   function: syscall_memory_map_get_size

   Returns size of memory map

   Parameters:

     id - memory map id

   Returns:

     Size or 0 on error.

*/
uintptr_t syscall_memory_map_get_size(uintptr_t id)
{
    return syscall1(SYSTEM_CALL_MEMORY_MAP_GET_SIZE, id);
}

int syscall_memory_map_map(uint32_t id, uint32_t address)
{
    return syscall2(SYSTEM_CALL_MEMORY_MAP_MAP, id, address);
}

int syscall_memory_map_unmap(uint32_t id)
{
    return syscall1(SYSTEM_CALL_MEMORY_MAP_UNMAP, id);
}

int syscall_set_ps_dump()
{
    return syscall0(SYSTEM_CALL_PS_DUMP_SET);
}

int syscall_read_ps_dump(PsInfo* info)
{
    return syscall1(SYSTEM_CALL_PS_DUMP_READ, (intptr_t)info);
}

uint32_t syscall_get_tick()
{
    return syscall0(SYSTEM_CALL_GET_TICK);
}

int syscall_get_kernel_version(char* buf, uint32_t size)
{
    return syscall2(SYSTEM_CALL_GET_KERNEL_VERSION, (intptr_t)buf, size);
}

int syscall_load_process_image(LoadProcessInfo* info)
{
    return syscall1(SYSTEM_CALL_LOAD_PROCESS_IMAGE, (intptr_t)info);
}

int syscall_kill_thread(uint32_t tid)
{
    return syscall1(SYSTEM_CALL_KILL_THREAD, tid);
}

int syscall_clear_screen()
{
    return syscall0(SYSTEM_CALL_CLEAR_SCREEN);
}

int syscall_peek(MessageInfo* message, int index, int flags)
{
    return syscall3(SYSTEM_CALL_PEEK, (intptr_t)message, index, flags);
}

int syscall_test(uint32_t laddress)
{
    return syscall1(SYSTEM_CALL_TEST, laddress);
}

int syscall_set_irq_receiver(int irq, int maskInterrupt)
{
    return syscall2(SYSTEM_CALL_SET_IRQ_RECEIVER, irq, maskInterrupt == SYS_MASK_INTERRUPT ? 1 : 0);
}

int syscall_has_irq_receiver(int irq)
{
    return syscall1(SYSTEM_CALL_HAS_IRQ_RECEIVER, irq);
}

int syscall_remove_irq_receiver(int irq)
{
    return syscall1(SYSTEM_CALL_REMOVE_IRQ_RECEIVER, irq);
}

int syscall_free_pages(uint32_t address, uint32_t size)
{
    uint32_t result;
    return syscall2(SYSTEM_CALL_FREE_PAGES, address, size);
    return result;
}

int syscall_get_memory_info(MemoryInfo* info)
{
    return syscall1(SYSTEM_CALL_GET_MEMORY_INFO, (intptr_t)info);
}

uint8_t* syscall_allocate_dma_memory(int size)
{
    return (uint8_t*)syscall1(SYSTEM_CALL_ALLOCATE_DMA_MEMORY, size);
}

uint32_t syscall_deallocate_dma_memory(void* address, int size)
{
    return syscall2(SYSTEM_CALL_DEALLOCATE_DMA_MEMORY, (intptr_t)address, size);
}

int syscall_set_timer(uint32_t tick)
{
    return syscall1(SYSTEM_CALL_SET_TIMER, tick);
}

int syscall_kill_timer(uint32_t id)
{
    return syscall1(SYSTEM_CALL_KILL_TIMER, id);
}

int syscall_change_base_priority(uint32_t priority)
{
    return syscall1(SYSTEM_CALL_CHANGE_BASE_PRIORITY, priority);
}

int syscall_set_dll_segment_writable()
{
    return syscall0(SYSTEM_CALL_SET_DLL_SEGMENT_WRITABLE);
}

int syscall_set_dll_segment_notshared(int index)
{
    return syscall1(SYSTEM_CALL_SET_DLL_SEGMENT_NOTSHARED, index);
}

int syscall_shutdown(int op, int device)
{
    return syscall2(SYSTEM_CALL_SHUTDOWN, op, device);
}

int syscall_log_print(const char* msg)
{
    return syscall1(SYSTEM_CALL_LOG_PRINT, (intptr_t)msg);
}

int syscall_receive_packet(uint8_t* frame)
{
    return syscall1(SYSTEM_CALL_RECEIVE_PACKET, (intptr_t)frame);
}

int syscall_send_packet(uint8_t* pkt, uint8_t* mac, uint32_t size, uint16_t pid)
{
    return syscall4(SYSTEM_CALL_SEND_PACKET, (intptr_t)pkt, (intptr_t)mac, size, pid);
}

int syscall_set_watch_point(void* address, int flag)
{
    return syscall2(SYSTEM_CALL_SET_WATCH_POINT, (intptr_t)address, flag);
}

int syscall_get_physical_address(uint32_t linearAddress)
{
    return syscall1(SYSTEM_CALL_GET_PHYSICAL_ADDRESS, linearAddress);
}

int syscall_remove_watch_point()
{
    return syscall0(SYSTEM_CALL_REMOVE_WATCH_POINT);
}

int syscall_allocate_contiguous(uint32_t laddress, int pageNum)
{
    return syscall2(SYSTEM_CALL_ALLOCATE_CONTIGUOUS, laddress, pageNum);
}

void syscall_deallocate_contiguous(uint32_t laddress, int pageNum)
{
    syscall2(SYSTEM_CALL_DEALLOCATE_CONTIGUOUS, laddress, pageNum);
}

uint64_t syscall_now_in_nanosec()
{
    union {
        struct {
            uint32_t l;
            uint32_t h;
        } u32;
        uint64_t u64;
    } n;
    syscall2(SYSTEM_CALL_NOW_IN_NANOSEC, (intptr_t)(&(n.u32.l)), (intptr_t)(&(n.u32.h)));
    return n.u64;
}

#include <monapi/MapFileParser.h>

int syscall_stack_trace_enable(uint32_t pid, const char* map_file_path)
{
    FileReader reader;
    if(!reader.open(map_file_path))
        return false;

    MapFileScanner<FileReader> scanner(reader);
    MapFileParser<MapFileScanner<FileReader> > parser(scanner);

    parser.parseAll();
    monapi_cmemoryinfo* cm =  parser.symbolInfos_.serialize();
    if(cm == NULL) 
        return false;

    int res =  syscall3(SYSTEM_CALL_STACKTRACE_ENABLE, pid, (intptr_t)cm->Data, cm->Size);
    
    monapi_cmemoryinfo_dispose(cm);
    monapi_cmemoryinfo_delete(cm);

    return res == 0;
}
void syscall_stack_trace_disable(uint32_t pid)
{
     syscall1(SYSTEM_CALL_STACKTRACE_DISABLE, pid);
}

void syscall_stack_trace_dump(uint32_t pid)
{
     syscall1(SYSTEM_CALL_STACKTRACE_DUMP, pid);
}



