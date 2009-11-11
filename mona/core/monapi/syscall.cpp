/*
    Title: System calls

    Primitive system calls.
*/

#include <monapi.h>
#include <monapi/messages.h>

using namespace MonAPI;

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
    MonAPI::Message::send(monapi_get_server_thread_id(ID_PROCESS_SERVER),
                          MSG_PROCESS_TERMINATED, MonAPI::System::getThreadID(), error);
    return syscall_kill();
}

int mthread_create(void (*f)(void)) {
    return syscall_mthread_create(f);
}

int mthread_create_with_arg(void __fastcall(*f)(void*), void* p) {
    return syscall_mthread_create_with_arg(f, p);
}

int mthread_kill(uint32_t id) {
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
    int result, arg2 = 0;
    SYSCALL_2(SYSTEM_CALL_MTHREAD_CREATE, result, (uint32_t)f, arg2);
    return result;
}

int syscall_mthread_create_with_arg(void __fastcall(*f)(void*), void* p)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_MTHREAD_CREATE, result, f, p);
    return result;
}

int syscall_mthread_kill(uint32_t id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MTHREAD_KILL, result, id);
    return result;
}

int syscall_sleep(uint32_t tick)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MTHREAD_SLEEP, result, tick);
    return result;
}

int syscall_print(const char* msg)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_PRINT, result, msg);
    return result;
}

int syscall_kill()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_KILL, result);

    /* not reached */
    return result;
}

int syscall_send(uint32_t pid, MessageInfo* message)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_SEND, result, pid, message);
    return result;
}

int syscall_receive(MessageInfo* message)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_RECEIVE, result, message);
    return result;
}

/*
   function: syscall_mutex_create

   Creates and returns a new mutex.

   Returns:

     A positive mutexid.

*/
intptr_t syscall_mutex_create()
{
    intptr_t result;
    int type = MUTEX_CREATE_NEW;
    SYSCALL_1(SYSTEM_CALL_MUTEX_CREATE, result, type);
    return result;
}

/*
   function: syscall_mutex_fetch

   Fetch a mutex by mutexid, the mutex may be created by other process.

   Parameters:

     id - mutexid returned by <syscall_mutex_create>.

   Returns:

     A positive mutexid if exists. Returns IDM_OBJECT_NOT_FOUND, IDM_SECURITY_ERROR or IDM_INVALID_TYPE on error.

*/
intptr_t syscall_mutex_fetch(intptr_t mutexid)
{
    ASSERT(mutexid != MUTEX_CREATE_NEW);
    intptr_t result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_CREATE, result, mutexid);
    return result;
}

/*
   function: syscall_mutex_try_lock

   Try to lock the mutex. When the mutex is locked by other process, returns M_

   Parameters:

     id - mutexid returned by <syscall_mutex_create>.

   Returns: M_EVENT_MUTEX_UNLOCKED, when the thread gets a lock. IDM_OBJECT_NOT_FOUND, IDM_SECURITY_ERROR or IDM_INVALID_TYPE on error.

*/
intptr_t syscall_mutex_trylock(intptr_t id)
{
    intptr_t result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_TRYLOCK, result, id);
    return result;
}

/*
   function: syscall_mutex_lock_timeout

   Block the callee thread until get a lock or timeout.

   Parameters:

     id - mutexid returned by <syscall_mutex_create>.
     timeoutMsec - timeout in msec.

   Returns: M_EVENT_MUTEX_UNLOCKED, when the thread gets a lock. M_EVENT_SLEEP, when timeout. IDM_OBJECT_NOT_FOUND, IDM_SECURITY_ERROR or IDM_INVALID_TYPE on error.

*/
intptr_t syscall_mutex_lock_timeout(intptr_t id, intptr_t timeoutMsec)
{
    intptr_t tick = timeoutMsec / KERNEL_TIMER_INTERVAL_MSEC;
    if (tick <= 0) {
        tick = 1;
    }

    intptr_t result;
    SYSCALL_2(SYSTEM_CALL_MUTEX_LOCK, result, id, tick);
    return result;
}

/*
   function: syscall_mutex_lock

   Block the callee thread until get a lock.

   Parameters:

     id - mutexid returned by <syscall_mutex_create>.

     Returns: Returns <<M_OK>> if the mutex is successfully locked, or <<M_BAD_MUTEX_ID> if mutexid is invalid.

*/
intptr_t syscall_mutex_lock(intptr_t id)
{
    intptr_t result;
    intptr_t noTimeout = 0;
    SYSCALL_2(SYSTEM_CALL_MUTEX_LOCK, result, id, noTimeout);
    return result;
}

/*
   function: syscall_mutex_unlock

   unlock the mutex.

   Parameters:

     id - mutexid returned by <syscall_mutex_create>.

     Returns: Returns <<M_OK>> if the mutex is successfully unlocked, or <<M_BAD_MUTEX_ID> if mutexid is invalid.
*/
intptr_t syscall_mutex_unlock(intptr_t id)
{
    intptr_t result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_UNLOCK, result, id);
    return result;
}

/*
   function: syscall_mutex_destroy

   destroy the mutex.

   Parameters:

     id - mutexid returned by <syscall_mutex_create>.

   Returns: Returnes M_OK. IDM_OBJECT_NOT_FOUND, IDM_SECURITY_ERROR or IDM_INVALID_TYPE on error.

*/
intptr_t syscall_mutex_destroy(intptr_t id)
{
    intptr_t result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_DESTROY, result, id);
    return result;
}

int syscall_semaphore_create(uint32_t n, uint32_t handle)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_SEMAPHORE_CREATE, result, n, handle);
    return result;
}

int syscall_semaphore_trydown(int id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_SEMAPHORE_TRYDOWN, result, id);
    return result;
}

int syscall_semaphore_down (int id )
{
    int result;
    SYSCALL_1(SYSTEM_CALL_SEMAPHORE_DOWN, result, id);
    return result;
}

int syscall_semaphore_up(int id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_SEMAPHORE_UP, result, id);
    return result;
}

int syscall_semaphore_destroy(int id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_SEMAPHORE_DESTROY, result, id);
    return result;
}

uint32_t syscall_lookup(const char* name)
{
    uint32_t pid;
    SYSCALL_1(SYSTEM_CALL_LOOKUP, pid, name);
    return pid;
}

int syscall_lookup_main_thread(const char* name)
{
    int tid;
    SYSCALL_1(SYSTEM_CALL_LOOKUP_MAIN_THREAD, tid, name);
    return tid;
}

int syscall_get_vram_info(volatile ScreenInfo* info)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_GET_VRAM_INFO, result, info);
    return result;
}

int syscall_get_cursor(int* x, int* y)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_GET_CURSOR, result, x, y);
    return result;
}

int syscall_set_cursor(int x, int y)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_SET_CURSOR, result, x, y);
    return result;
}

uint32_t syscall_get_pid()
{
    uint32_t result;
    SYSCALL_0(SYSTEM_CALL_GET_PID, result);
    return result;
}

uint32_t syscall_get_tid()
{
    uint32_t result;
    SYSCALL_0(SYSTEM_CALL_GET_TID, result);
    return result;
}

int syscall_get_arg_count()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_ARGUMENTS_NUM, result);
    return result;
}

int syscall_get_arg(char* buf, int n)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_GET_ARGUMENTS, result, buf, n);
    return result;
}

int syscall_mthread_yield_message()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_MTHREAD_YIELD_MESSAGE, result);
    return result;
}

int syscall_get_date(KDate* date)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_DATE, result, date);
    return result;
}

int syscall_get_io()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_GET_IO, result);
    return result;
}

int syscall_exist_message()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_EXIST_MESSAGE, result);
    return result;
}

uint32_t syscall_memory_map_create(uint32_t size)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_MEMORY_MAP_CREATE, result, size);
    return result;
}

uint32_t syscall_memory_map_get_size(uint32_t id)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_MEMORY_MAP_GET_SIZE, result, id);
    return result;
}

int syscall_memory_map_map(uint32_t id, uint32_t address)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_MEMORY_MAP_MAP, result, id, address);
    return result;
}

int syscall_memory_map_unmap(uint32_t id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MEMORY_MAP_UNMAP, result, id);
    return result;
}

int syscall_set_ps_dump()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_PS_DUMP_SET, result);
    return result;
}

int syscall_read_ps_dump(PsInfo* info)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_PS_DUMP_READ, result, info);
    return result;
}

uint32_t syscall_get_tick()
{
    uint32_t result;
    SYSCALL_0(SYSTEM_CALL_GET_TICK, result);
    return result;
}

int syscall_get_kernel_version(char* buf, uint32_t size)
{
    uint32_t result;
    SYSCALL_2(SYSTEM_CALL_GET_KERNEL_VERSION, result, buf, size);
    return result;
}

int syscall_load_process_image(LoadProcessInfo* info)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_LOAD_PROCESS_IMAGE, result, info);
    return result;
}

int syscall_kill_thread(uint32_t tid)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_KILL_THREAD, result, tid);
    return result;
}

int syscall_clear_screen()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_CLEAR_SCREEN, result);
    return result;
}

int syscall_peek(MessageInfo* message, int index, int flags)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_PEEK, result, message, index, flags);
    return result;
}

int syscall_test(uint32_t laddress)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_TEST, result, laddress);
    return result;
}

int syscall_set_irq_receiver(int irq, int maskInterrupt)
{
    uint32_t result;
    SYSCALL_2(SYSTEM_CALL_SET_IRQ_RECEIVER, result, irq, maskInterrupt == SYS_MASK_INTERRUPT ? 1 : 0);
    return result;
}

int syscall_has_irq_receiver(int irq)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_HAS_IRQ_RECEIVER, result, irq);
    return result;
}

int syscall_remove_irq_receiver(int irq)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_REMOVE_IRQ_RECEIVER, result, irq);
    return result;
}

int syscall_free_pages(uint32_t address, uint32_t size)
{
    uint32_t result;
    SYSCALL_2(SYSTEM_CALL_FREE_PAGES, result, address, size);
    return result;
}

int syscall_get_memory_info(MemoryInfo* info)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_GET_MEMORY_INFO, result, info);
    return result;
}

uint8_t* syscall_allocate_dma_memory(int size)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_ALLOCATE_DMA_MEMORY, result, size);
    return (uint8_t*)result;
}

uint32_t syscall_deallocate_dma_memory(void* address, int size)
{
    uint32_t result;
    SYSCALL_2(SYSTEM_CALL_DEALLOCATE_DMA_MEMORY, result, address, size);
    return result;
}

int syscall_set_timer(uint32_t tick)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_SET_TIMER, result, tick);
    return result;
}

int syscall_kill_timer(uint32_t id)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_KILL_TIMER, result, id);
    return result;
}

int syscall_change_base_priority(uint32_t priority)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_CHANGE_BASE_PRIORITY, result, priority);
    return result;
}

int syscall_set_dll_segment_writable()
{
    uint32_t result;
    SYSCALL_0(SYSTEM_CALL_SET_DLL_SEGMENT_WRITABLE, result);
    return result;
}

int syscall_set_dll_segment_notshared(int index)
{
    uint32_t result;
    SYSCALL_1(SYSTEM_CALL_SET_DLL_SEGMENT_NOTSHARED, result, index);
    return result;
}

int syscall_shutdown(int op, int device)
{
    uint32_t result;
    SYSCALL_2(SYSTEM_CALL_SHUTDOWN, result, op, device);
    return result;
}

int syscall_log_print(const char* msg)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_LOG_PRINT, result, msg);
    return result;
}

int syscall_receive_packet(uint8_t* frame)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_RECEIVE_PACKET, result, frame);
    return result;
}

int syscall_send_packet(uint8_t* pkt, uint8_t* mac, uint32_t size, uint16_t pid)
{
    int result;
    SYSCALL_4(SYSTEM_CALL_SEND_PACKET, result, pkt, mac, size, pid);
    return result;
}

int syscall_set_watch_point(void* address, int flag)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_SET_WATCH_POINT, result, address, flag);
    return result;
}

int syscall_get_physical_address(uint32_t linearAddress)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_GET_PHYSICAL_ADDRESS, result, linearAddress);
    return result;
}

int syscall_remove_watch_point()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_REMOVE_WATCH_POINT, result);
    return result;
}

int syscall_allocate_contiguous(uint32_t laddress, int pageNum)
{
    uint32_t result;
    SYSCALL_2(SYSTEM_CALL_ALLOCATE_CONTIGUOUS, result, laddress, pageNum);
    return result;
}

void syscall_deallocate_contiguous(uint32_t laddress, int pageNum)
{
    uint32_t result;
    SYSCALL_2(SYSTEM_CALL_DEALLOCATE_CONTIGUOUS, result, laddress, pageNum);
    return;
}
