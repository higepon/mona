/*!
    \file  ISO9660File.cpp
    \brief ISO9660File

    Copyright (c) 2005 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2005/01/26 update:$Date$
*/

#include "ISO9660File.h"

#ifdef MONA
#include <monapi.h>
#include <monapi/string.h>
#else
#include <string.h>
#include <stdio.h>
#endif

using namespace MonAPI;

/*----------------------------------------------------------------------
    ISO9660File
----------------------------------------------------------------------*/
ISO9660File::ISO9660File(ISO9660FileSystem* fs)
{
    this->fs        = fs;
    this->hasDetail = false;
    this->pointer   = 0;
}

ISO9660File::~ISO9660File()
{
}

dword ISO9660File::Read(void* buffer, dword size)
{
    dword rest = this->attribute.size - this->pointer;

    if (rest < size)
    {
        size = rest;
    }

    int lba = this->attribute.extent + (this->pointer + ISO9660FileSystem::SECTOR_SIZE - 1) / ISO9660FileSystem::SECTOR_SIZE;
    int sectorCount = (this->pointer + size + ISO9660FileSystem::SECTOR_SIZE - 1) / ISO9660FileSystem::SECTOR_SIZE - this->pointer / ISO9660FileSystem::SECTOR_SIZE;
    dword readSize = sectorCount * ISO9660FileSystem::SECTOR_SIZE;

    byte* temp = new byte[readSize];
    if (temp == NULL) return 0;

    bool readResult = fs->Read(lba, temp, readSize);
    if (!readResult)
    {
        delete temp;
        return 0;
    }

    memcpy(buffer, temp + (this->pointer - ((int)this->pointer / ISO9660FileSystem::SECTOR_SIZE)) * ISO9660FileSystem::SECTOR_SIZE, size);

    delete[] temp;
    return size;
}

dword ISO9660File::Write(void* buffer, dword size)
{
    return 0;
}

bool ISO9660File::Seek(dword position, dword flag)
{
    if (SEEK_SET == flag)
    {
        if (position < 0)
        {
            this->pointer = 0;
        }
        else
        {
            this->pointer = position;
        }
    }
    else if (SEEK_CUR == flag)
    {
        this->pointer += pointer;
        if (0 > position)
        {
            if (this->pointer < (dword)-position)
            {
                this->pointer = 0;
            }
        }
    }
    else if (SEEK_END == flag)
    {
        this->pointer = this->attribute.size + pointer;
        if (0 > pointer)
        {
            if (this->attribute.size < (dword)-position)
            {
                this->pointer = 0;
            }
        }
    }

    if (this->pointer > this->attribute.size) this->pointer = this->attribute.size;

    return false;
}
