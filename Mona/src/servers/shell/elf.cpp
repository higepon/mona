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

#define SHARED_MM_ERROR -1
#define FAT_INIT_ERROR  -2
#define FAT_OPEN_ERROR  -3

#define MAX_IMAGE_SIZE (4096 * 150)

#define ORG 0xA0000000

ELFLoader::ELFLoader() : errorCode_(0), header_(NULL), pheader_(NULL)
{
}

ELFLoader::~ELFLoader()
{
}

int ELFLoader::prepare(dword elf)
{
    int size = 0;

    /* check ELF header */
    header_  = (ELFHeader*)elf;
    if (!isValidELF()) return errorCode_;

    /* Program Header */
    pheader_ = (ELFProgramHeader*)((dword)header_ + header_->phdrpos);

    /* Section Header */
    sheader_ = (ELFSectionHeader*)((dword)header_ + header_->shdrpos);

    /* get size of image */
    dword start = ORG;
    for (int i = 0; i < header_->shdrcnt; i++)
    {
        if (sheader_[i].address >= start)
        {
            start = sheader_[i].address;
            size = start - ORG + 1 + sheader_[i].size;
        }
    }

    if (size > MAX_IMAGE_SIZE)
    {
        printf("alert!!! : ELF size over size = %d \n", size);
    }

    return size;
}

#define SH_NOBITS 8
dword ELFLoader::load(byte* toAddress)
{
    for (int i = 0; i < header_->phdrcnt; i++)
    {
#if 1   // yui pheader_[0]->virtualaddr != ORG‚Ìê‡‚Éƒ[ƒh‚ÉŽ¸”s‚·‚é
        if (pheader_[i].type == PT_LOAD) {
            memcpy((void*)(pheader_[i].virtualaddr - ORG + toAddress), (void*)((dword)header_ + pheader_[i].offset), pheader_[i].filesize);
            if (pheader_[i].memorysize > pheader_[i].filesize) {
                memset((void*)(pheader_[i].virtualaddr + pheader_[i].filesize - ORG + toAddress), 0, pheader_[i].memorysize - pheader_[i].filesize);
            }
        }
#else
        if (pheader_[i].type == PT_LOAD && pheader_[i].filesize == pheader_[i].memorysize)
        {
            memcpy((void*)(toAddress + pheader_[i].virtualaddr - pheader_->virtualaddr), (void*)((dword)header_ + pheader_[i].offset), pheader_[i].filesize);
        }
        else if (pheader_[i].type == PT_LOAD && pheader_[i].filesize != pheader_[i].memorysize)
        {
            memcpy((void*)(toAddress + pheader_[i].virtualaddr - pheader_->virtualaddr), (void*)((dword)header_ + pheader_[i].offset), pheader_[i].memorysize);

            /* zero clear*/
            //memset((void*)(toAddress + pheader_[i].virtualaddr - header_->entrypoint + pheader_[i].filesize), 0, pheader_[i].memorysize - pheader_[i].filesize);
        }
#endif
    }

    for (int i = 0; i < header_->shdrcnt; i++)
    {
        /* .bss */
        if (sheader_[i].type == SH_NOBITS)
        {
            memset((void*)(toAddress + sheader_[i].address - ORG), 0, sheader_[i].size);
        }
    }

    return header_->entrypoint;
}

int ELFLoader::getErrorCode() const
{
    return errorCode_;
}

bool ELFLoader::isValidELF()
{
    /* check magic number */
    if (header_->magic[0] != 0x7F || header_->magic[1] != 'E'
        || header_->magic[2] != 'L' ||header_->magic[3] != 'F')
    {
        errorCode_ = ELF_ERROR_NOT_ELF;
        return false;
    }

    /* little endian, version 1, x86 */
    if (header_->clazz != 1 || header_->headerversion != 1 || header_->archtype != 3)
    {
        errorCode_ = ELF_ERORR_NOT_SUPPORTED_ELF;
        return false;
    }

    /* check executable */
    if (header_->type != 2)
    {
        errorCode_ = ELF_ERORR_NOT_EXECUTABLE;
        return false;
    }

    /* valid ELF */
    return true;
}
