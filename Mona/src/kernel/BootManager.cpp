/*!
  \file  BootManager.cpp
  \brief class Boot parameters management

  Copyright (c) 2002-2004 Higepon and the individuals listed on the ChangeLog entries.
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2004/12/13 update:$Date$
*/

#include "global.h"
#include "BootManager.h"

/*----------------------------------------------------------------------
    BootManager
----------------------------------------------------------------------*/
BootManager::BootManager(dword kernelAddress, dword monaConfigAddress, dword monaConfigSize)
{
    setupRamFiles(kernelAddress);
    setupMonaConfig(monaConfigAddress, monaConfigSize);
}

BootManager::~BootManager()
{
    delete[] monaConfig;

    for (int i = 0; i < files.size(); i++)
    {
        RamFileInfo* info = files.get(i);
        delete[] info->buffer;
        delete info;
    }
}

byte* BootManager::getFile(const char* name, dword* size)
{
    for (int i = 0; i < files.size(); i++)
    {
        RamFileInfo* info = files.get(i);

        if (memcmp(name, info->name, strlen(name) ) == 0)
        {
            *size = info->size;
            return info->buffer;
        }
    }

    return NULL;
}

void BootManager::setupRamFiles(dword kernelAddress)
{
    FileHeader* header = (FileHeader*)(kernelAddress + sizeof(FileHeader) + ((FileHeader*)kernelAddress)->size);

    for (; header->size != 0xFFFFFFFF; header = (FileHeader*)((dword)header + sizeof(FileHeader) + header->size))
    {
        RamFileInfo* info = new RamFileInfo;
        info->buffer      = new byte[header->size];

        info->size = header->size;
        strncpy(info->name, header->name, 12);
        info->name[12] = '\0';
        memcpy(info->buffer, (byte*)((dword)header + sizeof(FileHeader)), header->size);

        this->files.add(info);
    }
}

void BootManager::setupMonaConfig(dword address, dword size)
{
    this->monaConfig     = new byte[size];
    this->monaConfigSize = size;
    memcpy(this->monaConfig, (byte*)address, size);
}

byte* BootManager::getMonaConfig(dword* size)
{
    *size = this->monaConfigSize;
    return monaConfig;
}
