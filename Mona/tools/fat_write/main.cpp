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

using namespace FatFS;

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
bool initialize(const char* filename)
{
    inf.fp = fopen(filename, "rb+");
    if (inf.fp == NULL)
    {
        fprintf(stderr, "initialize error: fopen()\n");
        return false;
    }

    inf.device = new VirtualFloppy(inf.fp);
    if (inf.device == NULL)
    {
        fprintf(stderr, "initialize error: VirtualFloppy\n");
        return false;
    }

    inf.device->open();

    inf.fat = new FatStorage();
    if (inf.fat == NULL)
    {
        fprintf(stderr, "initialize error: FatStorage\n");
        return false;
    }

    if (!inf.fat->initialize(inf.device))
    {
        fprintf(stderr, "initialize error: FatStorage initialize\n");
        return false;
    }

    inf.current = inf.fat->getRootDirectory();
    if (inf.current == NULL)
    {
        fprintf(stderr, "getRootDirectory error\n");
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
void freeDirectory(Directory *p)
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
Directory* trackingDirectory(const char *path_, int *cursor)
{
    Directory *p = inf.current;
    int i = *cursor;
    int j;

    char path[128];
    strncpy(path, path_, sizeof(path));
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
Directory* searchFile(const char *path_, int *entry, int *cursor)
{
    Directory *p = inf.current;
    int index = -1;

    char path[128];
    strncpy(path, path_, sizeof(path));
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
            fprintf(stderr, "can not get directory\n");
            return NULL;
        }

        if ('\0' != dir[tmp]) {
            fprintf(stderr, "directory not exist\n");
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
bool saveImage(const char *path, byte *bf, dword size)
{
    int entry, cursor;

    Directory *p = searchFile(path, &entry, &cursor);
    if (NULL == p)
        return false;

    if (-1 != entry) {
        fprintf(stderr, "file already exist error\n");
        freeDirectory(p);
        return false;
    }

    entry = p->newFile((byte*)path+cursor, size);
    if (-1 == entry) {
        fprintf(stderr, "can not create file\n");
        freeDirectory(p);
        return false;
    }

    File *df = p->getFile(entry);
    if (NULL == df) {
        fprintf(stderr, "can not open file\n");
        freeDirectory(p);
        return false;
    }

    if (false == df->write(bf, size)) {
        fprintf(stderr, "write error\n");
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
byte* loadFromFile(const char* path, dword* size)
{
    byte* buf;
    FILE* fp;

    fp = fopen(path, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "loadFromFile: file not found\n");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);

    if (size == 0)
    {
        fclose(fp);
        return NULL;
    }

    fseek(fp, 0, SEEK_SET);

    buf = new byte [*size];
    if (buf == NULL)
    {
        fprintf(stderr, "loadFromFile: memory allocate error\n");
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
void touch(const char *path)
{
    int entry, cursor = 0;

    Directory *p = searchFile(path, &entry, &cursor);
    if (NULL == p)
        return;

    if (-1 != entry) {
        fprintf(stderr, "touch:file already exist!\n");
        freeDirectory(p);
        return;
    }

    entry = p->newFile((byte*)path+cursor, 0);
    if (-1 == entry) {
        fprintf(stderr, "touch: can not create file\n");
        freeDirectory(p);
        return;
    }

    freeDirectory(p);
}

/*!
    \brief

    \author Gaku
    \date   create: update:
*/
void rm(const char *path)
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

bool cp(const char *src, const char *dst)
{
    dword size = 0;
    byte *buf = loadFromFile(src, &size);

    if (size == 0)
    {
        touch(dst);
        return true;
    }

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

bool read(const char* path, byte* buf, int size)
{
    int entry;
    int cursor;

    Directory *p = searchFile(path, &entry, &cursor);

    if (p == NULL)
    {
        return false;
    }

    File *df = p->getFile(entry);

    if (df == NULL)
    {
        fprintf(stderr, "read: can not open file\n");
        freeDirectory(p);
        return false;
    }

    fprintf(stderr, "size = %d bytes", df->size());

    if (!df->read(buf, size))
     {
        fprintf(stderr, "write error\n");
        freeDirectory(p);
        return false;
    }

    delete df;

    return true;
}

/*!
    \brief

    \author Tino
    \date   create:2004/04/27 update:
*/
bool mkdir(const char *path)
{
    int tmp = 0;
    Directory* d = trackingDirectory(path, &tmp);
    if (d == NULL)
    {
        fprintf(stderr, "mkdir: can not create directory\n");
        return false;
    }

    bool ret = false;
    if (path[tmp] != '\0')
    {
        char path_[128];
        strncpy(path_, path, 128);
        int entry = d->newDirectory((byte*)&path_[tmp]);
        if (entry != -1) ret = true;
    }
    if (!ret) fprintf(stderr, "mkdir: can not create directory\n");
    freeDirectory(d);
    return ret;
}

/*!
    \brief

    \author Tino
    \date   create:2004/04/27 update:
*/
bool writeMBR(const char *src)
{
    FILE* fp = fopen(src, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "writeMBR: can not open file\n");
        return false;
    }

    byte buf[512];
    for (int i = 0; i < 512; i++) buf[i] = 0;
    fread(buf, sizeof(byte), 512, fp);
    fclose(fp);
    fseek(inf.fp, 0, SEEK_SET);
    fwrite(buf, sizeof(byte), 512, inf.fp);
    return true;
}

/*!
    \brief

    \author Higepon
    \date   create:2003/03/08 update:2004/04/27
*/
int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("usage: fat_write image source/--mkdir/--mbr path\n");
        return 0;
    }

    if (!initialize(argv[1])) return 1;

//     byte buf[512];
//     memset(buf, 0, 512);
//     read(argv[2], buf, 512);
//     for (int i = 0; i < 512; i++) printf("%x", buf[i]);

    int ret = 0;
    if (strcmp(argv[2], "--mkdir") == 0)
    {
        if (!mkdir(argv[3])) ret = 1;
    }
    else if (strcmp(argv[2], "--mbr") == 0)
    {
        if (!writeMBR(argv[3])) ret = 1;
    }
    else
    {
        rm(argv[3]);
        if (!cp(argv[2], argv[3])) ret = 1;
    }

    finalize();
    return ret;
}
