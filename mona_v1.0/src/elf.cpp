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
#include <global.h>
#include <io.h>
#include <syscalls.h>

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

    g_console->printf("header_ = %x", header_);

    for (int i = 0; i < header_->phdrcnt; i++) {

        if (pheader_[i].type == PT_LOAD && pheader_[i].filesize == pheader_[i].memorysize) {

            enter_kernel_lock_mode();
            g_console->printf("\n\n\n\n\n@%x@ $%x$", (void*)(toAddress + pheader_[i].virtualaddr - header_->entrypoint), (void*)((dword)header_ + pheader_[i].offset));
            exit_kernel_lock_mode();

            memcpy((void*)(toAddress + pheader_[i].virtualaddr - header_->entrypoint), (void*)((dword)header_ + pheader_[i].offset), pheader_[i].filesize);

        } else if (pheader_[i].type = PT_LOAD && pheader_[i].filesize != pheader_[i].memorysize) {

            g_console->printf("+++++++++++\n");
            memset((void*)(toAddress + pheader_[i].virtualaddr - header_->entrypoint), 0, pheader_[i].memorysize);
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
