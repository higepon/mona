/*!
  \file   elfparser.cpp
  \brief  Mona OS. ELF relocation parser

  Copyright (c) 2002- 2004 HigePon
  WITHOUT ANY WARRANTY

  \author  HigePon
  \version $Revision$
  \date   create:2004/05/02 update:$Date$
*/

#ifndef __MONA_ELF_PARSER_H__
#define __MONA_ELF_PARSER_H__

#include "elf.h"

class ELFParser
{
public:
    ELFParser();
    ~ELFParser();

public:
    bool set(byte* elf, dword size);
    int getType();
    int parse();
    bool load(byte* image);

    inline dword getStartAddr()  const { return this->startAddr; }
    inline dword getEndAddr()    const { return this->endAddr; }
    inline dword getImageSize()  const { return this->imageSize; }
    inline dword getEntryPoint() const { return this->header->entrypoint; }

private:
    const char* getSectionName(dword index);
    const char* getSymbolName(dword index);

private:
    byte* elf;
    ELFHeader* header;
    ELFProgramHeader* pheaders;
    ELFSectionHeader* sheaders;
    ELFSymbolEntry* symbols;
    dword sectionNames, symbolNames;
    dword startAddr, endAddr, imageSize;

public:
    enum
    {
        ERR_SIZE           = 6,
        TYPE_NOT_ELF       = 5,
        TYPE_NOT_SUPPORTED = 4,
        TYPE_RELOCATABLE   = 2,
        TYPE_EXECUTABLE    = 1
    };

    enum
    {
        PT_NULL    = 0,
        PT_LOAD    = 1,
        PT_DYNAMIC = 2,
        PT_INTERP  = 3,
        PT_NOTE    = 4,
        PT_SHLIB   = 5,
        PT_PHDR    = 6
    };

    enum
    {
        NULLS    = 0,
        PROGBITS = 1,
        SYMTAB   = 2,
        STRTAB   = 3,
        RELA     = 4,
        HASH     = 5,
        DYNAMIC  = 6,
        NOTE     = 7,  /* OS defined           */
        NOBITS   = 8,  /* bss                  */
        REL      = 9,
        SHLIB    = 10, /* reserved. DO NOT USE */
        DYNSYM   = 11
    } SectionType;

    enum
    {
        WRITABLE   = 0x01,
        ALLOC      = 0x02,
        EXECUTABLE = 0x04
    } SectionFlags;
};

#endif  // __MONA_ELF_PARSER_H__
