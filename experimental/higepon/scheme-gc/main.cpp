/*!
  \file   main.cpp
  \brief  scheme

  Copyright (c) 2006 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2006/06/16 update:$Date$
*/
#include "scheme.h"

using namespace util;
using namespace monash;

int main(int argc, char *argv[])
{
#if 0
    FILE* f = fopen("/SERVERS/TEST.SCM", "rb");
    if (NULL == f)
    {
        _printf("file open error\n");
        return -1;
    }
    char buf[32];
    uint32_t size = fread(buf, 1, 32, f);
    _printf("read size = %d: %s\n", size, buf);
    fclose(f);
#elif 0
    FILE* f = fopen("/SERVERS/TEST.SCM", "rb");
    if (NULL == f)
    {
        _printf("file open error\n");
        return -1;
    }
    if (-1 == fseek(f, 0, SEEK_END))
    {
        perror("fseek");
        return -1;
    }
    size_t size = ftell(f);
    _printf("file size = %d\n", size);
    fclose(f);

#elif 0
    FILE* f = fopen("/SERVERS/TEST.SCM", "rb");
    if (NULL == f)
    {
        _printf("file open error\n");
        return -1;
    }
    size_t size = monapi_file_get_file_size(f->_file); // 19616 byte
    _printf("file size = %d\n", size);
    char* buf = new char[size];
    int readSize = fread(buf, 1, size, f);
    _printf("readSize = %d pos=%d\n", ftell(f)); // => readSize = 19616 pos=1
    _printf("buf=%s\n", buf);  // 1024 byte しか読めてない
    delete[] buf;
    fclose(f);
#elif 0
    FILE* f = fopen("/SERVERS/TEST.SCM", "rb");
    if (NULL == f)
    {
        _printf("file open error\n");
        return -1;
    }
    _printf("%c\n", fgetc(f));
    _printf("%c\n", fgetc(f));
    _printf("%c\n", fgetc(f));
    _printf("%c\n", fgetc(f));
    _printf("%c\n", fgetc(f));
    _printf("%c\n", fgetc(f));

    fclose(f);

#else
    // if continuation failed, see cont_initialize function and fix cont_stack_bottom!
    scheme_init();
    if (argc == 1)
    {
        scheme_input_loop();
        return 0;
    }
    else
    {
        scheme_exec_file(argv[1]);
        return 0;
    }
#endif
}
