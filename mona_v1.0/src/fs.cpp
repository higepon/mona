/*!
    \file  fs.cpp
    \brief file system

    Copyright (c) 2002-2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2004/03/12 update:$Date$
*/

#include <FSOperation.h>

/*----------------------------------------------------------------------
    FSOperation
----------------------------------------------------------------------*/
FSOperation::FSOperation()
{
    this->file       = NULL;
    this->dir        = NULL;
    this->current    = NULL;
    this->fat        = NULL;
    this->device     = NULL;
    this->errorNo    = FS_NO_ERROR;
    this->isOpenFlag = false;
}

bool FSOperation::initialize(IStorageDevice* device)
{
    this->device = device;
    this->device->open();

    this->fat = new FatStorage();
    checkMemoryAllocate(this->fat, "FatStorage : memory error");

    if (!(this->fat->initialize(device)))
    {
        this->errorNo = FS_INIT_ERROR;
        delete this->fat;
        return false;
    }

    this->current = this->fat->getRootDirectory();

    if (this->current == NULL)
    {
        this->errorNo = FS_GET_ROOT_ERROR;
        delete this->fat;
        return false;
    }
    return true;
}

FSOperation::~FSOperation()
{
    this->device->close();
    delete fat;
}

bool FSOperation::isOpen() const
{
    return this->isOpenFlag;
}

int FSOperation::getErrorNo() const
{
    return this->errorNo;
}

int FSOperation::size()
{
    if (!isOpenFlag || file == NULL)
    {
        return 0;
    }
    return file->size();
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
void FSOperation::freeDirectory(Directory *p)
{
    if (p->getIdentifer() == this->current->getIdentifer())
    {
        if (p != this->current)
        {
            delete this->current;
            this->current = p;
        }
    }
    else
    {
        delete p;
    }
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
Directory* FSOperation::trackingDirectory (char *path, int *cursor)
{
    Directory *p = this->current;
    int i = *cursor;
    int j;

    if ('/' == path[i]) {
        p = this->fat->getRootDirectory();
        i++;
    }

    while ('\0' != path[i]) {
        for (j = i; '\0' != path[j]; j++)
            if ('/' == path[j])
                break;
        int next = j + 1;
        if ('\0' == path[j])
            next = j;
        path[j] = '\0';

        char *name = path+i;

        if (0 == strcmp(name, "."))
            name = "..";
        else if (0 == strcmp(name, ".."))
            name = "...";

        int entry = p->searchEntry((byte*)name);

        if (j != next)
            path[j] = '/';

        if (-1 == entry)
            break;

        Directory *tmp = p->getDirectory(entry);

        if (NULL == tmp) {
            freeDirectory(p);
            return NULL;
        }

        freeDirectory(p);
        p = tmp;

        i = next;
    }

    *cursor = i;

    return p;
}

/*!
    \brief

    \author Gaku & Higepon
    \date   create: update:
*/
bool FSOperation::createFile(char* path)
{
    int entry, cursor = 0;

    Directory *p = searchFile(path, &entry, &cursor);
    if (NULL == p)
    {
        this->errorNo = FS_DIR_NOT_EXIST_ERROR;
        return false;
    }

    if (-1 != entry)
    {
        this->errorNo = FS_FILE_EXIST;
        freeDirectory(p);
        return false;
    }

    entry = p->newFile((byte*)path+cursor, 0);
    if (-1 == entry)
    {
        this->errorNo = FS_FILE_CREATE_ERROR;
        freeDirectory(p);
        return false;
    }

    freeDirectory(p);
    return true;
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
Directory* FSOperation::searchFile (char *path, int *entry, int *cursor)
{
    Directory *p = this->current;
    int index = -1;

    for (int i = 0; '\0' != path[i]; i++) {
        if ('/' == path[i])
            index = i;
    }

    *cursor = 0;

    if (-1 != index) {
        path[index] = '\0';

        char *dir = path;
        if (0 == index)
            dir = "/";

        int tmp = 0;

        p = trackingDirectory(dir, &tmp);
        if (NULL == p) {
            this->errorNo = FS_GET_DIR_ERROR;
            return NULL;
        }

        if ('\0' != dir[tmp]) {
            this->errorNo = FS_DIR_NOT_EXIST_ERROR;
            freeDirectory(p);
            return NULL;
        }

        *cursor = index + 1;
    }

    *entry = p->searchEntry((byte*)path+*cursor);

    return p;
}

bool FSOperation::open(char* path, int mode)
{
    int entry;
    int cursor;

    if (this->isOpenFlag)
    {
        this->errorNo = FS_ALREADY_OPEN_ERROR;
        return false;
    }

    dir = searchFile(path, &entry, &cursor);

    if (dir == NULL)
    {
        this->errorNo = FS_FILE_NOT_FOUND;
        return false;
    }

    this->file = dir->getFile(entry);

    if (file == NULL)
    {
        this->errorNo = FS_FILE_OPEN_ERROR;
        freeDirectory(dir);
        return false;
    }

    this->mode = mode;

    switch(mode)
    {
    case FILE_OPEN_APPEND_WRITE:
        this->file->seek(0, SEEK_END);
        break;

    case FILE_OPEN_NORMAL_WRITE:
        break;

    case FILE_OPEN_READ:
        break;

    default:
        break;
    }

    this->isOpenFlag = true;
    return true;
}

bool FSOperation::close()
{
    if (this->mode == FILE_OPEN_NORMAL_WRITE || this->mode == FILE_OPEN_APPEND_WRITE)
    {
        this->file->flush();
    }
    delete this->file;
    this->file = NULL;
    this->isOpenFlag = false;
    return true;
}

bool FSOperation::read(byte* buf, int size)
{
    if (!isOpenFlag)
    {
        this->errorNo = FS_FILE_IS_NOT_OPEN;
        return false;
    }

    if (!this->file->read(buf, size))
    {
        freeDirectory(dir);
        return false;
    }
    return true;
}

bool FSOperation::write(byte* buf, int size)
{
    if (!isOpenFlag)
    {
        this->errorNo = FS_FILE_IS_NOT_OPEN;
        return false;
    }

    if (!this->file->write(buf, size))
    {
        freeDirectory(dir);
        return false;
    }
    return true;
}
