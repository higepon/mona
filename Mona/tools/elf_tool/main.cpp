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
    bool ok = parser.set(elf.get(), elf.get_Length());
    if (!ok)
    {
        fprintf(stderr, "ERROR: file type is not ELF!\n");
        return 1;
    }

    int type = parser.getType();
    printf("file type = %d\n", type);
    if (type != ELFParser::TYPE_RELOCATABLE && type != ELFParser::TYPE_EXECUTABLE)
    {
        fprintf(stderr, "ERROR: file type is not supported!\n");
        return 1;
    }

    int result = parser.parse();
    printf("parse result = %d\n", result);
    if (result != 0)
    {
        fprintf(stderr, "ERROR: can not parse!\n");
        return 1;
    }

    dword imageSize = parser.getImageSize();
    printf("start address = %08x\n", parser.getStartAddr());
    printf("end address = %08x\n", parser.getEndAddr());
    printf("image size = %d\n", imageSize);

    if (output != NULL)
    {
        _A<byte> image(imageSize);
        if (!parser.load(image.get()))
        {
            fprintf(stderr, "ERROR: load failed!\n");
            return 1;
        }

        FILE* f = fopen(output, "wb");
        fwrite(image.get(), sizeof(byte), imageSize, f);
        fclose(f);
        printf("entry point = %08x", parser.getEntryPoint());
    }

    return 0;
}
