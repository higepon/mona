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

    this->header = (Elf32_Ehdr*)elf;
    this->sheader = (Elf32_Shdr*)((dword)elf + header->e_shoff);
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
