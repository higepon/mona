#include <monapi.h>
#include <monapi/messages.h>

using namespace MonAPI;

/*----------------------------------------------------------------------
    system call wrappers
----------------------------------------------------------------------*/
int sleep(dword ms) {

    dword tick = ms / 10;

    if (tick <= 0)
    {
        tick = 1;
    }

    return syscall_sleep(tick);
}

int set_timer(dword ms)
{
    dword tick = ms / 10;

    if (tick <= 0)
    {
        tick = 1;
    }
    return syscall_set_timer(tick);
}

int kill_timer(dword id)
{
    return syscall_kill_timer(id);
}

int print(const char* msg) {

#if 1  // temporary

    static dword tid = THREAD_UNKNOWN;
    if (tid == THREAD_UNKNOWN) tid = Message::lookupMainThread("PROCESS.BIN");

    char buf[128];

    buf[127] = '\0';

    for (int len = strlen(msg); len > 0; msg += 127, len -= 127)
    {
        strncpy(buf, msg, 127);
#if 0  /// DEBUG for message
        char buf2[128];

        sprintf(buf2, "{%d", syscall_get_tid());
        syscall_print(buf2);
#endif

        if (tid == THREAD_UNKNOWN || Message::sendReceive(NULL, tid + 1, MSG_PROCESS_STDOUT_DATA, 0, 0, 0, buf) != 0)
        {
            syscall_print(buf);
        }
#if 0  /// DEBUG for message
        syscall_print("}");
#endif
    }

    return 0;
#else
    return syscall_print(msg);
#endif
}

int kill() {
    return syscall_kill();
}

int exit(int error) {
    MonAPI::Message::send(monapi_get_server_thread_id(ID_PROCESS_SERVER),
        MSG_PROCESS_TERMINATED, MonAPI::System::getThreadID());
    return syscall_kill();
}

int mthread_create(dword f) {
    return syscall_mthread_create(f);
}

int mthread_join(dword id) {
    return syscall_mthread_join(id);
}

/*----------------------------------------------------------------------
    for C++
----------------------------------------------------------------------*/
int atexit( void (*func)(void)) {
    return -1;
}

void __cxa_pure_virtual() {
    print("__cxa_pure_virtual called\n");
}

void _pure_virtual() {
    print("_pure_virtual called\n");
}

void __pure_virtual() {
    print("_pure_virtual called\n");
}

/*----------------------------------------------------------------------
    printf
----------------------------------------------------------------------*/
#if 1
void printf(const char *format, ...) {

    char buf[128];
    int bufpos = 0;

    void** list = (void **)&format;

    list++;
    for (int i = 0; format[i] != '\0'; i++) {

        if (format[i] == '%') {
            if (bufpos > 0) {
                buf[bufpos] = '\0';
                print(buf);
                bufpos = 0;
            }
            i++;

            switch (format[i]) {
              case 's':
                  print((char *)*list);
                  list++;
                  break;
              case 'd':
                  putInt(*(int*)list, 10);
                  list++;
                  break;
              case 'x':
                  print("0x");
                  putInt(*(int*)list, 16);
                  list++;
                  break;
              case 'c':
                  putCharacter((char)*(int*)(list));
                  list++;
                  break;
              case '%':
                  putCharacter('%');
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
                print(buf);
                bufpos = 0;
            }
        }
    }

    if (bufpos > 0) {
        buf[bufpos] = '\0';
        print(buf);
    }
}
#endif

#if 0
void printf(const char *format, ...) {

    char str[512];
    str[0] = '\0';
    va_list args;
    int result;

    va_start(args, format);
    result = vsprintf(str, format, args);
    va_end(args);
    if(result > 512){
      /*buffer overflow*/
    }

    print(str);
}
#endif

void putInt(size_t n, int base) {

    static char buf[256];
    int geta;
    int num = n;

    if (base != 16) {

        if (n == 0) {

            putCharacter('0');
            return;
        }
        for (geta = 0; num; num /= 10, geta++);
        if ((int)n < 0) {
            geta++;
            base *= -1;
        }
    } else {
        geta = 8;
    }

    char* p = ltona(n, buf, geta, base);

    for (; *p != '\0'; p++) {
        putCharacter(*p);
    }
}

void putCharacter(char ch) {
    char* str = " ";
    str[0] = ch;
    print(str);
}

void printInt(int num) {

    if (num == 0)
    {
        print("0");
        return;
    }

    char revstr[20];
    char str[20];
    int  i = 0;
    int  j = 0;

    /* negative number */
    if (num < 0) {
        str[0] = '-';
        j++;
        num = num * -1;
    }

    /* number to buffer */
    do {
        revstr[i] = '0' + (int)(num % 10);
        num = num / 10;
        i++;
    } while (num != 0);
    revstr[i] = '\0';

    /* revert */
    for (; i >= 0; j++) {
        str[j] = revstr[i - 1];
        i--;
    }

    /* print */
    print(str);
    return;
}

size_t _power(size_t x, size_t y) {

    size_t result = x;

    for (size_t i = 1; i < y; i++) {
        result *= x;
    }
    return (int)result;
}

/*----------------------------------------------------------------------
    System call
----------------------------------------------------------------------*/
int syscall_mthread_create(dword f)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MTHREAD_CREATE, result, f);
    return result;
}

int syscall_mthread_join(dword id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MTHREAD_JOIN, result, id);
    return result;
}

int syscall_sleep(dword tick)
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

int syscall_load_process(const char* path, const char* name, CommandOption* list)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_LOAD_PROCESS, result, path, name, list);
    return result;
}

int syscall_kill()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_KILL, result);

    /* not reached */
    return result;
}

int syscall_send(dword pid, MessageInfo* message)
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

int syscall_mutex_create()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_MUTEX_CREATE, result);
    return result;
}

int syscall_mutex_trylock(int id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_TRYLOCK, result, id);
    return result;
}

int syscall_mutex_lock (int id )
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_LOCK, result, id);
    return result;
}

int syscall_mutex_unlock(int id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_UNLOCK, result, id);
    return result;
}

int syscall_mutex_destroy(int id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MUTEX_DESTROY, result, id);
    return result;
}

dword syscall_lookup(const char* name)
{
    dword pid;
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

int syscall_file_open(const char* path, int mode, volatile dword* size)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_FILE_OPEN, result, path, mode, size);
    return result;
}

int syscall_file_read(const char* path, dword size, dword* readSize)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_FILE_READ, result, path, size, readSize);
    return result;
}

int syscall_file_write(const char* path, dword size, dword* writeSize)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_FILE_WRITE, result, path, size, writeSize);
    return result;
}

int syscall_file_create(const char* path)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_FILE_CREATE, result, path);
    return result;
}

int syscall_file_close()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_FILE_CLOSE, result);
    return result;
}

int syscall_fdc_read(dword lba, byte* buffer, dword blocknum)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_FDC_READ, result, lba, buffer, blocknum);
    return result;
}

int syscall_fdc_write(dword lba, byte* buffer, dword blocknum)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_FDC_WRITE, result, lba, buffer, blocknum);
    return result;
}

int syscall_fdc_open()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_FDC_OPEN, result);
    return result;
}

int syscall_fdc_close()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_FDC_CLOSE, result);
    return result;
}

dword syscall_get_pid()
{
    dword result;
    SYSCALL_0(SYSTEM_CALL_GET_PID, result);
    return result;
}

dword syscall_get_tid()
{
    dword result;
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

int syscall_fdc_disk_changed()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_FDC_DISK_CHANGED, result);
    return result;
}

dword syscall_memory_map_create(dword size)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_MEMORY_MAP_CREATE, result, size);
    return result;
}

dword syscall_memory_map_get_size(dword id)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_MEMORY_MAP_GET_SIZE, result, id);
    return result;
}

int syscall_memory_map_map(dword id, dword address)
{
    int result;
    SYSCALL_2(SYSTEM_CALL_MEMORY_MAP_MAP, result, id, address);
    return result;
}

int syscall_memory_map_unmap(dword id)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_MEMORY_MAP_UNMAP, result, id);
    return result;
}

int syscall_dir_open()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_DIR_OPEN, result);
    return result;
}

int syscall_dir_read(const char* name, int* size, int* attr)
{
    int result;
    SYSCALL_3(SYSTEM_CALL_DIR_READ, result, name, size, attr);
    return result;
}

int syscall_dir_close()
{
    int result;
    SYSCALL_0(SYSTEM_CALL_DIR_CLOSE, result);
    return result;
}

int syscall_cd(const char* path)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_CD, result, path);
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

dword syscall_get_tick()
{
    dword result;
    SYSCALL_0(SYSTEM_CALL_GET_TICK, result);
    return result;
}

dword syscall_file_position()
{
    dword result;
    SYSCALL_0(SYSTEM_CALL_FILE_POSITION, result);
    return result;
}

dword syscall_file_seek(dword pt, int flag)
{
    dword result;
    SYSCALL_2(SYSTEM_CALL_FILE_SEEK, result, pt, flag);
    return result;
}

int syscall_get_kernel_version(char* buf, dword size)
{
    dword result;
    SYSCALL_2(SYSTEM_CALL_GET_KERNEL_VERSION, result, buf, size);
    return result;
}

int syscall_load_process_image(LoadProcessInfo* info)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_LOAD_PROCESS_IMAGE, result, info);
    return result;
}

int syscall_kill_thread(dword tid)
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

int syscall_test(dword laddress)
{
    int result;
    SYSCALL_1(SYSTEM_CALL_TEST, result, laddress);
    return result;
}

int syscall_set_irq_receiver(int irq)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_SET_IRQ_RECEIVER, result, irq);
    return result;
}

int syscall_remove_irq_receiver(int irq)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_REMOVE_IRQ_RECEIVER, result, irq);
    return result;
}

int syscall_free_pages(dword address, dword size)
{
    dword result;
    SYSCALL_2(SYSTEM_CALL_FRRE_PAGES, result, address, size);
    return result;
}

int syscall_get_memory_info(MemoryInfo* info)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_GET_MEMORY_INFO, result, info);
    return result;
}

byte* syscall_allocate_dma_memory()
{
    dword result;
    SYSCALL_0(SYSTEM_CALL_ALLOCATE_DMA_MEMORY, result);
    return (byte*)result;
}

dword syscall_deallocate_dma_memory(void* address)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_DEALLOCATE_DMA_MEMORY, result, address);
    return result;
}

int syscall_set_timer(dword tick)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_SET_TIMER, result, tick);
    return result;
}

int syscall_kill_timer(dword id)
{
    dword result;
    SYSCALL_1(SYSTEM_CALL_KILL_TIMER, result, id);
    return result;
}
