#ifndef _MONA_USERLIB_SYSCALL_H_
#define _MONA_USERLIB_SYSCALL_H_

#define MESSAGE_LOOP MonAPI::messageLoop

extern "C" int MonaMain(List<char*>* pekoe);
extern "C" int user_start();
extern "C" int sleep(dword ms);
extern "C" int heavy();
extern "C" int print(const char*);
extern "C" int kill();
extern "C" int exit(int error);
extern "C" int mthread_create(dword f);
extern "C" int mthread_join(dword id);
extern "C" int syscall_test();
extern "C" int syscall_sleep(dword tick);
extern "C" int syscall_print(const char*);
extern "C" int syscall_kill();
extern "C" int syscall_send(dword id, MessageInfo* message);
extern "C" int syscall_receive(MessageInfo* message);
extern "C" int syscall_exist_message();
extern "C" int syscall_mthread_create(dword f);
extern "C" int syscall_mthread_join(dword id);
extern "C" int syscall_mutex_create();
extern "C" int syscall_mutex_trylock(int id);
extern "C" int syscall_mutex_lock (int id );
extern "C" int syscall_mutex_unlock(int id);
extern "C" int syscall_get_vram_info(volatile ScreenInfo* info);
extern "C" int syscall_load_process(const char* path, const char* name, CommandOption* list);
extern "C" int syscall_get_cursor(int* x, int* y);
extern "C" int syscall_set_cursor(int x, int y);
extern "C" int syscall_mutex_destroy(int id);
extern "C" int syscall_map(dword pid, dword sharedId, dword linearAddress, dword size);
extern "C" int syscall_file_open(const char* path, int mode, volatile dword* size);
extern "C" int syscall_file_read(const char* path, dword size, dword* readSize);
extern "C" int syscall_file_write(const char* path, dword size, dword* writeSize);
extern "C" int syscall_file_close();
extern "C" int syscall_file_create(const char* path);
extern "C" int syscall_fdc_open();
extern "C" int syscall_fdc_close();
extern "C" int syscall_fdc_read(dword lba, byte* buffer, dword blocknum);
extern "C" int syscall_fdc_write(dword lba, byte* buffer, dword blocknum);
extern "C" int syscall_fdc_disk_changed();
extern "C" int syscall_map2(MappingInfo* info);
extern "C" int syscall_unmap2(dword sharedId);
extern "C" int syscall_lookup_main_thread(const char* name);
extern "C" dword syscall_get_pid();
extern "C" dword syscall_get_tid();
extern "C" int syscall_get_arg_count();
extern "C" int syscall_get_arg(char* buf, int n);
extern "C" int syscall_mthread_yeild_message();
extern "C" int syscall_get_date(KDate* date);
extern "C" int syscall_get_io();
extern "C" int syscall_dir_open();
extern "C" int syscall_dir_read(const char* name, int* size);
extern "C" int syscall_dir_close();
extern "C" int syscall_cd(const char* path);
extern "C" int syscall_set_ps_dump();
extern "C" int syscall_read_ps_dump(PsInfo* info);
extern "C" dword syscall_file_position();
extern "C" dword syscall_file_seek(dword pt, int flag);

extern "C" dword syscall_memory_map_create(dword size);
extern "C" dword syscall_memory_map_get_size(dword id);
extern "C" int syscall_memory_map_map(dword id, dword address);
extern "C" int syscall_memory_map_unmap(dword id);

extern "C" dword syscall_lookup(const char* name);
extern "C" dword syscall_get_tick();
extern "C" int syscall_get_kernel_version(char* buf, dword size);

extern "C" void* malloc(unsigned long size);
extern "C" void free(void * address);
extern "C" void __cxa_pure_virtual();
extern "C" void _pure_virtual(void);
extern "C" void __pure_virtual(void);
extern "C" int atexit( void (*func)(void));
extern "C" void setupArguments(List<char*>* arg);

void putCharacter(char ch);
void putInt(size_t n, int base);
void printf(const char *format, ...);
void printInt(int num);
size_t _power(size_t x, size_t y);

byte inp8(dword port);
void outp8(dword port, byte value);
word inp16(dword port);
void outp16(dword port, word value);
dword inp32(dword port);
void outp32(dword port, dword value);

void* operator new(size_t size);
void  operator delete(void* address);

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
