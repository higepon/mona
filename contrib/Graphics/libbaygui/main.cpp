/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "baygui.h"

extern "C" int dllmain(uint32_t reason);
extern "C" __attribute__((constructor)) void libgui_initialize();
extern "C" __attribute__((destructor)) void libgui_finalize();
extern "C" FuncVoid* __CTOR_LIST__[];
extern "C" FuncVoid* __DTOR_LIST__[];

int dllmain(uint32_t reason)
{
    switch (reason)
    {
    case 0: // DLL_PROCESS_ATTACH
        _logprintf("DLL_PROCESS_ATTACH\n");
        invokeFuncList(__CTOR_LIST__, __FILE__, __LINE__);
        break;
    case 1: // DLL_PROCESS_DETACH
        _logprintf("LIBC DLL_PROCESS_DETACH\n");
        invokeFuncList(__DTOR_LIST__, __FILE__, __LINE__);
        break;
    default:
        _printf("%s:%s:%d\n", __FILE__, __func__, __LINE__);
        break;
    }
    return 1;
}

bool libbaygui_initialized = false;

__attribute__((constructor)) void libbaygui_initialize()
{
    if (libbaygui_initialized) return;
    monapi_initialize();
    libbaygui_initialized = true;
}

__attribute__((destructor)) void libbaygui_finalize()
{
}

int main(int argc, char* argv[])
{
#ifdef SDL
    Frame* frame = new Frame();
    frame->setBounds(100,100,320,240);
    frame->run();
    delete(frame);
#endif
    return 0;
}

extern "C" void __main()
{
}
