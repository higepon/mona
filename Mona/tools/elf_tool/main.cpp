/*!
  \file   main.cpp
  \brief  test tool for Mona OS. ELF relocation

  Copyright (c) 2002- 2004 HigePon
  WITHOUT ANY WARRANTY

  \author  HigePon
  \version $Revision$
  \date   create:2004/05/02 update:$Date$
*/

#include "elfparser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <monapi/Array.h>

_A<byte> loadFromFile(const char* path)
{
    _A<byte> result;

    FILE* stream = fopen(path, "rb");
    if (stream == NULL)
    {
        fprintf(stderr, "ERROR: can not open file: %s\n", path);
        return result;
    }

    fseek(stream, 0, SEEK_END);
    int size = ftell(stream);

    if (size == 0)
    {
        fclose(stream);
        return result;
    }

    fseek(stream, 0, SEEK_SET);

    result.Alloc(size);
    if (result == NULL)
    {
        result.Unset();
        fprintf(stderr, "ERROR: can not allocate buffer!\n");
    }
    else
    {
        fread(result.get(), 1, size, stream);
    }
    fclose(stream);

    return result;
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("usage: %s dll\n", argv[0]);
        return 1;
    }

    _A<byte> elf = loadFromFile(argv[1]);
    if (elf.get() == NULL) return 1;

    /*
    ||
    || ELF Operation start
    ||
    */
    ELFParser parser;

    /*
    || Set ELF
    */
    parser.Set(elf.get(), elf.get_Length());
    printf("elf type=%d\n", parser.GetType());

    /*
    || Parse
    */
    switch(parser.GetType())
    {
    case ELFParser::ET_REL:

        parser.Parse(0xA0000);
        break;
    case ELFParser::ET_EXEC:

        parser.Parse();
        break;
    default:
        printf("unknown type\n");
        return -1;

        /* not reached */
        break;
    }

    /*
    || Get Image size
    */
    dword imageSize = parser.GetImageSize();
    printf("image size = %d\n", imageSize);

    /*
    || Allocate buffer
    */
    byte* imageBuf = new byte[imageSize];
    if (imageBuf == NULL)
    {
        printf("buffer allocate error\n");
        return -1;
    }

    /*
    || create Image
    */
    bool result = parser.CreateImage(imageBuf);
    printf("Create Image result = %s\n", result ? "OK" : "NG");

    /* call */
    return 0;
}
