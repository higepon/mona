/*!
    \file   yaml.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/07/14 update:$Date$
*/
#include "yaml.h"

using namespace util;

uint8_t* loadFile(const String& path, uint32_t* size)
{
    uint8_t* buf;
    FILE* fp;

    fp = fopen(path.data(), "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "%s: file not found\n", __func__);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *size = ftell(fp);

    if (size == 0)
    {
        fclose(fp);
        return NULL;
    }

    fseek(fp, 0, SEEK_SET);

    buf = new uint8_t [*size];
    if (buf == NULL)
    {
        fprintf(stderr, "%s: memory allocate error\n", __func__);
        fclose(fp);
        return NULL;
    }
    fread(buf, 1, *size, fp);
    fclose(fp);
    return buf;
}

void loadYAML(const String& path, YAML& yaml)
{
    uint32_t size;
    char* content = (char*)loadFile(path, &size);
    if (NULL == content)
    {
        fprintf(stderr, "%s failed\n", __func__);
        return;
    }

    String line;
    int index = -1;
    int start = 0;
    for (uint32_t i = 0; i < size; i++)
    {
        if (content[i] == '\n')
        {
            content[i] = '\0';
            char* p = &content[start];
            line = p;
            start = i + 1;
            if (line.size() == 0 || line[0] == '#')
            {
                line = "";
                continue;
            }
            else if (line[0] == '-')
            {
                Strings* ss = new Strings;
                index++;
                yaml.add(ss);
            }
            else if (line[1] == '-')
            {
                Strings* ss = yaml.get(index);
                if (NULL == ss)
                {
                    fprintf(stderr, "unknown yaml type %s\n", path.data());
                    exit(-1);
                }
                char b[1024];
                char* p = (char*)line.data();
                strcpy(b, &p[3]);
                ss->add(new String(b));
            }
        }
    }
}

