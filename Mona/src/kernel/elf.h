/*!
    \file  elf.h
    \brief utilities for ELF file format

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/09/04 update:$Date$
*/
#ifndef _MONA_ELF_
#define _MONA_ELF_

#include <sys/types.h>

typedef struct {
    byte  magic[4];      // 0x7F, 'E', 'L', 'F'
    byte  clazz;         // address size 1:32bit / 2:64bit
    byte  byteorder;     // 1:little / 2:big
    byte  headerversion; // 1
    byte  pad[9];        // padding
    word  type;          // 1:relocatable / 2:executable / 3:common / 4:core image
    word  archtype;      // 2:SPARK / 3:x86 / 4:68K
    dword fileversion;   // 1
    dword entrypoint;    // entry point for executable
    dword phdrpos;       // position at program header
    dword shdrpos;       // position at section header
    dword flag;          // 0
    word  hdrsize;       // ELF header size
    word  phdrent;       // program header entry size
    word  phdrcnt;       // program header entry count
    word  shdrent;       // section header entry size
    word  shdrcnt;       // section header entry count
    word  strsec;        // section number to string
} ELFHeader;

typedef struct {
    dword name;
    dword type;
    dword flags;
    dword address;
    dword offset;
    dword size;
    dword link;
    dword info;
    dword align;
    dword entsize;
} ELFSectionHeader;

typedef struct {
    dword type;
    dword offset;
    dword virtualaddr;
    dword physaddr;
    dword filesize;
    dword memorysize;
    dword flags;
    dword align;
} ELFProgramHeader;

#define ELF_ERROR_NOT_ELF           -1
#define ELF_ERORR_NOT_SUPPORTED_ELF -2
#define ELF_ERORR_NOT_EXECUTABLE    -3

#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
int loadProcess(byte* image, dword size, dword entrypoint, const char* path, const char* name, bool isUser, CommandOption* list);
int loadProcess(const char* path, const char* name, bool isUser, CommandOption* list);

class ELFLoader {

  public:
    ELFLoader();
    ~ELFLoader();

  public:
    int prepare(dword elf);
    int getErrorCode() const;
    dword load(byte* toAddress);

  private:
    bool isValidELF();

  private:
    int errorCode_;
    ELFHeader*        header_;
    ELFProgramHeader* pheader_;
    ELFSectionHeader* sheader_;

};

#endif
