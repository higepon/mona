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
    byte  numberFATs;
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
    static const int NORMAL_STATE;
    static const int BPB_ERROR;
    static const int NOT_FAT12_ERROR;
    static const int FAT_READ_ERROR;
    static const int NOT_DIR_ERROR;
    static const int PATH_LENGTH;
    static const int DRIVER_READ_ERROR;
    static const int DRIVER_WRITE_ERROR;
    static const int FILE_EXIST_ERROR;
    static const int END_OF_FILE;
    static const char PATH_SEP;
    static const int IS_OPEN_ERROR;
    static const int INVALID_FILE_NAME;
    static const int FILE_NOT_FOUND;
    static const int READ_MODE;
    static const int WRITE_MODE;

 public:

    bool initilize();
    bool createFlie(const char* name, const char* ext);
    bool open(const char* path, const char* filename, int mode);
    bool close();
    bool read(byte* buffer);
    bool write(byte* buffer);
    bool write(byte* buffer, int size);
    bool rename(const char* from, const char* to);
    bool remove(const char* file);
    bool removeDirecotry(const char* name);
    bool makeDirectory(const char* name);
    bool changeDirectory(const char* path);
    bool changeDirectoryRelative(const char* path);
    bool readHasNext() const;
    int getFileSize() const;
    int getErrorNo() const;

 private:

    bool readBPB();
    bool isFAT12();
    bool readFAT(bool allocate);
    bool writeFAT();
    bool writeEntry();
    bool readEntry();
    word getFATAt(int cluster) const;
    bool setFATAt(int cluster, word fat);
    bool releaseCluster();
    char* getPathAt(const char* path, int index) const;
    bool compareName(const char* name1, const char* name2) const;
    int clusterToLba(int cluster);

 private:

    byte buf_[512];            /* file buffer     */
    byte* fat_;                /* fat region      */
    DiskDriver* driver_;       /* disk driver     */
    BitMap* map_;              /* cluster map     */
    byte errNum_;              /* error number    */

    bool readHasNext_;         /* read state      */
    bool isOpen_;              /* r/w  state      */
    bool firstWrite_;          /* write state     */
    int  currentCluster_;      /* r/w  state      */
    int  fileSize_;            /* read state      */
    int  readCounter_;         /* read counter    */
    int  currentDirectory_;    /* current cluster */
    int  fatStart_;            /* fatStart        */
    int  openMode_;            /* open mode       */
    char currentPath_[512];    /* current path    */

    /* depends on bpb */
    int rootDirSectors_;
    int firstDataSector_;
    int rootEntryStart_;

    DirectoryEntry entries_[16];
    DirectoryEntry* currentEntry_;
    BPB bpb_;

};

#endif
