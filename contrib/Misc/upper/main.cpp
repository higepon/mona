#include <monapi.h>
#include <monalibc.h>
#include <string>

using namespace std;

void toUpper(char* text, int length);
string readLine();

#define USE_LIBC 0

int main(int argc, char* argv[])
{
// test for yume
#if 0
    const char* t = "Hello, World\n";
    fwrite(t, 1, strlen(t), stdout);
    return 0;

#endif
    _logprintf("upper start*******************************************************************\n");
    // upper argument text
    if (argc >= 2)
    {
        char* text = argv[1];
        toUpper(text, strlen(text));
#if USE_LIBC
        _printf("%s:%d\n", __FILE__, __LINE__);
        fwrite(text, 1, strlen(text) + 1, stdout);
#else
        monapi_stdout_write((uint8_t*)text, strlen(text) + 1);
#endif
        return 0;
    }
    // upper string which read from stdin
    monapi_stdin_lock_for_read();
    string result = readLine();
    monapi_stdin_unlock_for_read();
    char* text = new char[result.size()];
    strcpy(text, result.c_str());
    toUpper(text, result.size());
    monapi_stdout_write((uint8_t*)text, result.size());
    delete[] text;
    return 0;
}

#if USE_LIBC
string readLine()
{
    string result("");
    for (;;) {
        uint8_t buffer[256];
        uint32_t size = fread(buffer, 1, 256, stdin);
        for (uint32_t i = 0; i < size; i++)
        {
            result += buffer[i];
            if (buffer[i] == '\n' || buffer[i] == '\0') return result;
        }
    }
    return result;
}
#else
string readLine()
{
    string result("");
    for (;;) {
        uint8_t buffer[256];
        uint32_t size = monapi_stdin_read(buffer, 256);
        for (uint32_t i = 0; i < size; i++)
        {
            result += buffer[i];
            if (buffer[i] == '\n' || buffer[i] == '\0') return result;
        }
    }
    return result;
}
#endif

void toUpper(char* text, int length)
{
    for (int i = 0; i < length; i++)
    {
       text[i] = toupper(text[i]);
    }
}
