/*!
    \file  Loader.h
    \brief Loader

    Copyright (c) 200 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/06/16 update:$Date$
*/
#ifndef _MONA_LOADER_
#define _MONA_LOADER_

#include "global.h"
#include "string.h"
#include "global.h"
#include "io.h"
#include "syscalls.h"

/*----------------------------------------------------------------------
    Loader
----------------------------------------------------------------------*/
class Loader
{
public:
    static int Load(uint8_t* image, uint32_t size, uint32_t entrypoint, const char* name, bool isUser, CommandOption* list);

public:
    enum
    {
        ORG            = 0xA0000000,
        //       MAX_IMAGE_SIZE = (4096 * 150)
       MAX_IMAGE_SIZE = (4 * 1024 * 1024)
    };

};

#endif
