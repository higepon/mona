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

#include <elf.h>
#include <string.h>

ELFLoader::ELFLoader() {

}

ELFLoader::~ELFLoader() {

}

int ELFLoader::prepare(dword elf) {

    int size = 0;

    /* check ELF header */
    header_  = (ELFHeader*)elf;
    if (!isValidELF()) return errorCode_;

    /* Program Header */
    pheader_ = (ELFProgramHeader*)((dword)header_ + header_->phdrpos);

    /* get size of image */
    for (int i = 0; i < header_->phdrcnt; i++) {

        if (pheader_[i].type == PT_LOAD) {

            size += pheader_[i].memorysize;
        }
    }

    return size;
}

bool ELFLoader::load(byte* toAddress) {

    for (int i = 0; i < header_->phdrcnt; i++) {

        if (pheader_[i].type == PT_LOAD) {

            memcpy((void*)(toAddress + pheader_->virtualaddr - header_->entrypoint), (void*)((dword)header_ + pheader_->offset), pheader_->filesize);
        }
    }

    return true;
}

int ELFLoader::getErrorCode() const {

    return errorCode_;
}

bool ELFLoader::isValidELF() {

    /* check magic number */
    if (header_->magic[0] != 0x7F || header_->magic[1] != 'E'
        || header_->magic[2] != 'L' ||header_->magic[3] != 'F') {

        errorCode_ = ELF_ERROR_NOT_ELF;
        return false;
    }

    /* little endian, version 1, x86 */
    if (header_->clazz != 1 || header_->headerversion != 1 || header_->archtype != 3) {

        errorCode_ = ELF_ERORR_NOT_SUPPORTED_ELF;
        return false;
    }

    /* check executable */
    if (header_->type != 2) {

        errorCode_ = ELF_ERORR_NOT_EXECUTABLE;
        return false;
    }

    /* valid ELF */
    return true;
}
