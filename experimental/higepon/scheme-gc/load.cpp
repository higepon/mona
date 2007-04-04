#include "scheme.h"

using namespace util;

String load(const char* file)
{
    String ret;
    FILE* fp = fopen(file, "rb");
    if (NULL == fp)
    {
        fprintf(stderr, "can not open %s\n", file);
        return ret;
    }

    if (-1 == fseek(fp, 0, SEEK_END))
    {
        perror("fseek");
        return ret;
    }

    size_t size = ftell(fp);
#ifdef USE_BOEHM_GC
    char* buffer = new(GC) char[size + 1];SCM_ASSERT(buffer);
#else
    char* buffer = new char[size + 1];SCM_ASSERT(buffer);
#endif
    if (NULL == buffer)
    {
        fprintf(stderr, "memory error \n");
        return ret;
    }

    fseek(fp, 0, SEEK_SET);

    fread(buffer, 1, size, fp);
    fclose(fp);
    buffer[size] = '\0';
    ret = String(buffer);
#ifndef MONASH_DONT_FREE_MEMORY
    delete[] buffer;
#endif
    return ret;
}
