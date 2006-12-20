//*****************************************************************************
// fat.h :
// 2004/04/26 by Gaku :
// Licence : see http://gaku.s12.xrea.com/wiki/main.cgi?c=g&p=Gaku%2FLicence
//*****************************************************************************

#ifndef _FAT_H_
#define _FAT_H_

//-----------------------------------------------------------------------------
#ifdef MONA
#include <sys/types.h>
#else
#include "types.h"
#endif
#include "file.h"
#include "IStorageDevice.h"
#include "endian_util.h"

namespace FatFS
{

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

	static const uint8_t FileSystemID[];

	uint32_t bytesPerSector;
	uint32_t sectorsPerCluster;
	uint32_t reservedSectors;
	uint32_t numberOfFats;
	uint32_t numberOfDirEntry;
	uint32_t sectorsPerFat;
	uint32_t rootDirectoryEntry;
	uint32_t dataArea;
	uint32_t numberOfClusters;

	IStorageDevice *floppy;
	uint8_t *flag;
	uint8_t *fat;

public:
	FAT ();
	~FAT ();

	bool initialize (IStorageDevice *p);

	uint32_t getBytesPerSector ();
	uint32_t getSectorsPerCluster ();
	uint32_t getNumberOfDirEntry ();
	uint32_t getNumberOfClusters ();
	uint32_t getRootDirectoryEntry ();

	uint8_t* readSectors (uint32_t c, uint32_t s, uint32_t d, uint32_t *sects, uint32_t *last);
	uint32_t allocateCluster (uint32_t cluster, uint32_t count);
	void freeCluster (uint32_t cluster);
	void setEndOfCluster(uint32_t cluster);

	bool read (uint32_t lba, uint8_t *bf);
	bool write (uint32_t lba, uint8_t *bf);

	uint32_t getLbaFromCluster (uint32_t cluster);
	uint32_t getClusterFromLba (uint32_t lba);
	uint32_t getNextCluster (uint32_t cluster);

private:
	void setNextCluster (uint32_t cluster, uint32_t next);
	uint32_t searchFreeCluster (uint32_t cluster);
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
	uint32_t last;
	FAT *fat;
	FatDirectory *parent;
	uint8_t *file;
	uint8_t *flag;
	uint32_t *lba;
	uint32_t fsize;
	uint32_t sectors;
	uint32_t pos;
	int entry;
	bool sizeChanged;

public:
	FatFile ();
	~FatFile ();

	bool initialize (FAT *p, FatDirectory *d, int e, uint32_t c, uint32_t s);

	uint32_t read (uint8_t *bf, uint32_t sz);
	uint32_t write (uint8_t *bf, uint32_t sz);
	bool seek (int pt, int flag);
	bool flush ();
	bool resize (uint32_t sz);
	uint32_t position ();
	uint32_t size ();

private:
	bool expandClusters (uint32_t sz);
	void reduceClusters (uint32_t sz);
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
		MARK_UNUSED = 0x00,
		RESIZE_DELTA = 1
	};

	uint32_t start;
	uint32_t last;
	FAT *fat;
	uint8_t *entrys;
	uint8_t *unused;
	uint8_t *end;
	uint32_t *lba;
	uint32_t sectors;

public:
	FatDirectory ();
	~FatDirectory ();

	virtual bool initialize (FAT *p, uint32_t cluster) = 0;

	int searchEntry (uint8_t *bf);
	int getEntryName (int entry, uint8_t *bf);
	int getHeadEntry ();
	int getNextEntry (int entry);
	bool deleteEntry (int entry);

	int newDirectory (uint8_t *bf);
	int newFile (uint8_t *bf, uint32_t sz);

	Directory* getDirectory (int entry);
	File* getFile (int entry);
	bool isDirectory (int entry);
	bool isFile (int entry);
	uint32_t getIdentifer ();

	bool setFileSize (int entry, uint32_t size);
	bool setCluster (int entry, uint32_t cluster);

protected:
	uint8_t* searchUnusedEntry ();

private:
	bool isValid (uint8_t *ent);
	void expandFileName (uint8_t *name, uint8_t *bf);
	bool clearDirectory (int entry);
	int newEntry (uint8_t *bf, uint32_t sz, uint8_t attr, uint32_t fsize);
	void setEntry (uint8_t *ent, uint8_t *n, uint8_t a,uint16_t , uint32_t s);
	int searchFreeEntry ();
	bool expandEntry ();
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatRootDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FatRootDirectory : public FatDirectory
{
public:
	FatRootDirectory () : FatDirectory() {}
	~FatRootDirectory () {}

	bool initialize (FAT *p, uint32_t c);
};

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// FatSubDirectory
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class FatSubDirectory : public FatDirectory
{
public:
	FatSubDirectory () : FatDirectory() {}
	~FatSubDirectory () {}

	bool initialize (FAT *p, uint32_t c);
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

}

//-----------------------------------------------------------------------------
#endif // _FAT_H_ //
