/*!
    \file  FAT12.h
    \brief FAT12

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/04/10 update:$Date$
*/
#ifndef _MONA_FAT12_
#define _MONA_FAT12_

#include<types.h>

/*! Bios Parameter Block */
typedef struct BPB {

    byte oemName[8];
    word sizeOfSector;
    byte sectorPerCluster;
    word reservedSector;
    byte NumberOfFat;
    word rootEntries;
    word totalSector;
    byte media;
    word fatSize;
    word sectorPerTrack;
    word numberOfHeads;
    word hiddenSector;
    word totalBigSector;
    byte driveId;
    byte dirtyFlag;
    byte extendBootSign;
    dword serialNo;
    byte volume[11];
    byte type[8];
};

/*!
    FAT12 File System claass
*/
class FAT12 {

 public:
    FAT12();
    ~FAT12();

 public:

 private:

};

#endif
