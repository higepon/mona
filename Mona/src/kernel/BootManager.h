/*!
    \file  BootManager.h
    \brief Boot parameter management

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/12/13 update:$Date$
*/
#ifndef _MONA_BOOT_MANAGER_
#define _MONA_BOOT_MANAGER_

#include "kernel.h"
#include "sys/HList.h"

class BootManager
{
public:
    BootManager(dword kernelAddress, dword monaConfigAddress, dword monaConfigSize);
    virtual ~BootManager();

public:
    byte* getFile(const char* name, dword* size);
    byte* getMonaConfig(dword* size);

private:
    typedef struct
    {
        dword size;
        char name[13];
        byte* buffer;
    } RamFileInfo;

    typedef struct
    {
        dword size;
        char name[12];
    } FileHeader;

private:
    void setupRamFiles(dword kernelAddress);
    void setupMonaConfig(dword address, dword size);

private:
    HList<RamFileInfo*> files;
    byte* monaConfig;
    dword monaConfigSize;
};

#endif
