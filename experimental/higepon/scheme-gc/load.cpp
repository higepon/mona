#include "scheme.h"

using namespace util;

String load(const String& file)
{
    String ret;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    FILE* fp = fopen(file.data(), "rb");
    if (NULL == fp)
    {
        fprintf(stderr, "can not open %s\n", file.data());
        return ret;
    }
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (-1 == fseek(fp, 0, SEEK_END))
    {
        perror("fseek");
        return ret;
    }
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    size_t size = ftell(fp);

#ifdef USE_BOEHM_GC
    char* buffer = new(GC) char[size + 1];SCM_ASSERT(buffer);
#else
    char* buffer = new char[size + 1];SCM_ASSERT(buffer);
#endif

    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
    if (NULL == buffer)
    {
        fprintf(stderr, "memory error \n");
        return ret;
    }
    fseek(fp, 0, SEEK_SET);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debugmq
    int readSizeTotal = 0;
    for (;;)
    {
        int readSize = fread(&buffer[readSizeTotal], 1, size - readSizeTotal, fp);
        readSizeTotal += readSize;
        buffer[readSizeTotal] = '\0';
        if (readSize == 0 || readSizeTotal >= size)
        {
            break;
        }
    }
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debugmq
    fclose(fp);
    buffer[size] = '\0';
    _logprintf("buffer = %x size=%d readSize=%d %s %s:%d\n", buffer, size, readSizeTotal, __func__, __FILE__, __LINE__);fflush(stdout);// debugmq
    for (int i = 0; i < size; i++)
    {
        _logprintf("%c", buffer[i]);
    }
    ret = String(buffer);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debugmq
#ifndef MONASH_DONT_FREE_MEMORY
    delete[] buffer;
#endif
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debugmq
    return ret;
}
