#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "HogeDriver.h"
#include "FAT12.h"

int main(int argc, char *argv[]) {

    DiskDriver* driver = new HogeDriver("./floppy.dat", 50);
    FAT12*      fat    = new FAT12(driver);

    if (!fat->initilize()) {

        int errorNo = fat->getErrorNo();

        if (errorNo == FAT12::BPB_ERROR) printf("BPB read  error \n");
        else if (errorNo == FAT12::NOT_FAT12_ERROR) printf("NOT FAT12 error \n");
        else if (errorNo == FAT12::FAT_READ_ERROR) printf("NOT FAT12 error \n");
        else printf("unknown error \n");

        /* error */
        return -1;
    }

    printf("fat initilize\n");

    printf("¢£changeDirectory to SOMEDIR\n");
    if (!fat->changeDirectoryRelative("SOMEDIR")) {
        printf("some dir not found");
    }


//      if (!fat->open(".", "rtc.cpp", FAT12::READ_MODE)) {

//          printf("open failed");
//      }


//      while (fat->readHasNext()) {
//          byte buf[512];
//          fat->read(buf);
//          for (int i = 0; i < 512; i++) printf("%x", (char)buf[i]);
//      }


    printf("¢£create file hige.cpp\n");
    if (!fat->createFlie("HIGE", "CPP")) {

        printf("can not create file=%d", fat->getErrorNo());
    }

//      printf("¢£open file hige.cpp\n");
//      if (!fat->open(".", "HIGE.CPP", FAT12::WRITE_MODE)) {

//          printf("open failed");
//      }

//      printf("¢£write to hige.cpp\n");
//      byte text[512];
//      memset(text, 0x41, 512);
//      if (!fat->write(text)) {

//          printf("write failed");
//      }

//      memset(text, 0x42, 512);
//      if (!fat->write(text)) {

//          printf("write failed");
//      }

//      memset(text, 0x43, 512);
//      if (!fat->write(text)) {

//          printf("write failed");
//      }

//      memset(text, 0x44, 512);
//      if (!fat->write(text, 50)) {

//          printf("write failed");
//      }


//      if (!fat->close()) {
//          printf("close failed");
//      }

//      //      memset(text, 0x44, 512);
//      //      if (!fat->write(text)) {

//      //           printf("write failed");
//      //      }


//      if (!fat->open(".", "HIGE.CPP", FAT12::READ_MODE)) {

//          printf("open failed");
//      }


//      while (fat->readHasNext()) {
//          byte buf[512];
//          fat->read(buf);
//          for (int i = 0; i < 512; i++) printf("%x", (char)buf[i]);
//      }

//      if (!fat->changeDirectoryRelative("..")) {
//          printf("some dir not found");
//      }

//  //      if (!fat->open(".", "KERNEL.CPP", FAT12::READ_MODE)) {

//  //          printf("open failed");
//  //      }

//  //      while (fat->readHasNext()) {
//  //          byte buf[512];
//  //          fat->read(buf);
//  //          for (int i = 0; i < 512; i++) printf("%c", (char)buf[i]);
//  //      }



//      //      memset(text, 1, 512);
//      //      if (!fat->write(text)) {

//      //           printf("write failed");
//      //      }

//      //      if (!fat->open("SOMEDIR\\DIR1\\DIR2\\DIR3\\DIR4", "SOME.CPP", FAT12::READ_MODE)) {
//      //          printf("open failed");
//      //      }

//      //      while (fat->readHasNext()) {
//      //          byte buf[512];
//      //          fat->read(buf);
//      //          for (int i = 0; i < 512; i++) printf("%c", (char)buf[i]);
//      //      }

//      return 0;

//      if (!fat->changeDirectoryRelative("SOMEDIR")) {
//          printf("some dir not found");
//      }

//      if (!fat->open("..", "HELLO.TXT", FAT12::READ_MODE)) {

//          printf("open failed");
//      }

    //      if (!fat->changeDirectory("SOMEDIR\\DIR1\\DIR2\\DIR3\\DIR4")) {
    //          printf("changeDirectory failed");
    //          return -1;
    //      }


    delete fat;
    delete driver;

    return 0;
}

