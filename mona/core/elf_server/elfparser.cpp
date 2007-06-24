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

#ifdef MONA
#include <monapi.h>
#elif defined(_DEBUG_ELF)
#include <stdio.h>
#include <string.h>
#endif

ELFParser::ELFParser() : elf(NULL), symbols(NULL), sectionNames(0), symbolNames(0)
{
}

ELFParser::~ELFParser()
{
}

bool ELFParser::set(uint8_t* elf, uint32_t size)
{
    /* ELF Header */
    if (size < sizeof(ELFHeader)) return false;
    this->header = (ELFHeader*)elf;
    int type = this->getType();
    if (type == TYPE_NOT_ELF) return false;

    /* Program Header */
    uint32_t phdrend = this->header->phdrpos + sizeof(ELFProgramHeader) * this->header->phdrcnt;
    if (size < phdrend) return false;
    this->pheader = (ELFProgramHeader*)&elf[this->header->phdrpos];

    /* Section Header */
    uint32_t shdrend = this->header->shdrpos + sizeof(ELFSectionHeader) * this->header->shdrcnt;
    if (size < shdrend) return false;
    this->sheader = (ELFSectionHeader*)&elf[this->header->shdrpos];

    /* find min of start address, and max of end address */
    this->startAddr = 0xFFFFFFFF;
    this->endAddr   = 0;

    for (int i = 0; i < this->header->phdrcnt; i++)
    {
        ELFProgramHeader p = this->pheader[i];
        if (p.type != PT_LOAD) continue;

        if (p.virtualaddr < this->startAddr) this->startAddr = p.virtualaddr;
        if ((p.virtualaddr + p.memorysize) > this->endAddr) this->endAddr = p.virtualaddr + p.memorysize;
    }

    for (int i = 0; i < this->header->shdrcnt; i++)
    {
        ELFSectionHeader s = this->sheader[i];
        if (!(s.flags & ALLOC)) continue;

        if (s.address < this->startAddr) this->startAddr = s.address;
        if ((s.address + s.size) > this->endAddr) this->endAddr = s.address + s.size;
    }

    /* image size of ELF */
    this->imageSize = this->endAddr - this->startAddr;
    if (this->imageSize <= 0) return false;

    this->elf = elf;
    return true;
}

int ELFParser::getType()
{
    if (this->elf == NULL) return ERR_SIZE;

    /* check magic number */
    if (this->header->magic[0] != 0x7F || this->header->magic[1] != 'E'
        || this->header->magic[2] != 'L' || this->header->magic[3] != 'F')
    {
        return TYPE_NOT_ELF;
    }

    /* little endian, version 1, x86 */
    if (this->header->clazz != 1 || this->header->headerversion != 1 || this->header->archtype != 3)
    {
        return TYPE_NOT_SUPPORTED;
    }

    if (this->header->type == 1) return TYPE_RELOCATABLE;
    else if (this->header->type == 2) return TYPE_EXECUTABLE;

    return TYPE_NOT_SUPPORTED;
}

const char* ELFParser::getSectionName(uint32_t index)
{
    if (this->sectionNames == 0) return "";
    if (index >= this->sheader[sectionNames].size) return "";

    return (const char*)&this->elf[this->sheader[sectionNames].offset + index];
}

const char* ELFParser::getSymbolName(uint32_t index)
{
    if (this->symbolNames == 0) return "";
    if (index >= this->sheader[symbolNames].size) return "";

    return (const char*)&this->elf[this->sheader[symbolNames].offset + index];
}

int ELFParser::parse()
{
    if (this->elf == NULL) return ERR_SIZE;

    /* String Table Symbol Table */
    for (int i = 0, strTableIndex = 0; i < this->header->shdrcnt; i++)
    {
        ELFSectionHeader s = this->sheader[i];

        if (s.type == SYMTAB) symbols = (ELFSymbolEntry*)&this->elf[s.offset];

        if (s.type != STRTAB) continue;

        if (strTableIndex == 0)
        {
            sectionNames = i;
            strTableIndex++;
        }
        else if (strTableIndex == 1)
        {
            symbolNames = i;
            strTableIndex++;
        }
    }

#ifdef _DEBUG_ELF
    for (int i = 0; i < this->header->phdrcnt; i++)
    {
        ELFProgramHeader p = this->pheader[i];
        _printf("pheaders[%2d].virtualaddr=%08x physaddr=%08x type=%2d\n",
            i, p.virtualaddr, p.physaddr, p.type);
    }

    for (int i = 0; i < this->header->shdrcnt; i++)
    {
        ELFSectionHeader s = this->sheader[i];
        _printf("sheaders[%2d].address=%08x size=%8x type=%2d %s\n",
            i, s.address, s.size, s.type, getSectionName(s.name));
    }
#endif




//     for (int i = 0; i < this->header->shdrcnt; i++)
//     {
//         ELFSectionHeader s = this->sheader[i];

//         if (s.type == SYMTAB)
//         {
//             int numEntry = s.size / sizeof(ELFSymbolEntry);
//#ifdef _DEBUG_ELF
//             _printf("symbol numentry = %d\n", numEntry);
//#endif

//             ELFSymbolEntry* sym = (ELFSymbolEntry*)((uint32_t)(this->header) + s.offset);
//#ifdef _DEBUG_ELF
//             _printf("value = %x\n", sym[8].value);
//             _printf("size  = %x\n", sym[8].size);
//#endif

//         }
//     }


    for (int i = 0; i < this->header->shdrcnt; i++)
    {
        ELFSectionHeader s = this->sheader[i];

        if (s.type == REL)
        {
            int num = s.size / sizeof(ELFRelocationEntry);
            ELFRelocationEntry* rel = (ELFRelocationEntry*)((uint32_t)(this->header) + s.offset);

            for (int j = 0; j < num; j++)
            {
                ELFSymbolEntry entry = symbols[(rel[j].indexType) >> 8];

#ifdef _DEBUG_ELF
                _printf("relocate offset=%8x symbol [%10s]",
                    rel[j].offset, getSectionName(this->sheader[entry.section].name));
                _printf("value = %8x size = %8x %s",
                    entry.value, entry.size, getSymbolName(entry.name));
#endif

#ifdef _DEBUG_ELF
                uint32_t* p = (uint32_t*)&this->elf[this->sheader[entry.section].offset + rel[j].offset];
                _printf("relocate target address = %8x\n", *p);
#endif
            }
        }
    }

    return 0;
}

bool ELFParser::load(uint8_t* image)
{
    switch (this->getType())
    {
        case TYPE_EXECUTABLE:
            memset(image, 0, this->imageSize);
            for (int i = 0; i < this->header->phdrcnt; i++)
            {
                ELFProgramHeader p = this->pheader[i];
                if (p.type != PT_LOAD) continue;

                memcpy(&image[p.virtualaddr - this->startAddr], &this->elf[p.offset], p.filesize);
            }
            return true;
        case TYPE_RELOCATABLE:
            break;
    }

    return false;
}
