#include <userlib.h>
#include <KeyBoardManager.h>

int listener();
int disp();
static char buf[32];
static bool come;
static Mutex* mutex;

class Color {

  public:

    inline static byte bpp24to8(dword color) {
        return color;
    }

    inline static word bpp24to1555(dword color) {
        return color;
    }
};

class Screen {

  public:
    Screen();
    virtual ~Screen();


  public:

    bool fillRect(int x, int y, int width, int height, dword color);

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

Screen::Screen() {

    /* get and set vram information */
    syscall_get_vram_info(&sinfo);
    vram_        = (byte*)sinfo.vram;
    bpp_         = sinfo.bpp;
    xResolution_ = sinfo.x;
    yResolution_ = sinfo.y;
}

Screen::~Screen() {
}

bool Screen::fillRect(int x, int y, int width, int height, dword color) {
    return false;
}

int main() {

    dword id;
    mutex = new Mutex();

    Screen screen;

    printf("get x = %x", screen.getXResolution());

    /* look up */
    dword myPid   = Message::lookup("USER.ELF");
    dword destPid = Message::lookup("KEYBDMNG.SVR");
    if (destPid == 0) {
        printf("process KEYBDMNG.SVR not found\n");
        for (;;);
    }

    /* create message for KEYBDMNG.SVR */
    MessageInfo info;
    info.header = MSG_KEY_REGIST_TO_SERVER;
    info.arg1   = myPid;

    /* send */
    if (Message::send(destPid, &info)) {
        printf("regist error\n");
    }

    if (mutex->init()) {
        print("mutex init errror\n");
        exit(-1);
    }

    if (!(id = mthread_create((dword)listener))) {
        print("mthread create error\n");
        exit(-1);
    }

    if (mthread_join(id)) {
        print("mthread join error\n");
        exit(-1);
    }

    disp();
    return 0;
}

int disp() {

    for (;;) {
        if (come) {
            mutex->lock();
            print(buf);
            buf[0] = '0';
            come = false;
            mutex->unlock();
        }
    }
}

int listener() {

    MessageInfo message;

    for (;;) {
        if (!Message::receive(&message)) {
            mutex->lock();
            buf[0] = '<';
            buf[1] = (char)message.arg1;
            buf[2] = '>';
            buf[3] = '\0';
            come = true;
            mutex->unlock();
        }
    }
}
