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
#include <gui/System/Array.h>

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

void usage(const char* name)
{
    fprintf(stderr, "usage: %s [-o IMAGE] ELF\n", name);
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }

    const char* target, * output = NULL;
    if (strcmp(argv[1], "-o") == 0)
    {
        if (argc < 4)
        {
            usage(argv[0]);
            return 1;
        }
        output = argv[2];
        target = argv[3];
    }
    else
    {
        target = argv[1];
    }

    _A<byte> elf = loadFromFile(target);
    if (elf == NULL) return 1;

    ELFParser parser;

    return 0;
}
