#include<tester.h>
#include<FAT12.h>
#include<FDCDriver.h>
#include<global.h>
#include<io.h>
#include<GraphicalConsole.h>
#include<operator.h>
#include<IA32MemoryManager.h>
#include<z.h>
#include<MemoryManager.h>

extern "C" void put_pixel(int x, int y, char color);

typedef struct {
    dword a;
    dword b;
    dword width;
    dword height;
} ARGBHeader;

static bool drawARGB(byte* rgba, int x, int y, int size);
static int getColorNumber(byte* rgba);

struct read_info {
        FAT12 *fat;
        int sz;
} read_info;

int read(input_stream *p, int sz)
{
        if (0 >= read_info.sz)
                return 0;

        if (!read_info.fat->read(p->bf)) {
                g_console->printf("error read\n");
                return 0;
        }

        if (512 > read_info.sz)
                return read_info.sz;

        read_info.sz -= 512;

        return 512;
}

int write(output_stream *p, int sz)
{
        *((int*)(p)->data) = sz;

        return sz;
}


bool drawARGB(byte* rgba, int x, int y, int size) {

    //    int color;
    int startx = x;
    int starty = y;
    ARGBHeader* header = (ARGBHeader*)rgba;

    /* check identifier */
    if (header->a != 0x41524742) return false;

    /* draw loop */
    for (int i = sizeof(ARGBHeader) / 4; i < size / 4; i++) {

        if (x >= startx + (int)(header->width)) {

            y++;
            x = startx;
        }

        if (y >= starty + (int)(header->height)) break;

        //color = (rgba[i * 4 + 0] || rgba[i * 4 + 1] || rgba[i * 4 + 2]) ? GP_WHITE : GP_BLACK;
        //        if (rgba[i * 4 + 0] == 0xFF) color = GP_WHITE;
        //        else if (rgba[i * 4 + 0] == 0xC0) color = GP_GRAY;
        //        else color = GP_BLACK;


        put_pixel(x, y, getColorNumber(&(rgba[i * 4])));
        x++;
    }

    return true;
}

int getColorNumber(byte* rgba) {

    byte r = rgba[2];
    byte g = rgba[1];
    byte b = rgba[0];
    int result;

    if (r == 0x00 && g == 0x00 && b == 0x00) result = 0;
    else if (r == 0x00 && g == 0x00 && b == 0x80) result = 1;
    else if (r == 0x00 && g == 0x80 && b == 0x00) result = 2;
    else if (r == 0x00 && g == 0x80 && b == 0x80) result = 3;
    else if (r == 0x80 && g == 0x00 && b == 0x00) result = 4;
    else if (r == 0x80 && g == 0x00 && b == 0x80) result = 5;
    else if (r == 0x80 && g == 0x80 && b == 0x00) result = 6;
    else if (r == 0xc0 && g == 0xc0 && b == 0xc0) result = 7;
    else if (r == 0x80 && g == 0x80 && b == 0x80) result = 8;
    else if (r == 0x00 && g == 0x00 && b == 0xff) result = 9;
    else if (r == 0x00 && g == 0xff && b == 0x00) result = 10;
    else if (r == 0x00 && g == 0xff && b == 0xff) result = 11;
    else if (r == 0x00 && g == 0xff && b == 0xff) result = 12;
    else if (r == 0xff && g == 0x00 && b == 0x00) result = 13;
    else if (r == 0xff && g == 0x00 && b == 0xff) result = 14;
    else result = 15;

    return result;
}

void FDCDriverTester() {

    g_info_level = MSG;

    g_fdcdriver = new FDCDriver();
    g_console->printf("[1.1]");
    g_fdcdriver->motor(ON);

    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    g_console->printf("[1.2]");
    FAT12* fat = new FAT12((DiskDriver*)g_fdcdriver);

        g_console->printf("fat=[%x]", (dword)fat);

    if (!fat->initilize()) {
            g_console->printf("error fat initialize\n");
                g_fdcdriver->motor(false);
                return;
        }
    g_console->printf("[1.3]");

        if (!fat->open(".", "Z.Z", FAT12::READ_MODE)) {
                g_console->printf("error open mona.z\n");
                g_fdcdriver->motor(false);
                return;
        }

    g_console->printf("[1.4]");

        read_info.fat = fat;


        read_info.sz = fat->getFileSize();

        unsigned char *bf = (unsigned char*)malloc(512);
        if (NULL == bf) {
                g_console->printf("not enough memory\n");
                g_fdcdriver->motor(false);
                return;
        }


    g_console->printf("[1.5]");
        input_stream is;
        is.bf = bf;
        is.sz = 512;
        is.read = read;

    g_console->printf("[1.6]************************************");
        int bf_size = 1024 * 1024 * 1;
        bf = (unsigned char*)malloc(bf_size);
    g_console->printf("[1.7]malloc=%x", (dword)bf);
        if (NULL == bf) {
                g_console->printf("not enough memory\n");
                g_fdcdriver->motor(false);
                return;
        }

        int image_size;
        output_stream os;
        os.bf = bf;
    g_console->printf("[1.8]");
        os.sz = bf_size;
        os.write = write;
        os.data = &image_size;

        decode(&is, &os);
    g_console->printf("[3.0]");

              for (int i = 0; i < 25; i++) {
                  g_console->printf("%c", bf[i]);
              }
    //        drawARGB(bf, 0, 0, image_size);


        if (!fat->close()) {
                g_console->printf("error close\n");
        }

        g_console->printf("ok go on\n");

        g_fdcdriver->motor(false);
        delete(fat);

}

/*!
    \file   tester_higepon.cpp
    \brief  test code for higepon

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/18 update:$Date$
*/

void ELFTester(byte* out) {

    g_fdcdriver = new FDCDriver();

    byte tbuf[512];
    for (int i = 0; i < 0xff; i++) {tbuf[i] = i;}
    for (int i = 0xff; i < 512; i++){ tbuf[i] = 512 - i;}


    g_console->printf("hoge[1]");
    g_fdcdriver->motor(false);

    for (int i = 0; i< 20; i++) {
        delay(1);
        delay(1);
    }

    g_fdcdriver->motor(true);
    g_console->printf("hoge[2]");
    info(DEV_NOTICE, "before recalibrate");
    g_console->printf("hoge[3]");
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_console->printf("hoge[4]");
    info(DEV_NOTICE, "before read");
    for (int i = 1; i < 25; i++) {
        memset(tbuf, 0x99, 512);
        g_fdcdriver->read(i, tbuf);

    }

    FAT12* fat = new FAT12((DiskDriver*)g_fdcdriver);
    if (!fat->initilize()) {

        int errorNo = fat->getErrorNo();

        if (errorNo == FAT12::BPB_ERROR) info(ERROR, "BPB read  error \n");
        else if (errorNo == FAT12::NOT_FAT12_ERROR) info(ERROR, "NOT FAT12 error \n");
        else if (errorNo == FAT12::FAT_READ_ERROR) info(ERROR, "NOT FAT12 error \n");
        else info(ERROR, "unknown error \n");

        info(ERROR, "fat initilize faild\n");
        while (true);
    }

    info(MSG, "initilize OK\n");

    info(MSG, "try to open file USER.ELF\n");
    if (!fat->open(".", "USER.ELF", FAT12::READ_MODE)) {

        info(ERROR, "open failed");
    }

    for (int i = 0; i < 10; i++) {

        if (!fat->read(out + 512 * i)) {

            info(ERROR, "read failed %d", i);
        }

    }

    if (!fat->close()) {
       info(ERROR, "close failed");
    }

    g_console->printf("load done...USER.ELF\n");
    g_fdcdriver->motor(false);

}




void FDCTester() {

    info(DEV_NOTICE, "start1");

    g_fdcdriver = new FDCDriver();

    info(DEV_NOTICE, "start2");

    byte tbuf[512];
    for (int i = 0; i < 0xff; i++) {tbuf[i] = i;}
    for (int i = 0xff; i < 512; i++){ tbuf[i] = 512 - i;}

    g_fdcdriver->motor(true);

    info(DEV_NOTICE, "before recalibrate");

    g_fdcdriver->recalibrate();

    info(DEV_NOTICE, "before read");
    for (int i = 0; i < 10; i++) {
        memset(tbuf, 0x99, 512);
        g_fdcdriver->read(1, tbuf);

    }

    info(DEV_NOTICE, "after read");

//      // write
//      for (int i = 0; i < 73; i++) {

//          g_console->printf("write");
//          memset(tbuf, i + 5, 512);
//          if (!g_fdcdriver->write(i, tbuf)) {

//              g_console->printf("write failed %d", i);
//              //            g_fdcdriver->motor(false);
//              //            while (true);
//          }
//      }

//      memset(tbuf, 0x99, 512);
//      if (!g_fdcdriver->read(0, tbuf)) {
//           g_console->printf("read failed %d", 50);
//           g_fdcdriver->motor(false);
//           while (true);
//      }
//      for (int i = 0; i < 512; i++) g_console->printf("[%d]", tbuf[i]);

//      while (g_demo_step < 8);

//      memset(tbuf, 0x99, 512);
//      if (!g_fdcdriver->read(1, tbuf)) {
//           g_console->printf("read failed %d", 50);
//           g_fdcdriver->motor(false);
//           while (true);
//      }
//      for (int i = 0; i < 512; i++) g_console->printf("[%d]", tbuf[i]);
//      while (true);

//      g_fdcdriver->motor(false);
//      g_console->printf("ok");
//      while (true);

    FAT12* fat = new FAT12((DiskDriver*)g_fdcdriver);
    if (!fat->initilize()) {

        int errorNo = fat->getErrorNo();

        if (errorNo == FAT12::BPB_ERROR) info(ERROR, "BPB read  error \n");
        else if (errorNo == FAT12::NOT_FAT12_ERROR) info(ERROR, "NOT FAT12 error \n");
        else if (errorNo == FAT12::FAT_READ_ERROR) info(ERROR, "NOT FAT12 error \n");
        else info(ERROR, "unknown error \n");

        info(ERROR, "fat initilize faild\n");
        while (true);
    }

    info(MSG, "initilize OK\n");
    info(MSG, "changeDirectory to SOMEDIR\n");

    if (!fat->changeDirectoryRelative("SOMEDIR")) {
        info(ERROR, "some dir not found");
        while (true);
    }

    info(MSG, "changeDirectoryRelative OK\n");
    info(MSG, "try to create file hige.cpp\n");

    if (!fat->createFlie("HIGE", "CPP")) {

        info(ERROR, "can not create file=%d", fat->getErrorNo());
        while (true);
    }

    info(MSG, "try to open file hige.cpp\n");
    if (!fat->open(".", "HIGE.CPP", FAT12::WRITE_MODE)) {

        info(ERROR, "open failed");
    }

    info(MSG, "try to write to hige.cpp 'M' * 512\n");
    byte text[512];
    memset(text, 'M', 512);
    if (!fat->write(text)) {

        info(ERROR, "write failed");
    }

    info(MSG, "try to write to hige.cpp 'o' * 512\n");
    memset(text, 'o', 512);
    if (!fat->write(text)) {

       info(ERROR, "write failed");
    }

    info(MSG, "try to write to hige.cpp 'n' * 512\n");
    memset(text, 'n', 512);
    if (!fat->write(text)) {

       info(ERROR, "write failed");
    }

    info(MSG, "try to write to hige.cpp 'a' * 512\n");
    memset(text, 'a', 512);
    if (!fat->write(text)) {

       info(ERROR, "write failed");
    }

    info(MSG, "try to close  hige.cpp\n");

    if (!fat->close()) {
       info(ERROR, "close failed");
    }

    g_console->printf("\nHit any key to start [kernel thread demo]\n");
    g_fdcdriver->motor(false);

    info(MSG, "test done!! Boot Windows & read floppy disk \\somedir\\hige.cpp\n");
}
