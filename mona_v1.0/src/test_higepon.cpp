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
    g_fdcdriver->recalibrate();

    for (int i = 0; i < 10; i++) {
        memset(tbuf, 0x99, 512);
        g_fdcdriver->read(1, tbuf);

    }

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

        g_console->printf("fat initilize faild\n");
        while (true);
    }

    g_console->printf("init ok");
    while (true);
    g_console->printf("changeDirectory to SOMEDIR\n");
    if (!fat->changeDirectoryRelative("SOMEDIR")) {
        g_console->printf("some dir not found");
        while (true);
    }

    g_console->printf("cdr ok");

    g_console->printf("create file hige.cpp\n");
    if (!fat->createFlie("HIGE", "CPP")) {

        g_console->printf("can not create file=%d", fat->getErrorNo());
        while (true);
    }

    g_console->printf("open file hige.cpp\n");
    if (!fat->open(".", "HIGE.CPP", FAT12::WRITE_MODE)) {

        g_console->printf("open failed");
    }

    g_console->printf("write to hige.cpp\n");
    byte text[512];
    memset(text, 'M', 512);
    if (!fat->write(text)) {

        g_console->printf("write failed");
    }

    memset(text, 'o', 512);
    if (!fat->write(text)) {

        g_console->printf("write failed");
    }

    memset(text, 'n', 512);
    if (!fat->write(text)) {

        g_console->printf("write failed");
    }

    memset(text, 'a', 512);
    if (!fat->write(text)) {

        g_console->printf("write failed");
    }

    if (!fat->close()) {
        g_console->printf("close failed");
    }

    g_console->printf("\nHit any key to start [kernel thread demo]\n");
    g_fdcdriver->motor(false);
}
