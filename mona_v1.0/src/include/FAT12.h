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
#include<BitMap.h>

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
    static const int NOT_DIR_ERROR;
    static const int PATH_LENGTH;
    static const int DRIVER_READ_ERROR;
    static const int FILE_EXIST_ERROR;
    static const char PATH_SEP;

    static const int READ_MODE;

 public:

    bool initilize();
    bool createFlie(const char* name, const char* ext);
    bool open(const char* path, const char* filename, int mode);
    bool close();
    bool read(byte* buffer);
    bool write(const char* file, byte* buffer);
    bool rename(const char* from, const char* to);
    bool remove(const char* file);
    bool removeDirecotry(const char* name);
    bool makeDirectory(const char* name);
    bool changeDirectory(const char* path);
    bool changeDirectoryRelative(const char* path);
    bool readHasNext() const;

    int getErrorNo() const;
    BPB bpb_;


 private:

    bool setBPB();
    bool isFAT12();
    word getFATAt(int cluster) const;
    bool setFatAt(int cluster, word fat);
    char* getPathAt(const char* path, int index) const;
    int getEmptyEntry(int cluster);
    bool compareName(const char* name1, const char* name2) const;
    int clusterToLbp(int cluster);

 private:

    byte buf_[512];             /* file buffer     */
    byte* fat_;                 /* fat region      */
    DiskDriver* driver_;        /* disk driver     */
    byte errNum_;               /* error number    */

    bool dirty_;                /* dirty flag      */

    bool readHasNext_;          /* read state      */
    bool isOpen_;               /* read state      */
    int  currentCluster_;       /* read state      */
    int  fileSize_;             /* read state      */

    char currentPath_[512];      /* current path   */
    int currentDirecotry_;      /* current cluster */

    int fatStart_;              /* fatStart        */
    BitMap* map_;               /* cluster map     */

    /* depends on bpb */
    int rootDirSectors_;
    int firstDataSector_;
    int rootEntryStart_;


};

#endif
