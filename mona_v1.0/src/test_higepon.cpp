#include<tester.h>
#include<FAT12.h>
#include<FDCDriver.h>
#include<global.h>

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

    g_fdcdriver = new FDCDriver(g_console);

    byte tbuf[512];
    for (int i = 0; i < 0xff; i++) {tbuf[i] = i;}
    for (int i = 0xff; i < 512; i++){ tbuf[i] = 512 - i;}

    g_fdcdriver->motor(true);

    info(DEV_NOTICE, "before recalibrate");

    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

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

    info(MSG, "try to open file hige.cpp\n");
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

    g_console->printf("\nUSER.ELF\n");
    g_fdcdriver->motor(false);

}




void FDCTester() {

    info(DEV_NOTICE, "start1");

    g_fdcdriver = new FDCDriver(g_console);

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
