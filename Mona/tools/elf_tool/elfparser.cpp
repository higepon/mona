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

ELFParser::ELFParser() : elf(NULL)
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
    this->type   = this->GetType();

    return true;
}

int ELFParser::GetType() const
{
    if (this->elf == NULL) return ET_NONE;

    /* check Magic Number */
    if (this->header->e_ident[EI_MAG0] != 0x7F
        || this->header->e_ident[EI_MAG1] != 'E'
        || this->header->e_ident[EI_MAG2] != 'L'
        || this->header->e_ident[EI_MAG3] != 'F')
    {
        return ET_NONE;
    }

    /* 32bits, Intel 80386 */
    if (this->header->e_ident[EI_CLASS] != ELFCLASS32 || this->header->e_machine != EM_386)
    {
        return ET_NOT_SUPPORTED;
    }

    /* type */
    if (this->header->e_type == ET_REL || this->header->e_type == ET_EXEC)
    {
        return this->header->e_type;
    }

    return ET_NOT_SUPPORTED;
}

bool ELFParser::Relocate(byte* base)
{

    return true;
}

bool ELFParser::Relocate(Elf32_Shdr* relHeader, Elf32_Rel* relEntry)
{
    if (relHeader->sh_info > header->e_shnum - 1)
    {
        printf("sh_info:index out\n");
        return false;
    }

    /* target for relocation */
    Elf32_Shdr* targetSect = &this->sheader[relHeader->sh_info];

    /* pointer to relocate value */
    Elf32_Addr* target = (Elf32_Addr*)((dword)this->header + targetSect->sh_offset + relEntry->r_offset);

    if (relHeader->sh_link > header->e_shnum - 1)
    {
        printf("sh_link:index out\n");
        return false;
    }

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

Elf32_Addr ELFParser::GetEntry() const
{
    if (this->elf == NULL) return 0;
    return this->header->e_entry;
}

/* relocation pattern */

  /* relocate       */
  /* get image size */
  /* load           */

/* execute pattern */

  /* get image size */
  /* load           */
