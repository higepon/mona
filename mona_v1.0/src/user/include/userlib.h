#ifndef _MONA_USERLIB_
#define _MONA_USERLIB_

#include <types.h>
#include <string.h>
#include <collection.h>
#include <MemoryManager.h>

#define SYSTEM_CALL_PROCESS_SLEEP   5
#define SYSTEM_CALL_HEAVEY          6
#define SYSTEM_CALL_PRINT           7
#define SYSTEM_CALL_KILL            8
#define SYSTEM_CALL_PUT_PIXEL       9
#define SYSTEM_CALL_SEND            10
#define SYSTEM_CALL_RECEIVE         11
#define SYSTEM_CALL_MTHREAD_CREATE  12
#define SYSTEM_CALL_MTHREAD_JOIN    13
#define SYSTEM_CALL_MUTEX_CREATE    14
#define SYSTEM_CALL_MUTEX_LOCK      15
#define SYSTEM_CALL_MUTEX_TRYLOCK   16
#define SYSTEM_CALL_MUTEX_UNLOCK    17
#define SYSTEM_CALL_MUTEX_DESTROY   18
#define SYSTEM_CALL_LOOKUP          19
#define SYSTEM_CALL_GET_VRAM_INFO   20
#define SYSTEM_CALL_LOAD_PROCESS    21
#define SYSTEM_CALL_MAP             22
#define SYSTEM_CALL_SET_CURSOR      23
#define SYSTEM_CALL_GET_CURSOR      24
#define SYSTEM_CALL_FILE_OPEN       25
#define SYSTEM_CALL_FILE_READ       26
#define SYSTEM_CALL_FILE_CLOSE      27
#define SYSTEM_CALL_MAP_TWO         28
#define SYSTEM_CALL_FDC_OPEN        29
#define SYSTEM_CALL_FDC_CLOSE       30
#define SYSTEM_CALL_FDC_READ        31
#define SYSTEM_CALL_FDC_WRITE       32
#define SYSTEM_CALL_UNMAP_TWO       33
#define SYSTEM_CALL_GET_PID         34
#define SYSTEM_CALL_ARGUMENTS_NUM   35
#define SYSTEM_CALL_GET_ARGUMENTS   36
#define SYSTEM_CALL_MTHREAD_YIELD_M 37
#define SYSTEM_CALL_DATE            38
#define SYSTEM_CALL_TEST            99

#define interface class
#define MESSAGE_LOOP messageLoop

extern "C" int MonaMain(List<char*>* pekoe);
extern "C" int user_start();
extern "C" int sleep(dword tick);
extern "C" int heavy();
extern "C" int print(const char*);
extern "C" int kill();
extern "C" int exit(int error);
extern "C" int _put_pixel(int x, int y, char color);
extern "C" int mthread_create(dword f);
extern "C" int mthread_join(dword id);
extern "C" int syscall_test();
extern "C" int syscall_sleep(dword tick);
extern "C" int syscall_heavy();
extern "C" int syscall_print(const char*);
extern "C" int syscall_kill();
extern "C" int syscall_put_pixel(int x, int y, char color);
extern "C" int syscall_send(dword pid, MessageInfo* message);
extern "C" int syscall_receive(MessageInfo* message);
extern "C" int syscall_mthread_create(dword f);
extern "C" int syscall_mthread_join(dword id);
extern "C" int syscall_mutex_create();
extern "C" int syscall_mutex_trylock(int id);
extern "C" int syscall_mutex_lock (int id );
extern "C" int syscall_mutex_unlock(int id);
extern "C" int syscall_get_vram_info(volatile ScreenInfo* info);
extern "C" int syscall_load_process(const char* name, CommandOption* list);
extern "C" int syscall_get_cursor(int* x, int* y);
extern "C" int syscall_set_cursor(int x, int y);
extern "C" int syscall_mutex_destroy(int id);
extern "C" int syscall_map(dword pid, dword sharedId, dword linearAddress, dword size);
extern "C" int syscall_file_open(char* path, char* file, dword* size);
extern "C" int syscall_file_read(char* buf, dword size, dword* readSize);
extern "C" int syscall_file_close();
extern "C" int syscall_fdc_open();
extern "C" int syscall_fdc_close();
extern "C" int syscall_fdc_read(dword lba, byte* buffer, dword blocknum);
extern "C" int syscall_fdc_write(dword lba, byte* buffer, dword blocknum);
extern "C" int syscall_map2(MappingInfo* info);
extern "C" int syscall_unmap2(dword sharedId);
extern "C" int syscall_get_pid();
extern "C" int syscall_get_arg_count();
extern "C" int syscall_get_arg(char* buf, int n);
extern "C" int syscall_mthread_yeild_message();
extern "C" int syscall_get_date(KDate* date);
extern "C" void* malloc(unsigned long size);
extern "C" void free(void * address);
extern "C" void __cxa_pure_virtual();
extern "C" void _pure_virtual(void);
extern "C" void __pure_virtual(void);
extern "C" int atexit( void (*func)(void));
extern "C" dword syscall_lookup(const char* name);
extern "C" void setupArguments(List<char*>* arg);

byte inportb(dword);
void outportb(dword, byte);
void* operator new(size_t size);
void  operator delete(void* address);

class MonaApplication;
extern MonaApplication* monaApp;

/*----------------------------------------------------------------------
    Observable/Observer
----------------------------------------------------------------------*/
class Observable;
interface Observer;
interface Observer {

  public:
    virtual void update(Observable* o, void* arg) = 0;
};

class Observable {

  public:
    Observable() : changed(false) {}
    virtual ~Observable(){}

  public:
    virtual void addObserver(Observer* o);
    virtual void notifyObservers();
    virtual void notifyObservers(void* arg);
    virtual void deleteObservers();
    virtual void setChanged();
    virtual void clearChanged();
    virtual bool hasChanged() const;
    virtual int countObservers();

  protected:
    HList<Observer*> observers;
    bool changed;
};

/*----------------------------------------------------------------------
    Date
----------------------------------------------------------------------*/
class Date {

  public:
    Date() {
        syscall_get_date(&date);
    }

    ~Date() {}

  public:
    inline int year() const {
        return date.year;
    }

    inline int month() const {
        return date.month;
    }

    inline int day() const {
        return date.day;
    }

    inline int hour() const {
        return date.hour;
    }

    inline int min() const {
        return date.min;
    }

    inline int sec() const {
        return date.sec;
    }

    inline int dayofweek() const {
        return date.dayofweek;
    }

    inline void refresh() {
        syscall_get_date(&date);
    }

  private:
    KDate date;

};

/*----------------------------------------------------------------------
    System
----------------------------------------------------------------------*/
class System {
  public:
    static inline dword getPID() {
        return syscall_get_pid();
    }
};

/*----------------------------------------------------------------------
    Device
----------------------------------------------------------------------*/
interface Device {

  public:
    virtual int open()         = 0;
    virtual int close()        = 0;
    virtual int ioctl(void* p) = 0;
};

/*----------------------------------------------------------------------
    StorageDevice
----------------------------------------------------------------------*/
interface StorageDevice : public Device {

  public:
    virtual dword getBlockSize() const = 0;
    virtual int read(dword lba,  byte* buf, dword blocknum)  = 0;
    virtual int write(dword lba, byte* buf, dword blocknum)  = 0;
};

/*----------------------------------------------------------------------
    Floppy
----------------------------------------------------------------------*/
class Floppy : public StorageDevice {

  public:
    Floppy(int device);
    virtual ~Floppy();

  public:
    virtual int open();
    virtual int close();
    virtual int read(dword lba,  byte* buf, dword blocknum);
    virtual int write(dword lba, byte* buf, dword blocknum);
    virtual int ioctl(void* p);

    inline virtual dword getBlockSize() const {
        return BLOCK_SIZE;
    }

  public:
    static const dword BLOCK_SIZE = 512;
    static const int FLOPPY_1     = 0;
};

/*----------------------------------------------------------------------
    Receiver
----------------------------------------------------------------------*/
interface Receiver {
  public:
    virtual void onKeyDown(int keycode, int modifiers) = 0;
};

/*----------------------------------------------------------------------
    MonaApplication
----------------------------------------------------------------------*/
class MonaApplication : public Receiver {

  public:
    MonaApplication(char* name);
    virtual ~MonaApplication();

  public:
    virtual int main(List<char*>* pekoe) = 0;

    /* default implementation */
    virtual void onKeyDown(int keycode, int modifiers) {
    }

    virtual inline dword getMyPID() {
        return mypid_;
    }

  protected:
    dword mypid_;
};

/*----------------------------------------------------------------------
    InputStream
----------------------------------------------------------------------*/
interface InputStream {

  public:
    virtual int open()   = 0;
    virtual void close() = 0;
    virtual int read(byte* buf, int size) = 0;
    virtual dword getReadSize() const = 0;
};

class FileInputStream : public InputStream {

  public:
    FileInputStream(char* file);
    virtual ~FileInputStream();

  public:
    virtual int open();
    virtual void close();
    virtual int read(byte* buf, int size);
    virtual dword getReadSize() const;
    virtual dword getFileSize() const;

  protected:
    char* file_;
    dword readSize_;
    dword fileSize_;
    bool isOpen_;
};

/*----------------------------------------------------------------------
    MemoryMap
----------------------------------------------------------------------*/
class MemoryMap {

  private:
    MemoryMap(){};
    virtual ~MemoryMap(){};

    public:
    inline static MemoryMap* create() {
        static MemoryMap map;
        return &map;
    }

    inline dword map(dword pid, dword linearAddress, dword linearAddress2, dword size) {

        if (linearAddress < 0x90000000 || linearAddress >=0xA0000000) {
            info_.errorCd = 4;
            return 0;
        }

        if (linearAddress2 < 0x90000000 || linearAddress2 >=0xA0000000) {
            info_.errorCd = 4;
            return 0;
        }

        info_.attachPid = pid;
        info_.linearAddress1 = linearAddress;
        info_.linearAddress2 = linearAddress2;
        info_.size           = size;

        return syscall_map2(&info_);
    }

    inline int unmap(dword sharedId) {
        return syscall_unmap2(sharedId);
    }

    inline dword getLastErrorCode() const {
        return info_.errorCd;
    }

  private:
    MappingInfo info_;
};

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
    static void create(MessageInfo* info, dword header, dword arg1, dword arg2, dword arg3, char* str);
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
    Raster
----------------------------------------------------------------------*/
class Raster {
  public:
    static const int XOR = 1;
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
    static bool bitblt(Screen* destScreen, int destX, int destY, int width, int height
                , Screen* sourceScreen, int sourceX, int sourceY);
    static bool bitblt(Screen* destScreen, int destX, int destY, int width, int height
                , Screen* sourceScreen, int sourceX, int sourceY, int raster);

    inline byte* getVRAM() const {
        return vram_;
    }

    inline int getBpp() const {
        return bpp_;
    }

    inline int getWidth() const {
        return xResolution_;
    }

    inline int getHeight() const {
        return yResolution_;
    }

   static inline void copyPixel16(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel24(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

    static inline void copyPixel32(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;
        dpixel[destX + destY * xResolution] = spixel[sourceX + sourceY * xResolution];
    }

   static inline void copyPixel16XOR(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel16* dpixel = (Pixel16*)dvram;
        Pixel16* spixel = (Pixel16*)svram;

        dword dest  = destX + destY * xResolution;
        dword source = sourceX + sourceY * xResolution;

        dpixel[dest].p[0] = (dpixel[dest].p[0]) ^ (spixel[source].p[0]);
        dpixel[dest].p[1] = (dpixel[dest].p[1]) ^ (spixel[source].p[1]);
    }

    static inline void copyPixel24XOR(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel24* dpixel = (Pixel24*)dvram;
        Pixel24* spixel = (Pixel24*)svram;

        dword dest  = destX + destY * xResolution;
        dword source = sourceX + sourceY * xResolution;

        dpixel[dest].p[0] = (dpixel[dest].p[0]) ^ (spixel[source].p[0]);
        dpixel[dest].p[1] = (dpixel[dest].p[1]) ^ (spixel[source].p[1]);
        dpixel[dest].p[2] = (dpixel[dest].p[2]) ^ (spixel[source].p[2]);
    }

    static inline void copyPixel32XOR(byte* dvram, int destX, int destY, byte* svram, int sourceX, int sourceY, int xResolution) {
        Pixel32* dpixel = (Pixel32*)dvram;
        Pixel32* spixel = (Pixel32*)svram;

        dword dest  = destX + destY * xResolution;
        dword source = sourceX + sourceY * xResolution;

        dpixel[dest].p = (dpixel[dest].p) ^ (spixel[source].p);
    }

  public:

  protected:
    byte* vram_;
    int bpp_;
    int xResolution_;
    int yResolution_;
};

class VirtualScreen : public Screen {

  public:
    VirtualScreen();
    VirtualScreen(dword vramsize);
    virtual ~VirtualScreen();

  protected:
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
