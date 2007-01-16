#include <monapi.h>
#include <monalibc.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        return -1;
    }
    char* text = argv[1];
    monapi_stdout_write((uint8_t*)text, strlen(text) + 1);
    return 0;
}

