/*!
    \file  elf.cpp
    \brief elf utility

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/09/06 update:$Date$
*/

#include "elf.h"

#define ELF_ERROR_NOT_ELF           -1
#define ELF_ERORR_NOT_SUPPORTED_ELF -2
#define ELF_ERORR_NOT_EXECUTABLE    -3

const char* errorNames[] =
{
    "ELF_ERROR_NOT_ELF",
    "ELF_ERORR_NOT_SUPPORTED_ELF",
    "ELF_ERORR_NOT_EXECUTABLE"
};

#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6

#define SHARED_MM_ERROR -1
#define FAT_INIT_ERROR  -2
#define FAT_OPEN_ERROR  -3

#define MAX_IMAGE_SIZE (4096 * 150)
#define MONAGED_COPY(dst, dp, array, ap, size) { \
    for (dword i = 0; i < size; i++) dst[dp + i] = array[ap + i]; }
#define MONAGED_COPY2(dst, dp, dsz, array, ap, size) { \
    _A<byte> _dst((byte*)dst.get(), dsz, false); \
    for (dword i = 0; i < size; i++) _dst[dp + i] = array[ap + i]; }
#define MONAGED_FILL(dst, pos, val, size) { \
    for (dword i = 0; i < size; i++) dst[pos + i] = val; }

#define ORG 0xA0000000

ELFLoader::ELFLoader(_A<byte> elf) : elf(elf), imageSize(0), errorCode(0)
{
    this->prepare();
}

ELFLoader::~ELFLoader()
{
}

const char* ELFLoader::getErrorName() const
{
    if (this->errorCode == 0) return NULL;
    return errorNames[-1 - this->errorCode];
}

void ELFLoader::prepare()
{
    /* check ELF header */
    _A<byte> header((byte*)&this->header, sizeof(this->header), false);
    for (int i = 0; i < header.get_Length(); i++) header[i] = this->elf[i];
    if (!isValidELF()) return;

    /* Program Header */
    dword plen = sizeof(ELFProgramHeader) * this->header.phdrcnt;
    this->pheaders.Set(this->header.phdrcnt);
    MONAGED_COPY2(this->pheaders, 0, plen, this->elf, this->header.phdrpos, plen)

    /* Section Header */
    dword slen = sizeof(ELFSectionHeader) * this->header.shdrcnt;
    this->sheaders.Set(this->header.shdrcnt);
    MONAGED_COPY2(this->sheaders, 0, slen, this->elf, this->header.shdrpos, slen)

    /* get size of image */
    int size = 0;
    dword start = ORG;
    FOREACH (ELFSectionHeader, shdr, this->sheaders)
    {
        if (shdr.address >= start)
        {
            start = shdr.address;
            size = start - ORG + 1 + shdr.size;
        }
    }

    if (size > MAX_IMAGE_SIZE)
    {
        printf("alert!!! : ELF size over size = %d \n", size);
    }

    this->imageSize = size;
    return;
}

#define SH_NOBITS 8
dword ELFLoader::load(_A<byte> image)
{
    FOREACH (ELFProgramHeader, phdr, this->pheaders)
    {
#if 1   // yui this->pheaders[0]->virtualaddr != ORG‚Ìê‡‚Éƒ[ƒh‚ÉŽ¸”s‚·‚é
        if (phdr.type != PT_LOAD) continue;

        MONAGED_COPY(image, phdr.virtualaddr - ORG, this->elf, phdr.offset, phdr.filesize)
        if (phdr.memorysize > phdr.filesize)
        {
            int pos = phdr.virtualaddr + phdr.filesize - ORG;
            int len = phdr.memorysize - phdr.filesize;
            printf("%s:%d:[%x+%x(%x)/%x]", __FILE__, __LINE__, pos, len, pos + len, image.get_Length());
            MONAGED_FILL(image, phdr.virtualaddr + phdr.filesize - ORG, 0, phdr.memorysize - phdr.filesize)
            printf("OK!\n");
        }
#else
        if (phdr.type == PT_LOAD && phdr.filesize == phdr.memorysize)
        {
            MONAGED_COPY(image, phdr.virtualaddr - this->pheaders->virtualaddr, this->elf, phdr.offset, phdr.filesize)
        }
        else if (phdr.type == PT_LOAD && phdr.filesize != phdr.memorysize)
        {
            MONAGED_COPY(image, phdr.virtualaddr - this->pheaders->virtualaddr, this->elf, phdr.offset, phdr.memorysize)

            /* zero clear*/
            //MONAGED_FILL(image, phdr.virtualaddr - this->header.entrypoint + phdr.filesize, 0, phdr.memorysize - phdr.filesize)
        }
#endif
    }

    FOREACH (ELFSectionHeader, shdr, this->sheaders)
    {
        /* .bss */
        if (shdr.type != SH_NOBITS) continue;

        MONAGED_FILL(image, shdr.address - ORG, 0, shdr.size)
    }

    return this->header.entrypoint;
}

bool ELFLoader::isValidELF()
{
    /* check magic number */
    if (this->header.magic[0] != 0x7F || this->header.magic[1] != 'E'
        || this->header.magic[2] != 'L' ||this->header.magic[3] != 'F')
    {
        this->errorCode = ELF_ERROR_NOT_ELF;
        return false;
    }

    /* little endian, version 1, x86 */
    if (this->header.clazz != 1 || this->header.headerversion != 1 || this->header.archtype != 3)
    {
        this->errorCode = ELF_ERORR_NOT_SUPPORTED_ELF;
        return false;
    }

    /* check executable */
    if (this->header.type != 2)
    {
        this->errorCode = ELF_ERORR_NOT_EXECUTABLE;
        return false;
    }

    /* valid ELF */
    return true;
}
