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

#include "types.h"

typedef dword Elf32_Addr;
typedef word  Elf32_Half;
typedef dword Elf32_Off;
typedef long  Elf32_Sword;
typedef dword Elf32_Word;

#define EI_NIDENT 16

typedef struct
{
    byte       e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off  e_phoff;
    Elf32_Off  e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct
{
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off  sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct
{
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

typedef struct
{
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct
{
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    byte       st_info;
    byte       st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((byte)(i))

class ELFParser
{
public:
    ELFParser();
    virtual ~ELFParser();

public:
    bool Set(byte* elf, dword size);
    int GetType() const;
    bool Parse(Elf32_Addr base = 0);
    dword GetImageSize() const;
    bool CreateImage(byte* to);

private:
    void SetType();
    bool Relocate(Elf32_Addr base);
    bool Relocate(Elf32_Addr base, Elf32_Shdr* relHeader, Elf32_Rel* relEntry);
    bool ParseExecutable();
    bool ParseRelocatable(Elf32_Addr base);

private:
    int type;
    dword elfSize;
    Elf32_Addr  imageSize;
    Elf32_Addr  startAddr;
    Elf32_Addr  endAddr;
    Elf32_Ehdr* header;
    Elf32_Shdr* sheader;
    Elf32_Phdr* pheader;

public:

    enum
    {
        EI_MAG0    = 0,
        EI_MAG1    = 1,
        EI_MAG2    = 2,
        EI_MAG3    = 3,
        EI_CLASS   = 4,
    };

    enum
    {
        EM_386 = 3
    };

    enum
    {
        ELFCLASS32 = 1
    };

    enum
    {
        EV_CURRENT = 1
    };

    enum
    {
        ET_NONE          = 0,
        ET_REL           = 1,
        ET_EXEC          = 2,
        ET_DYN           = 3,
        ET_CORE          = 4,
        ET_NOT_SUPPORTED = 6
    } e_type;

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
        SHT_NULL     = 0,
        SHT_PROGBITS = 1,
        SHT_SYMTAB   = 2,
        SHT_STRTAB   = 3,
        SHT_RELA     = 4,
        SHT_HASH     = 5,
        SHT_DYNAMIC  = 6,
        SHT_NOTE     = 7,  /* OS defined           */
        SHT_NOBITS   = 8,  /* bss                  */
        SHT_REL      = 9,
        SHT_SHLIB    = 10, /* reserved. DO NOT USE */
        SHT_DYNSYM   = 11
    } SectionType;

    enum
    {
        WRITABLE   = 0x01,
        ALLOC      = 0x02,
        EXECUTABLE = 0x04
    } SectionFlags;

    enum
    {
        R_386_NONE     = 0,
        R_386_32       = 1,
        R_386_PC32     = 2,
        R_386_GOT32    = 3,
        R_386_PLT32    = 4,
        R_386_COPY     = 5 ,
        R_386_GLOB_DAT = 6,
        R_386_JMP_SLOT = 7,
        R_386_RELATIVE = 8,
        R_386_GOTOFF   = 9,
        R_386_GOTPC    = 10
    };

};

#endif  // __MONA_ELF_PARSER_H__
