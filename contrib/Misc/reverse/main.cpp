#include <monapi.h>
#include <monalibc.h>
#include <string>

using namespace std;

void reverse(char* text, int length);
string readLine();

int main(int argc, char* argv[])
{
    // reverse argument text
    if (argc > 1)
    {
        char* text = argv[1];
        reverse(text, strlen(text) + 1);
        monapi_stdout_write((uint8_t*)text, strlen(text) + 1);
        return 0;
    }

    // upper string which read from stdin
    monapi_stdin_lock_for_read();
    string result = readLine();
    monapi_stdin_unlock_for_read();
    char* text = new char[result.size() + 1];
    strcpy(text, result.c_str());
    reverse(text, result.size());
    monapi_stdout_write((uint8_t*)text, result.size());
    delete[] text;
    return 0;
}

string readLine()
{
    string result("");
    for (;;) {
        uint8_t buffer[256];
        uint32_t size = monapi_stdin_read(buffer, 256);
        for (uint32_t i = 0; i < size; i++)
        {
            result += buffer[i];
            if (buffer[i] == '\n')
            {
                result += '\0';
            }
            else if (buffer[i] == '\0')
            {
                return result;
            }
        }
    }
    return result;
}

void reverse(char* text, int length)
{
    char* p = new char[length];
    memcpy(p, text, length);
    for (int i = 0; i < length - 1; i++)
    {
        text[i] = p[length - 2 - i];
    }
}
