/*!
    \file  elf.h
    \brief utilities for ELF file format

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/09/04 update:$Date$
*/
#ifndef _MONA_ELF_
#define _MONA_ELF_

#ifdef MONA
#include <sys/types.h>
#else
#include "types.h"
#endif

typedef struct
{
    uint8_t  magic[4];      // 0x7F, 'E', 'L', 'F'
    uint8_t  clazz;         // address size 1:32bit / 2:64bit
    uint8_t  uint8_torder;     // 1:little / 2:big
    uint8_t  headerversion; // 1
    uint8_t  pad[9];        // padding
    uint16_t  type;          // 1:relocatable / 2:executable / 3:common / 4:core image
    uint16_t  archtype;      // 2:SPARK / 3:x86 / 4:68K
    uint32_t fileversion;   // 1
    uint32_t entrypoint;    // entry point for executable
    uint32_t phdrpos;       // position at program header
    uint32_t shdrpos;       // position at section header
    uint32_t flag;          // 0
    uint16_t  hdrsize;       // ELF header size
    uint16_t  phdrent;       // program header entry size
    uint16_t  phdrcnt;       // program header entry count
    uint16_t  shdrent;       // section header entry size
    uint16_t  shdrcnt;       // section header entry count
    uint16_t  strsec;        // section number to string
} ELFHeader;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t virtualaddr;
    uint32_t physaddr;
    uint32_t filesize;
    uint32_t memorysize;
    uint32_t flags;
    uint32_t align;
} ELFProgramHeader;

typedef struct {
    uint32_t name;
    uint32_t type;
    uint32_t flags;
    uint32_t address;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t align;
    uint32_t entsize;
} ELFSectionHeader;

typedef struct
{
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint8_t type:4;
    uint8_t bind:4;
    uint8_t unused;
    uint16_t section;
} ELFSymbolEntry;

typedef struct
{
    uint32_t offset;
    uint32_t indexType;
} ELFRelocationEntry;

#endif
