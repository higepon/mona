/*!
  \file   main.cpp
  \brief  Mona OS. ELF relocation parser

  Copyright (c) 2002- 2004 HigePon
  WITHOUT ANY WARRANTY

  \author  HigePon
  \version $Revision$
  \date   create:2004/05/02 update:$Date$
*/

#include "elfparser.h"

#ifdef _DEBUG_ELF
#ifdef MONA
#include <monapi.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#endif
#endif

#define ASSERT(cond) if (!cond) { printf("%s:%d: null assert!!\n", __FILE__, __LINE__); exit(1); }


ELFParser::ELFParser()
{
}

ELFParser::~ELFParser()
{
}

bool ELFParser::Set(byte* elf, dword size)
{
    if (elf == NULL) return false;
    if (size < sizeof(Elf32_Ehdr)) return false;

    this->elfSize = size;
    this->header  = (Elf32_Ehdr*)elf;
    SetType();

    return true;
}

void ELFParser::SetType()
{
    if (this->header == NULL)
    {
        this->type =  ET_NONE;
        return;
    }

    /* check Magic Number */
    if (this->header->e_ident[EI_MAG0] != 0x7F
        || this->header->e_ident[EI_MAG1] != 'E'
        || this->header->e_ident[EI_MAG2] != 'L'
        || this->header->e_ident[EI_MAG3] != 'F')
    {
        this->type = ET_NONE;
        return;
    }

    /* 32bits, Intel 80386 */
    if (this->header->e_ident[EI_CLASS] != ELFCLASS32 || this->header->e_machine != EM_386)
    {
        this->type = ET_NOT_SUPPORTED;
        return;
    }

    /* type */
    if (this->header->e_type == ET_REL || this->header->e_type == ET_EXEC)
    {
        this->type = this->header->e_type;
        return;
    }

    this->type = ET_NOT_SUPPORTED;
    return;
}

int ELFParser::GetType() const
{
    return this->type;
}


bool ELFParser::Parse(Elf32_Addr base /* = 0 */)
{
    bool result;

    switch(GetType())
    {
    case ET_EXEC:
        result = ParseExecutable();
        break;

    case ET_REL:
        result = ParseRelocatable(base);
        break;
    default:
        result = false;
        break;
    }
    return result;
}

bool ELFParser::ParseRelocatable(Elf32_Addr base)
{

    return true;
}

bool ELFParser::ParseExecutable()
{
    /* Program Header */
    dword phdrend = this->header->e_phoff + this->header->e_phentsize * this->header->e_phnum;
    if (this->elfSize < phdrend) return false;

    /* Section Header */
    dword shdrend = this->header->e_shoff + this->header->e_shentsize * this->header->e_shnum;
    if (this->elfSize < shdrend) return false;

    this->sheader = (Elf32_Shdr*)((dword)header + header->e_shoff);
    this->pheader = (Elf32_Phdr*)((dword)header + header->e_phoff);

    /* find min of start address, and max of end address */
    Elf32_Addr startAddr = 0xFFFFFFFF;
    Elf32_Addr endAddr   = 0;

    for (int i = 0; i < this->header->e_phnum; i++)
    {
        Elf32_Phdr p = this->pheader[i];
        if (p.p_type != PT_LOAD) continue;

        if (p.p_vaddr < startAddr) startAddr = p.p_vaddr;
        if ((p.p_vaddr + p.p_memsz) > endAddr) endAddr = p.p_vaddr + p.p_memsz;
    }

    for (int i = 0; i < this->header->e_shnum; i++)
    {
        Elf32_Shdr s = this->sheader[i];
        if (!(s.sh_flags & ALLOC)) continue;

        if (s.sh_addr < startAddr) startAddr = s.sh_addr;
        if ((s.sh_addr + s.sh_size) > endAddr) endAddr = s.sh_addr + s.sh_size;
    }

    /* image size of ELF */
    this->imageSize = endAddr - startAddr;
    if (this->imageSize <= 0) return false;
    return true;
}

bool ELFParser::CreateImage(byte* to)
{
    /* this action includes .bss zero CLEAR */
    memset(to, 0, this->imageSize);


//             for (int i = 0; i < this->header->shdrcnt; i++)
//             {
//                 ELFSectionHeader s = this->sheader[i];
//                 if (s.type != SHT_PROGBITS) continue;

//                 memcpy(&image[s.address - this->startAddr], &this->elf[s.offset], s.size);

    return true;
}
