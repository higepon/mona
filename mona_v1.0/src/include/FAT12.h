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
    word  bytesPerSector;
    byte  sectorPerCluster;
    word  reservedSectorCount;
    byte  numberFats;
    word  rootEntryCount;
    word  totalSector16;
    byte  media;
    word  fatSize16;
    word  sectorPerTrack;
    word  numberOfHeaders;
    dword hiddenSector;
    dword totalSector32;
    byte  driveNumber;
    byte  reserved1;
    byte  bootSignature;
    dword volumeId;
    byte  volumeLabel[11];
    byte  fileSysType[8];
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
#pragma pack(0)

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
    static const int BPB_ERROR;
    static const int NOT_FAT12_ERROR;
    static const int FAT_READ_ERROR;
    static const int PATH_LENGTH;
    static const char PATH_SEP;

 public:

    bool initilize();
    bool createFlie(const char* name);
    bool open(const char* name);
    bool read(const char* file, byte* buffer);
    bool write(const char* file, byte* buffer);
    bool rename(const char* from, const char* to);
    bool remove(const char* file);
    bool removeDirecotry(const char* name);
    bool makeDirectory(const char* name);
    bool changeDirectory(const char* path);
    bool readHasNext();

    int getErrorNo();
    BPB bpb_;


 private:

    bool setBPB();
    bool isFAT12();
    word getFATAt(int cluster);

 private:

    byte buf_[512];             /* file buffer     */
    byte fat_[512];             /* fat region      */
    DiskDriver* driver_;        /* disk driver     */
    byte errNum_;               /* error number    */

    bool dirty_;                /* dirty flag      */

    bool readHasNext_;          /* read state      */
    int  currentCluster_;       /* read state      */

    char curentPath_[512];      /* current path    */
    int currentDirecotry_;      /* current cluster */

    int fatStart_;              /* fatStart        */

};

#endif
