/*!
  \file   main.cpp
  \brief  fileHeader for Mona

  Copyright (c) 2002- 2004 HigePon
  WITHOUT ANY WARRANTY

  \author  HigePon
  \version $Revision$
  \date   create:2004/12/12 update:$Date$
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned int dword;

bool getSize(const char* file, dword* size)
{
    FILE* stream = fopen(file, "rb");

    if (stream == NULL) return false;

    fseek(stream, 0, SEEK_SET);
    fseek(stream, 0, SEEK_END);
    *size = ftell(stream);

    fclose(stream);
    return true;
}

bool writeHeader(const char* file , const char* name, dword size)
{
    FILE* stream = fopen(file, "wb");

    if (stream == NULL) return false;

    fseek(stream, 0, SEEK_SET);

    fwrite(&size, sizeof(dword), 1, stream);
    fprintf(stream, "%-12s", name);

    fclose(stream);

    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("usage:%s path fileName(8.3) outputFile\n", argv[0]);
        return 1;
    }

    dword size;
    if (!getSize(argv[1], &size))
    {
        printf("getSize Error\n");
        return 1;
    }

    if (!writeHeader(argv[3], argv[2], size))
    {
        printf("write Header Error\n");
        return 1;
    }

    return 0;
}
