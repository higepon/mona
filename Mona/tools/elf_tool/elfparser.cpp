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

bool ELFParser::Relocate(byte* base)
{
    for (int i = 0; i < this->header->e_shnum; i++)
    {
        Elf32_Shdr s = this->sheader[i];

        if (s.sh_type != SHT_REL) continue;

        int entryNumber = s.sh_size / s.sh_entsize;
        Elf32_Rel* entries = (Elf32_Rel*)((dword)this->header + s.sh_offset);
        for (int j = 0; j < entryNumber; j++)
        {
            bool result = Relocate(&s, &entries[j]);

            if (!result) return false;
        }
    }

    return true;
}

bool ELFParser::Relocate(Elf32_Shdr* relHeader, Elf32_Rel* relEntry)
{
    if (relHeader->sh_info >= header->e_shnum)
    {
        printf("sh_info:index out\n");
        return false;
    }

    /* target for relocation */
    Elf32_Shdr* targetSect = &this->sheader[relHeader->sh_info];

    /* pointer to relocate value */
    Elf32_Addr* target = (Elf32_Addr*)((dword)this->header + targetSect->sh_offset + relEntry->r_offset);

    if (relHeader->sh_link  >= header->e_shnum)
    {
        printf("sh_link:index out\n");
        return false;
    }

    /* find sym table */
    Elf32_Shdr* symSect = &this->sheader[relHeader->sh_link];
    int symMaxIndex = symSect->sh_size / symSect->sh_entsize;
    Elf32_Sym* symTable = (Elf32_Sym*)((dword)this->header + symSect->sh_offset);

    int symIndex = ELF32_R_SYM(relEntry->r_info);
    if (symIndex >= symMaxIndex)
    {
        printf("symIndex:index out\n");
        return false;
    }

    if (symTable[symIndex].st_shndx == 0)
    {
        printf("symTable: external symbol\n");
        return false;
    }

    Elf32_Addr A = *target;
    Elf32_Addr S = symTable[symIndex].st_value;
    Elf32_Addr P = (Elf32_Addr)target;

    switch(ELF32_R_TYPE(relEntry->r_info))
    {
    case(R_386_NONE):

        /* none */
        break;
    case(R_386_32):

        /* S + A */
        *target = S + A;
        printf("value=%x S=%x A=%x\n", *target, S, A);
        break;
    case(R_386_PC32):

        /* S + A - P */
        *target = S + A - P;
        break;
    case(R_386_GOT32):

        /* G + A - P */
        break;
    case(R_386_PLT32):

        /* L + A - P */
        break;
    case(R_386_COPY):

        /* none */
        break;
    case(R_386_GLOB_DAT):

        /* S */
        break;
    case(R_386_JMP_SLOT):

        /* S */
        break;
    case(R_386_RELATIVE):

        /* B + A */
        break;
    case(R_386_GOTOFF):

        /* S + A - GOT */
        break;
    case(R_386_GOTPC):

        /* GOT + A - P */
        break;
    default:
        printf("relocation error?\n");
        break;
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
    if (this->header == NULL) return 0;
    return this->header->e_entry;
}

/* relocation pattern */

  /* relocate       */
  /* get image size */
  /* load           */

/* execute pattern */

  /* get image size */
  /* load           */
