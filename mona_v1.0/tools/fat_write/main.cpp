/*!
  \file   main.cpp
  \brief  tool for Mona os. create FAT12 boot fd image.

  fat operation is based on fat.cpp by Gaku
  Licence : see http://gaku.s12.xrea.com/wiki/main.cgi?c=g&p=Gaku%2FLicence

  Copyright (c) 2002- 2004 HigePon
  WITHOUT ANY WARRANTY

  \author  HigePon
  \version $Revision$
  \date   create:2004/03/08 update:$Date$
*/
#include <stdlib.h>
#include "string.h"
#include "fat.h"
#include "VirtualFloppy.h"

struct WriteInf
{
    FILE* fp;
    IStorageDevice* device;
    FatStorage* fat;
    Directory* current;
} inf;

/*!
    \brief

    \author Higepon
    \date   create:2003/03/08 update:
*/
bool initialize(char* filename)
{
    inf.fp = fopen(filename, "rb+");
    if (inf.fp == NULL)
    {
        printf("initialize error: fopen()\n");
        return false;
    }

    inf.device = new VirtualFloppy(inf.fp);
    if (inf.device == NULL)
    {
        printf("initialize error: VirtualFloppy\n");
        return false;
    }

    inf.device->open();

    inf.fat = new FatStorage();
    if (inf.fat == NULL)
    {
        printf("initialize error: FatStorage\n");
        return false;
    }

    if (!inf.fat->initialize(inf.device))
    {
        printf("initialize error: FatStorage initialize\n");
        return false;
    }

    inf.current = inf.fat->getRootDirectory();
    if (inf.current == NULL)
    {
        printf("getRootDirectory error\n");
        return false;
    }

    return true;
}

/*!
    \brief

    \author Higepon
    \date   create:2003/03/08 update:
*/
bool finalize()
{
    if (inf.fp != NULL)
    {
        fclose(inf.fp);
    }

    inf.device->close();
    return true;
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
void freeDirectory (Directory *p)
{
    if (p->getIdentifer() == inf.current->getIdentifer()) {
        if (p != inf.current) {
            delete inf.current;
            inf.current = p;
        }
    } else {
        delete p;
    }
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
Directory* trackingDirectory (char *path, int *cursor)
{
    Directory *p = inf.current;
    int i = *cursor;
    int j;

    if ('/' == path[i]) {
        p = inf.fat->getRootDirectory();
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

    \author Gaku
    \date   create: update:
*/
Directory* searchFile (char *path, int *entry, int *cursor)
{
    Directory *p = inf.current;
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
            printf("can not get directory\n");
            return NULL;
        }

        if ('\0' != dir[tmp]) {
            printf("directory not exist\n");
            freeDirectory(p);
            return NULL;
        }

        *cursor = index + 1;
    }

    *entry = p->searchEntry((byte*)path+*cursor);

    return p;
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
bool saveImage (char *path, byte *bf, dword size)
{
    int entry, cursor;

    Directory *p = searchFile(path, &entry, &cursor);
    if (NULL == p)
        return false;

    if (-1 != entry) {
        printf("file already exist error\n");
        freeDirectory(p);
        return false;
    }

    entry = p->newFile((byte*)path+cursor, size);
    if (-1 == entry) {
        printf("can not create file\n");
        freeDirectory(p);
        return false;
    }

    File *df = p->getFile(entry);
    if (NULL == df) {
        printf("can not open file\n");
        freeDirectory(p);
        return false;
    }

    if (false == df->write(bf, size)) {
        printf("write error\n");
        delete df;
        freeDirectory(p);
        return false;
    }

    delete df;
    freeDirectory(p);

    return true;
}

/*!
    \brief

    \author Higepon
    \date   create:2003/03/08 update:
*/
byte* loadFromFile(char* path, dword* size)
{
    byte* buf;
    FILE* fp;

    fp = fopen(path, "rb");
    if (fp == NULL)
    {
        printf("loadFromFile: file not found\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buf = new byte [*size];
    if (buf == NULL)
    {
        printf("loadFromFile: memory allocate error\n");
        fclose(fp);
        return NULL;
    }

    fread(buf, 1, *size, fp);

    fclose(fp);
    return buf;
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
void rm (char *path)
{
    int entry, cursor;

    Directory *p = searchFile(path, &entry, &cursor);
    if (NULL == p)
    {
        return;
    }

    if (entry == -1)
    {
        return;
    }

    if (false == p->deleteEntry(entry)) {
        freeDirectory(p);
        return;
    }
    freeDirectory(p);
}

bool cp (char *src, char *dst)
{
    dword size = 0;
    byte *buf = loadFromFile(src, &size);
    if (buf == NULL)
    {
        return false;
    }

    if (!saveImage(dst, buf, size))
    {
        delete buf;
        return false;
    }

    delete buf;
    return true;
}

/*!
    \brief

    \author Higepon
    \date   create:2003/03/08 update:
*/
int main(int argc, char *argv[])
{
    char destFile1[128];
    char destFile2[128];

    if (argc != 4) {
        printf("usage: fat_write.exe fdimage \n");
        exit(-1);
    }

    strcpy(destFile1, argv[3]);
    strcpy(destFile2, argv[3]);

    if (!initialize(argv[1]))
    {
        return -1;
    }

    rm(destFile1);

    if (!cp(argv[2], destFile2))
    {
        return -1;
        finalize();
    }

    finalize();
    return 0;
}
