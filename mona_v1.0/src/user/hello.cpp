#include <userlib.h>
#include <string.h>
#include <collection.h>

class myApplication : public MonaApplication {

  public:
    myApplication(char* name);
    virtual ~myApplication() {}

  public:
    virtual int main(List<char*>* pekoe);
    virtual void onKeyDown(int keycode, int modifiers);
};

myApplication::myApplication(char* name) : MonaApplication(name) {
}

int myApplication::main(List<char*>* pekoe) {

    /* arguments */
    printf("\nargument list\n");
    for (int i = 0; i < pekoe->size(); i++) {
        printf("[%s]\n", pekoe->get(i));
    }

    Date dt;

    printf("%d/%d/%d %d:%d:%d", dt.year(), dt.month(), dt.day(), dt.hour(), dt.min(), dt.sec());


    /* draw rectangle */
    Screen screen;
    //    screen.fillRect16(10, 10, 40, 50, Color::rgb(105, 141, 148));
    screen.fillRect16(10, 10, 40, 50, Color::rgb(0x89, 0x67, 0x81));

    /* memory share between this process and SHELL.SVR */
    dword pid = Message::lookup("SHELL.SVR");
    MemoryMap* mm = MemoryMap::create();
    dword sharedId = mm->map(pid, 0x90000000, 0x90005000, 4096);
    strcpy((char*)0x90005000, "data share Mona");
    mm->unmap(sharedId);


    screen.circle16(50, 50, 20, Color::rgb(0x89, 0x67, 0x81));

    screen.fillCircle16(100, 100, 20, Color::rgb(0x10, 0x67, 0x81));

    /* unmap? */
    //printf("%s", (char*)0x90005000);

    /* floppy read/write test */
    char buf[1024];
    memset(buf      , (byte)0x12, 512);
    memset(buf + 512, (byte)0x34, 512);

    StorageDevice* device = new Floppy(Floppy::FLOPPY_1);

    printf("device block size=%d\n", device->getBlockSize());

    device->open();
    device->write(5, (byte*)buf, 2);

    memset(buf, (byte)0x00, 1024);

    device->read(5, (byte*)buf, 2);
    device->close();
    delete device;

    for (int i = 0; i < 10; i++) {
        printf("[%x]", buf[i]);
    }
    printf("\n--------------------------\n");

    for (int i = 0; i < 10; i++) {
        printf("[%x]", buf[512 + i]);
    }

    Screen* virtualScreen = new VirtualScreen();
    virtualScreen->fillRect16(50, 50, 40, 40, Color::rgb(0x12, 0x34, 0x56));
    printf("%s", Screen::bitblt(&screen, 0, 0, 40, 40, virtualScreen, 50, 50) ? "OK" : "NG");

    return 0;
}

void myApplication::onKeyDown(int keycode, int modifiers) {
    printf("down\n");
}

int MonaMain(List<char*>* pekoe) {

    monaApp = new myApplication("HELLO.ELF");
    monaApp->main(pekoe);

//     byte buf[512];
//     memset(buf, 0, 512);

//     InputStream* stream = new FileInputStream("HELLO.ELF");
//     stream->open();
//     stream->read(buf, 510);
//     stream->close();


//     for (int i = 0; i < 50; i++) {
//         printf("[%x]", (int)(buf[i]));
//     }

//     dword mypid = Message::lookup("HELLO.ELF");
//     dword pid   = Message::lookup("MAP.SVR");

//     MessageInfo message;
//     message.header = MSG_MAP;
//     message.arg1   = mypid;
//     message.arg2   = 0x20000000;
//     message.from   = mypid;
//     strcpy(message.str, "mona://device/fd/buffer");

//     outportb(0x3f2, 0x1c);

//     if (Message::send(pid, &message)) {
//         printf("send error\n");
//     }

//     for (;;) {
//         /* receive */
//         if (!Message::receive(&message)) {

//             switch(message.header) {

//             case MSG_RESULT_OK:
//                 printf("contents=%s", (char*)0x20000000);
//                 break;

//             default:
//                 /* igonore this message */
//                 break;
//             }
//         }
//     }

    return 0;
}


/* "test044a.c" : 32bit-gbox, fast */
/*  stack:4k malloc:304k   */
/*  動けばいいんだ、動けば。 */


// #include <userlib.h>

// /* ----------- these are from Noiz2sa by Kenta Cho ------------ */

// #define BOARD_MAX 256
// #define NOT_EXIST -1
// #define FPS       30
// #define COLOR(R,G,B) ((R)<<16|(G)<<8|(B))

// static int bdIdx;
// static int boardMx, boardMy;
// static int boardRepx, boardRepy;
// static int boardRepXn, boardRepYn;

// int SCREEN_W,SCREEN_H;

// typedef struct {
//   int x, y, z;
//   int width, height, color;
// } Board;
// static Board board[BOARD_MAX];

// void initBackground() {
//     int i;
//     for ( i=0 ; i<BOARD_MAX ; i++ ) {
//         board[i].width = NOT_EXIST;
//     }
// }

// static void addBoard(int x, int y, int z, int width, int height) {
//     if ( bdIdx >= BOARD_MAX ) return;
//     board[bdIdx].x = x;
//     board[bdIdx].y = y;
//     board[bdIdx].z = z;
//     board[bdIdx].width = width/z;
//     board[bdIdx].height = height/z;
//     board[bdIdx].color = COLOR(0x99 * 256 / (1 + z), 0xAA * 256 / (1 + z), 0xDD);
//     bdIdx++;
// }

// void setStageBackground(int bn) {
//     int i, j, k;
//     bdIdx = 0;

//     for(i=0;i<BOARD_MAX;i++)
//         board[i].width = NOT_EXIST;

//     switch ( bn%6 ) {
//     case 0:
//         addBoard(9000, 9000, 500, 25000, 25000);
//         for ( i=0 ; i<4 ; i++ ) {
//             for ( j=0 ; j<4 ; j++ ) {
//                 if ( i > 1 || j > 1 ) {
//                     addBoard(i*16384, j*16384, 500, 10000+(i*12345)%3000, 10000+(j*54321)%3000);
//                 }
//             }
//         }
//         for ( j=0 ; j<8 ; j++ ) {
//             for ( i=0 ; i<4 ; i++ ) {
//                 addBoard(0, i*16384, 500-j*50, 20000-j*1000, 12000-j*500);
//             }
//         }
//         for ( i=0 ; i<8 ; i++ ) {
//             addBoard(0, i*8192, 100, 20000, 6400);
//         }
//         boardMx = 40; boardMy = 300;
//         boardRepx = boardRepy = 65536;
//         boardRepXn = boardRepYn = 4;
//         break;

//     case 1:
//         addBoard(12000, 12000, 400, 48000, 48000);
//         addBoard(12000, 44000, 400, 48000, 8000);
//         addBoard(44000, 12000, 400, 8000, 48000);
//         for ( i=0 ; i<16 ; i++ ) {
//             addBoard(0, 0, 400-i*10, 16000, 16000);
//             if ( i < 6 ) {
//                 addBoard(9600, 16000, 400-i*10, 40000, 16000);
//             }
//         }
//         boardMx = 128; boardMy = 512;
//         boardRepx = boardRepy = 65536;
//         boardRepXn = boardRepYn = 4;
//         break;

//     case 2:
//         for ( i=0 ; i<16 ; i++ ) {
//             addBoard(7000+i*3000, 0, 1600-i*100, 24000, 5000);
//             addBoard(7000+i*3000, 50000, 1600-i*100, 4000, 10000);
//             addBoard(-7000-i*3000, 0, 1600-i*100, 24000, 5000);
//             addBoard(-7000-i*3000, 50000, 1600-i*100, 4000, 10000);
//         }
//         boardMx = 0; boardMy = 1200;
//         boardRepx = 0;
//         boardRepy = 65536;
//         boardRepXn = 1;
//         boardRepYn = 10;
//         break;

//     case 3:
//         addBoard(9000, 9000, 500, 30000, 30000);
//         for ( i=0 ; i<4 ; i++ ) {
//             for ( j=0 ; j<4 ; j++ ) {
//                 if ( i > 1 || j > 1 ) {
//                     addBoard(i*16384, j*16384, 500, 12000+(i*12345)%3000, 12000+(j*54321)%3000);
//                 }
//             }
//         }
//         for ( i=0 ; i<4 ; i++ ) {
//             for ( j=0 ; j<4 ; j++ ) {
//                 if ( (i > 1 || j > 1) && (i+j)%3 == 0 ) {
//                     addBoard(i*16384, j*16384, 480, 9000+(i*12345)%3000, 9000+(j*54321)%3000);
//                 }
//             }
//         }
//         addBoard(9000, 9000, 480, 20000, 20000);
//         addBoard(9000, 9000, 450, 20000, 20000);
//         addBoard(32768, 40000, 420, 65536, 5000);
//         addBoard(30000, 32768, 370, 4800, 65536);
//         addBoard(32768, 0, 8, 65536, 10000);
//         boardMx = 10; boardMy = 100;
//         boardRepx = boardRepy = 65536;
//         boardRepXn = boardRepYn = 4;
//         break;

//     case 4:
//         addBoard(32000, 12000, 160, 48000, 48000);
//         addBoard(32000, 44000, 160, 48000, 8000);
//         addBoard(64000, 12000, 160, 8000, 48000);
//         for ( i=0 ; i<16 ; i++ ) {
//             addBoard(20000, 0, 160-i*10, 16000, 16000);
//             if ( i < 6 ) {
//                 addBoard(29600, 16000, 160-i*10, 40000, 16000);
//             }
//         }
//         boardMx = 0; boardMy = 128;
//         boardRepx = boardRepy = 65536;
//         boardRepXn = 2; boardRepYn = 2;
//         break;

//     case 5:
//         for ( k=0 ; k<5 ; k++ ) {
//             j = 0;
//             for ( i=0 ; i<16 ; i++ ) {
//                 addBoard(j, i*4096, 200-k*10, 16000, 4096);
//                 addBoard(j+16000-j*2, i*4096, 200-k*10, 16000, 4096);
//                 if ( i < 4 ) j += 2000;
//                 else if ( i < 6 ) j -= 3500;
//                 else if ( i < 12 ) j += 1500;
//                 else j -= 2000;
//             }
//         }
//         boardMx = -64; boardMy = 256;
//         boardRepx = boardRepy = 65536;
//         boardRepXn = boardRepYn = 2;
//         break;

//     }
// }

// void moveBackground() {
//     int i;
//     Board *bd;
//     for ( i=0 ; i<BOARD_MAX ; i++ ) {
//         if ( board[i].width == NOT_EXIST ) continue;
//         bd = &(board[i]);
//         bd->x += boardMx; bd->y += boardMy;
//         bd->x &= (boardRepx-1); bd->y &= (boardRepy-1);
//     }
// }

// void clearBuffer(unsigned char *pbuf,int val)
// {
//     int i;
//     for(i=0;i<SCREEN_W*SCREEN_H*3;i++)
//         pbuf[i]=val;
// }

// // optimized :: 川合(2002/01/21)    -> 式の調整
// void drawBoxAlpha(unsigned char *pbuf,int x1,int y1,int w,int h,int color,int alpha)
// {
//     int x,y,x2,y2;
//     unsigned char col_r, col_g, col_b;
//     x2 = x1+w; y2 = y1+h;

//     //clipping
//     if(x2<0 || y2<0 || x1>SCREEN_W || y1>SCREEN_H)
//         return;
//     if(x1<0) x1=0;
//     if(y1<0) y1=0;
//     if(x2>SCREEN_W) x2=SCREEN_W;
//     if(y2>SCREEN_H) y2=SCREEN_H;

//     col_r = ((color >> 16) & 0xff) * (0x10000 - alpha) >> 16;
//     col_g = ((color >>  8) & 0xff) * (0x10000 - alpha) >> 16;
//     col_b = ( color        & 0xff) * (0x10000 - alpha) >> 16;

//     for (y=y1;y<y2;y++)
//     {
//         //  int off = y * SCREEN_W + x1;
//         unsigned char *p = &pbuf[(y * SCREEN_W + x1) * 3];
//         for (x = x2 - x1; x > 0; x--)
//         {
//             //  pbuf[off] = AlphaBlend(pbuf[off],color,alpha);
//             p[2] = (p[2] * alpha >> 16) + col_r;
//             p[1] = (p[1] * alpha >> 16) + col_g;
//             p[0] = (p[0] * alpha >> 16) + col_b;
//             p += 4;
//         }
//     }
// }


// void drawBackground(unsigned char *pbuf) {
//     int i;
//     Board *bd;
//     int ox, oy, osx, osy, rx, ry;

//     clearBuffer(pbuf,0xFFFFFF);

//     osx = -boardRepx * (boardRepXn/2);
//     osy = -boardRepy * (boardRepYn/2);
//     for ( i=0 ; i<BOARD_MAX ; i++ ) {
//         if ( board[i].width == NOT_EXIST )
//             continue;
//         bd = &(board[i]);
//         ox = osx;
//         for ( rx = 0 ; rx < boardRepXn ; rx++, ox += boardRepx ) {
//             oy = osy;
//             for ( ry = 0 ; ry < boardRepYn ; ry++, oy += boardRepy ) {
//                 drawBoxAlpha(pbuf,
//                              (bd->x+ox)/bd->z + SCREEN_W/2,
//                              (bd->y+oy)/bd->z + SCREEN_H/2,
//                              bd->width, bd->height,
//                              bd->color, 0xD000);
//             }
//         }
//     }
// }

// /* ------------------- */

// int MonaMain(List<char*> *pekoe)
// {
//     int x, y;

//     Screen screen;
//     int vesaBpp = screen.getBpp();
//     byte* vesaVram = screen.getVRAM();
//     int scene_count,scene;

//     printf("user mode screen (x, y) = (%x, %x) %dbpp\n", screen.getWidth(), screen.getHeight(), screen.getBpp());
//     /* check bpp */
//     if (screen.getBpp() < 24) {
//         printf("sorry, this demo needs 24 or 32bpp Video mode\n");
//         return -1;
//     }

//     initBackground();
//     setStageBackground(1);
//     scene = 1;
//     scene_count = FPS * 10;

//     SCREEN_W = screen.getWidth();
//     SCREEN_H = screen.getHeight();

//     while(1) {
//         moveBackground();
//         drawBackground(vesaVram);
//         scene_count--;
//         if(scene_count <= 0){
//             scene       = (scene+1)%6;
//             scene_count = FPS * 20;
//             setStageBackground( scene );
//         }
//     }
// }
