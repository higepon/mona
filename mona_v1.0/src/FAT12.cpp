/*!
  \file  FAT12.cpp
  \brief class FAT12 File System

  Copyright (c) 2003 HigePon
  WITHOUT ANY WARRANTY

  \author  HigePon
  \version $Revision$
  \date   create:2003/04/10 update:$Date$
*/

#include<FAT12.h>
#include<string.h>

#include<stdio.h>
//#include<global.h>

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

/*!
  \brief initilize

  \author HigePon
  \date   create:2003/04/10 update:
*/
FAT12::FAT12(DiskDriver* driver) {

    driver_ = driver;
    return;
}

/*!
  \brief destroy

  \author HigePon
  \Date   create:2003/04/10 update:
*/
FAT12::~FAT12() {


    return;
}

bool FAT12::initilize() {

    if (!setBPB()) return false;

    if (isFAT12()) printf("file system is FAT12");
    else printf("file system unknown");

    int rootEntryStart = bpb_.reservedSectorCount
                       + bpb_.fatSize16 * bpb_.numberFats;

    if (!(driver_->read(rootEntryStart, buf_))) return false;

    printf("rootEntryStart = %d\n", rootEntryStart);

    DirectoryEntry entry[20];

    memcpy(entry, buf_, sizeof(DirectoryEntry) * 20);

    for (int j = 0; j < 15; j++) {

        if (entry[j].filename[0] == 0xe5 || entry[j].filename[0] == 0x00) continue;

        printf("[");
        for (int i = 0; i < 8; i++) printf("%c", (char)(entry[j].filename[i]));
        printf("]");
        printf("[");

        if (entry[j].attribute & ATTR_DIRECTORY) printf("DIR");
        else {
            for (int i = 0; i < 3; i++) printf("%c", (char)(entry[j].extension[i]));
        }
        printf("]");
        printf("size=%d ", entry[j].filesize);
        printf("%d/%d/%d ", entry[j].fdate.year + 1980, entry[j].fdate.month, entry[j].fdate.day);
        printf("%d:%d:%d ", entry[j].ftime.hour, entry[j].ftime.min, (entry[j].ftime.sec) * 2);
        printf("cluster = %d\n", entry[j].cluster);
    }


    int rootDirSectors = ((bpb_.rootEntryCount * 32) + (bpb_.bytesPerSector - 1)) / bpb_.bytesPerSector;
    int firstDataSector = bpb_.reservedSectorCount + bpb_.numberFats * bpb_.fatSize16 + rootDirSectors;
    int lbp = ((3 - 2) * bpb_.sectorPerCluster) + firstDataSector;

    if (!(driver_->read(lbp, buf_))) return false;

    for (int k = 0; k < 512; k++) printf("%c", (char)buf_[k]);


    printf("sizeof directryentry 32 = %d", sizeof(DirectoryEntry));

    return true;
}

/*!
  \brief specify file system.

  \author HigePon
  \Date   create:2003/04/17 update:
*/
bool FAT12::isFAT12() {

    int rootDirSectors;
    int totalSector;
    int dataSector;
    int countOfClusters;

    rootDirSectors = ((bpb_.rootEntryCount * 32) + (bpb_.bytesPerSector - 1))
                   / bpb_.bytesPerSector;

    totalSector = (bpb_.totalSector16 != 0) ? bpb_.totalSector16 : bpb_.totalSector32;
    dataSector = totalSector - (bpb_.reservedSectorCount + (bpb_.numberFats * bpb_.fatSize16) + rootDirSectors);
    countOfClusters = dataSector / bpb_.sectorPerCluster;

    /* FAT12 */
    if (countOfClusters < 4085) return true;

    /* FAT16, FAT32 or ??? */
    return false;
}

/*!
  \brief reade BPB and parse

  \author HigePon
  \Date   create:2003/04/17 update:
*/
bool FAT12::setBPB() {

    byte* p = buf_;

    if (!(driver_->read(0, buf_))) return false;

    p += 3;
    memcpy(bpb_.oemName, p, 8);
    p += 8;
    memcpy(&(bpb_.bytesPerSector), p, 2);
    p += 2;
    bpb_.sectorPerCluster = *p;
    p += 1;
    memcpy(&(bpb_.reservedSectorCount), p, 2);
    p += 2;
    bpb_.numberFats = *p;
    p += 1;
    memcpy(&(bpb_.rootEntryCount), p, 2);
    p += 2;
    memcpy(&(bpb_.totalSector16), p, 2);
    p += 2;
    bpb_.media = *p;
    p += 1;
    memcpy(&(bpb_.fatSize16), p, 2);
    p += 2;
    memcpy(&(bpb_.sectorPerTrack), p, 2);
    p += 2;
    memcpy(&(bpb_.numberOfHeaders), p, 2);
    p += 2;
    memcpy(&(bpb_.hiddenSector), p, 4);
    p += 4;
    memcpy(&(bpb_.totalSector32), p, 4);
    p += 4;
    bpb_.driveNumber = *p;
    p += 1;
    bpb_.reserved1 = *p;
    p += 1;
    bpb_.bootSignature = *p;
    p += 1;
    memcpy(&(bpb_.volumeId), p, 4);
    p += 4;
    memcpy(&(bpb_.volumeLabel), p, 11);
    p += 11;
    memcpy(&(bpb_.fileSysType), p, 8);
    return true;
}
