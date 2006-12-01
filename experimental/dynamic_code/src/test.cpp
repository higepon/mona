// gcc -S -O0 test.cpp
// callを逆アセンブルしてみてみるため
void call_functions(unsigned int func)
{
    ((void (*)(void))func)();
}
