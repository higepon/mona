#include<tester.h>
#include<FAT12.h>
#include<FDCDriver.h>
#include<global.h>

/*!
    \file   tester_higepon.cpp
    \brief  test code for higepon

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/05/18 update:$Date$
*/
void FDCTester() {

    g_fdcdriver = new FDCDriver(g_console);

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

    info(DEV_NOTICE, "init ok");

    info(DEV_NOTICE, "changeDirectory to SOMEDIR\n");
    if (!fat->changeDirectoryRelative("SOMEDIR")) {
        info(ERROR, "some dir not found");
        while (true);
    }

    info(DEV_NOTICE, "cdr ok\n");

    info(DEV_NOTICE, "create file hige.cpp\n");
    if (!fat->createFlie("HIGE", "CPP")) {

        info(ERROR, "can not create file=%d", fat->getErrorNo());
        while (true);
    }

    info(DEV_NOTICE, "open file hige.cpp\n");
    if (!fat->open(".", "HIGE.CPP", FAT12::WRITE_MODE)) {

        info(ERROR, "open failed");
    }

    info(DEV_NOTICE, "write to hige.cpp\n");
    byte text[512];
    memset(text, 'M', 512);
    if (!fat->write(text)) {

        info(ERROR, "write failed");
    }

    memset(text, 'o', 512);
    if (!fat->write(text)) {

       info(ERROR, "write failed");
    }

    memset(text, 'n', 512);
    if (!fat->write(text)) {

       info(ERROR, "write failed");
    }

    memset(text, 'a', 512);
    if (!fat->write(text)) {

       info(ERROR, "write failed");
    }

    if (!fat->close()) {
       info(ERROR, "close failed");
    }

    g_console->printf("\nHit any key to start [kernel thread demo]\n");
    g_fdcdriver->motor(false);
}
