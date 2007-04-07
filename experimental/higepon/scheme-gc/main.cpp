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
    FILE* f = fopen("/MONA.CFG", "rb");
    if (NULL == f)
    {
        _printf("file open error\n");
        return -1;
    }
    char buf[32];
    uint32_t size = fread(buf, 1, 32, f);
    _printf("read size = %d: %s\n", size, buf);
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
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        scheme_exec_file(argv[1]);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//        return scheme_exec_file(argv[1]);
    }
#endif
}
