#ifndef _MONA_USERLIB_
#define _MONA_USERLIB_
#include <types.h>
#include <string.h>

#define SYSTEM_CALL_PROCESS_SLEEP  5
#define SYSTEM_CALL_HEAVEY         6
#define SYSTEM_CALL_PRINT          7
#define SYSTEM_CALL_KILL           8
#define SYSTEM_CALL_PUT_PIXEL      9
#define SYSTEM_CALL_SEND           10
#define SYSTEM_CALL_RECEIVE        11
#define SYSTEM_CALL_MTHREAD_CREATE 12
#define SYSTEM_CALL_MTHREAD_JOIN   13
#define SYSTEM_CALL_MUTEX_CREATE   14
#define SYSTEM_CALL_MUTEX_LOCK     15
#define SYSTEM_CALL_MUTEX_TRYLOCK  16
#define SYSTEM_CALL_MUTEX_UNLOCK   17
#define SYSTEM_CALL_MUTEX_DESTROY  18
#define SYSTEM_CALL_LOOKUP         19
#define SYSTEM_CALL_GET_VRAM_INFO  20
#define SYSTEM_CALL_LOAD_PROCESS   21
#define SYSTEM_CALL_MAP            22
#define SYSTEM_CALL_SET_CURSOR     23
#define SYSTEM_CALL_GET_CURSOR     24

#define main() monamain()

extern "C" int user_start();

int sleep(dword tick);
int heavy();
int print(const char*);
int kill();
int exit(int error);
int _put_pixel(int x, int y, char color);
int mthread_create(dword f);
int mthread_join(dword id);

int syscall_sleep(dword tick);
int syscall_heavy();
int syscall_print(const char*);
int syscall_kill();
int syscall_put_pixel(int x, int y, char color);
int syscall_send(dword pid, MessageInfo* message);
int syscall_receive(MessageInfo* message);
int syscall_mthread_create(dword f);
int syscall_mthread_join(dword id);
int syscall_mutex_create();
int syscall_mutex_trylock();
int syscall_mutex_lock();
int syscall_mutex_unlock();
int syscall_get_vram_info(ScreenInfo* info);
int syscall_load_process(const char* name);
int syscall_get_cursor(int* x, int* y);
int syscall_set_cursor(int x, int y);
dword syscall_lookup(const char* name);
int syscall_mutex_destroy(int id);
int syscall_map(dword pid, dword sharedId, dword linearAddress, dword size);
void* malloc(unsigned long size);
void free(void * address);

void* operator new(size_t size);
void  operator delete(void* address);

/*----------------------------------------------------------------------
    Server
----------------------------------------------------------------------*/
class Server {

  public:
    virtual void service() = 0;

};

/*----------------------------------------------------------------------
    Mutex
----------------------------------------------------------------------*/
class Mutex {

  public:
    Mutex();
    ~Mutex();
    int init();
    int lock();
    int unlock();
    int tryLock();
    int destory();

  public:
    inline int getId() const {
        return mutexId_;
    }

  private:
    int mutexId_;
};

/*----------------------------------------------------------------------
    Message
----------------------------------------------------------------------*/
class Message {

  private:
    Message();
    virtual ~Message();

  public:
    static int send(dword pid, MessageInfo* info);
    static int receive(MessageInfo* info);
    static dword lookup(const char* name);
};

/*----------------------------------------------------------------------
    Color
----------------------------------------------------------------------*/
class Color {

  public:

    inline static word bpp24to565(dword color) {
        word result = (word)(((color >> 8) & 0xF800) | ((color >> 5) & 0x07E0) | ((color >> 3) & 0x001F));
        return result;
    }

    inline static dword rgb(byte r, byte g, byte b) {
        return b | g  <<  8 | r  << 16;
    }
};

/*----------------------------------------------------------------------
    Screen
----------------------------------------------------------------------*/
class Screen {

  public:
    Screen();
    virtual ~Screen();


  public:

    void putPixel16(int x, int y, dword color);
    void fillRect16(int x, int y, int w, int h, dword color);

    inline byte* getVRAM() const {
        return vram_;
    }

    inline int getBpp() const {
        return bpp_;
    }

    inline int getXResolution() const {
        return xResolution_;
    }

    inline int getYResolution() const {
        return yResolution_;
    }

  public:

  protected:
    byte* vram_;
    int bpp_;
    int xResolution_;
    int yResolution_;

  private:
    ScreenInfo sinfo;
};


void putCharacter(char ch);
void putInt(size_t n, int base);
void printf(const char *format, ...);
void printInt(int num);
size_t _power(size_t x, size_t y);

/* key info */
#define VK_SPACE      0x20
#define VK_BACKSPACE  0x21
#define VK_ENTER      0x22
#define VK_PERIOD     0x30
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

#define KEY_MODIFIER_DOWN  0x1
#define KEY_MODIFIER_UP    0x2
#define KEY_MODIFIER_SHIFT 0x4
#define KEY_MODIFIER_ALT   0x8
#define KEY_MODIFIER_CTRL  0x10
#define KEY_MODIFIER_WIN   0x20
#define KEY_MODIFIER_MENU  0x40

#endif
