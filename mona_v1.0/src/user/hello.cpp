#include <userlib.h>
#include <KeyBoardManager.h>
#include <jpegls.h>

int listener();
int disp();
int draw();
static char buf[32];
static bool come;
static Mutex* mutex;

int main() {

    dword id;
    mutex = new Mutex();

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

    if (!(id = mthread_create((dword)draw))) {
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

//#include "text.hex"

int draw() {

    Screen screen;
    printf("user mode screen (x, y) = (%x, %x)\n", screen.getXResolution(), screen.getYResolution());
    printf("you can draw from user mode!! and also use multi thread \n");

    /* nikq code */
//     CJPEGLS *jpeg = new CJPEGLS;

//     int w,h;
//     jpeg->Open(test_jpeg,sizeof(test_jpeg)-1);
//     jpeg->GetInfo(&w,&h);
//     byte *picture = new byte[w * h * 3];
//     jpeg->Decode(picture);

//     int x,y;
//     int vesaWidth = screen.getXResolution();
//     int vesaBpp = screen.getBpp();
//     byte* vesaVram = screen.getVRAM();

//     for(y=0;y<h;y++){
//         for(x=0;x<w;x++){
//             int k =(x+(y*vesaWidth))*vesaBpp/8;
//             int k2=(x+(y*w))*3;
//             /*                picture[k2]=0xFF;
//                               picture[k2+1]=0xFF;
//                               picture[k2+2]=0xFF;*/
//             vesaVram[k]   = picture[k2];
//             vesaVram[k+1] = picture[k2+1];
//             vesaVram[k+2] = picture[k2+2];
//         }
//     }
//     for (;;);

    word color = Color::bpp24to565(Color::rgb(0x7A, 0x7A, 0xC4));

    screen.fillRect16(0, 200, 800, 350, color);

    color = Color::bpp24to565(Color::rgb(0x63, 0x63, 0x9F));

    for (int x = 0; x < screen.getXResolution(); x++) {
        screen.putPixel16(x, 0.5 * x + 300, color);
    }
    for (;;);
}
