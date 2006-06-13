/*!
    \file  FSOperation.h
    \brief class FSOperation

    Copyright (c) 2002-2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2004/03/12 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef _MONA_FS_OPERATION__
#define _MONA_FS_OPERATION__

#ifdef ON_LINUX
#include "fat.h"
#include "_file.h"
#else
#include <fat_write/fat.h>
#include <fat_write/file.h>
#include <sys/error.h>
#include <monapi.h>
#endif

typedef struct LsInfo
{
    int entry;
    FatFS::Directory* p;
};

class FSOperation
{
  public:
    FSOperation();
    virtual ~FSOperation();

  public:
    bool initialize(IStorageDevice* device);
    bool open(char* path, int mode);
    bool close();
    bool read(byte* buf, int size);
    bool write(byte* buf, int size);
    bool createFile(char* path);
    bool isOpen() const;
    bool cd(char* path);
    bool openDir();
    bool readDir(char* name, int* size, int* attribute);
    bool closeDir();
    bool seek(dword pt, int flag);
    dword position();
    int getErrorNo() const;
    int size();

  private:
    void freeDirectory(FatFS::Directory *p);
    FatFS::Directory* trackingDirectory(char *path, int *cursor);
    FatFS::Directory* searchFile(char *path, int *entry, int *cursor);

  private:
    FatFS::File* file;
    FatFS::Directory* dir;
    FatFS::Directory* current;
    FatFS::FatStorage* fat;
    IStorageDevice* device;
    int errorNo;
    int mode;
    bool isOpenFlag;
    LsInfo lsinfo;
};

#endif
