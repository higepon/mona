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
#include<DiskDriver.h>

/*! Bios Parameter Block */
typedef struct BPB {

    byte  oemName[8];
    word  sizeOfSector;
    byte  sectorPerCluster;
    word  reservedSector;
    byte  numberOfFat;
    word  rootEntries;
    word  totalSector;
    byte  media;
    word  fatSize;
    word  sectorPerTrack;
    word  numberOfHeads;
    dword hiddenSector;
    dword totalBigSector;
    byte  driveId;
    byte  dirtyFlag;
    byte  extendBootSign;
    dword serialNo;
    byte  volume[11];
    byte  type[8];
};
#pragma pack(2)
typedef struct FDATE {
    unsigned day   : 5;
    unsigned month : 4;
    unsigned year  : 7;
};
#pragma pack(0)

#pragma pack(2)
typedef struct FTIME {
    unsigned sec : 5;
    unsigned min : 6;
    unsigned hour: 5;
};
#pragma pack()

#pragma pack(2)
typedef struct DirectoryEntry {
    byte  filename[8];
    byte  extension[3];
    byte  attribute;
    byte  reserved[10];
    FTIME ftime;
    FDATE fdate;
    word  cluster;
    dword filesize;
};
#pragma pack()

/*!
    FAT12 File System claass
*/
class FAT12 {

 public:
    FAT12(DiskDriver* driver);
    ~FAT12();

 public:

    bool initilize();
    BPB bpb_;


 private:

    bool setBPB();

 private:

    byte buf_[512];
    DiskDriver* driver_;
};

#endif
