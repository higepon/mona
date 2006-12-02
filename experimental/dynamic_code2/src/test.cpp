// g++ -masm=intel -S test.cpp
void call_func(unsigned int func)
{
    ((void(*)(void))func)();
}
