//*****************************************************************************
// fat.h :
// Licence : see http://gaku.s12.xrea.com/wiki/main.cgi?c=g&p=Gaku%2FLicence
// 2004/02/02 by Gaku :
//*****************************************************************************

//-----------------------------------------------------------------------------
#ifndef _FAT_H_
#define _FAT_H_

#include "sys/types.h"
#include "file.h"
#include "IStorageDevice.h"


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FAT
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FAT
{
private:
    enum {
        BYTES_PER_SECTOR    = 0x0b,
        SECTORS_PER_CLUSTER = 0x0d,
        RESERVED_SECTORS    = 0x0e,
        NUMBER_OF_FATS      = 0x10,
        NUMBER_OF_DIRENTRY  = 0x11,
        NUMBER_OF_SECTORS   = 0x13,
        SECTORS_PER_FAT     = 0x16,
        FILE_SYSTEM_ID      = 0x36,
        SECTOR_SIZE = 0x0200,
        START_OF_CLUSTER = 2,
        END_OF_CLUSTER = 0x0fff
    };

    static const byte FileSystemID[];

    dword bytesPerSector;
    dword sectorsPerCluster;
    dword reservedSectors;
    dword numberOfFats;
    dword numberOfDirEntry;
    dword sectorsPerFat;
    dword rootDirectoryEntry;
    dword dataArea;
    dword numberOfClusters;

    IStorageDevice *floppy;
    byte *flag;
    byte *fat;

public:
    FAT ();
    ~FAT ();

    bool initialize (IStorageDevice *p);

    dword getBytesPerSector ();
    dword getSectorsPerCluster ();
    dword getNumberOfDirEntry ();
    dword getNumberOfClusters ();
    dword getRootDirectoryEntry ();

    dword allocateCluster (dword cluster, dword count);
    void freeCluster (dword cluster);

    bool read (dword lba, byte *bf);
    bool write (dword lba, byte *bf);

    dword getLbaFromCluster (dword cluster);
    dword getNextCluster (dword cluster);

private:
    void setNextCluster (dword cluster, dword next);
    dword searchFreeCluster (dword cluster);
    void clearFlag ();
    void flushFat ();
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatFile
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FatDirectory;

class FatFile : public File
{
private:
    enum {
        RESIZE_DELTA = 8
    };

private:
    dword last;
    FAT *fat;
    FatDirectory *parent;
    byte *file;
    byte *flag;
    dword *lba;
    dword fsize;
    dword sectors;
    dword pos;
    int entry;
    bool sizeChanged;

public:
    FatFile ();
    ~FatFile ();

    bool initialize (FAT *p, FatDirectory *d, int e, dword c, dword s);

    dword read (byte *bf, dword sz);
    dword write (byte *bf, dword sz);
    bool seek (int pt, int flag);
    bool flush ();
    bool resize (dword sz);
    dword position ();
    dword size ();

private:
    void clearFlag ();
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FatDirectory : public Directory
{
protected:
    enum {
        ATTR_ARCHIVE   = 0x20,
        ATTR_DIRECTORY = 0x10,
        ATTR_VOLUME    = 0x08,
        ATTR_SYSTEM    = 0x04,
        ATTR_HIDDEN    = 0x02,
        ATTR_READONLY  = 0x01,
        ATTR_VFAT      = 0x0f
    };

    enum {
        FILENAME    = 0x00,
        EXTENTION   = 0x08,
        ATTRIBUTE   = 0x0b,
        EXTRADATA   = 0x0c,
        LOW_CLUSTER = 0x1a,
        FILESIZE    = 0x1c,
        SIZE_FILENAME  = 8,
        SIZE_EXTENTION = 3,
        END_OF_CLUSTER = 0xfff,
        MARK_DELETE = 0xe5,
        MARK_UNUSED = 0x00
    };

    dword start;
    byte *entrys;
    byte *unused;
    byte *end;
    dword *lba;
    FAT *fat;

public:
    FatDirectory ();
    ~FatDirectory ();

    virtual bool initialize (FAT *p, dword cluster) = 0;

    int searchEntry (byte *bf);
    int getEntryName (int entry, byte *bf);
    int getHeadEntry ();
    int getNextEntry (int entry);
    bool deleteEntry (int entry);

    int newDirectory (byte *bf);
    int newFile (byte *bf, dword sz);

    Directory* getDirectory (int entry);
    File* getFile (int entry);
    bool isDirectory (int entry);
    bool isFile (int entry);
    dword getIdentifer ();

    bool setFileSize (int entry, dword size);
    bool setCluster (int entry, dword cluster);

protected:
    byte* searchUnusedEntry ();

private:
    bool isValid (byte *ent);
    void expandFileName (byte *name, byte *bf);
    bool clearDirectory (int entry);
    int searchFreeEntry ();
    int newEntry (byte *bf, dword sz, byte attr, dword fsize);
    void setEntry (byte *ent, byte *n, byte a, word c, dword s);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatRootDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FatRootDirectory : public FatDirectory
{
public:
    FatRootDirectory () : FatDirectory() {}
    ~FatRootDirectory () {}

    bool initialize (FAT *p, dword cluster);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatSubDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FatSubDirectory : public FatDirectory
{
public:
    FatSubDirectory () : FatDirectory() {}
    ~FatSubDirectory () {}

    bool initialize (FAT *p, dword cluster);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatStorage
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FatStorage
{
private:
    FAT *fat;

public:
    FatStorage ();
    ~FatStorage ();

    bool initialize (IStorageDevice *p);

    Directory* getRootDirectory ();
};

//-----------------------------------------------------------------------------
#endif // _FAT_H_ //
