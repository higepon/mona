/*!
  \file  Screen
  \brief class Screen

  Copyright (c) 2006 Higepon
  All rights reserved.
  License=MIT/X License

  \author  HigePon
  \version $Revision$
  \date   create:2006/10/16 update:$Date$
*/

#include <monapi.h>
#include <monalibc.h>
#include <servers/screen.h>
#include <monapi/terminal/CommandParser.h>
#include "ScreenWriter.h"

using namespace MonAPI;
using namespace MonAPI::terminal;

static Stream in;
static Screen screen;

#define FONT_WIDTH   8
#define FONT_HEIGHT 16
#define FOREGROUND 0x000000
#define BACKGROUND 0xffffff

void cursor(bool erase)
{
    int x, y;
    syscall_get_cursor(&x, &y);
    screen.fillRect16(x * FONT_WIDTH, y * FONT_HEIGHT + FONT_HEIGHT - 2, 8, 2,
                      erase ? BACKGROUND : FOREGROUND);
}

void left(int n)
{
    int x, y;
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x - n, y);
}

void right(int n)
{
    int x, y;
    syscall_get_cursor(&x, &y);
    syscall_set_cursor(x + n, y);
}

void outCharacter(char c)
{
    char buf[2];
    buf[0] = c;
    buf[1] = '\0';
    syscall_print(buf);
}

#if 1
static CommandParser* parser;
void output(char* text, uint32_t length)
{
    if (length == 0) return;
    parser->parse((uint8_t*)text, length);

}
#else
void output(char* text, uint32_t length)
{
    CommandParser parser(NULL);
    const char DRAW_CURSOR_COMMAND[] = {0x1b, '[', '7', 'm', ' ', 0x1b, '[', '2', '7', 'm'};
    const char ERASE_CURSOR_COMMAND[]  = {0x1b, '[', '2', '7', 'm', ' ', 0x1b, '[', '7', 'm'};
    const uint32_t COMMAND_LENGTH = sizeof(ERASE_CURSOR_COMMAND);
    if (length == 0) return;
//    logprintf("output start\n");
//     for (uint32_t i = 0; i < length; i++)
//     {
//         logprintf("%c", text[i]);
//     }
//    logprintf("\noutput end\n");


    char c = text[0];
    if (c == 0x1b)
    {
        if (length >= 7 && text[1] == '[' && text[6] == 'H')
        {
            char tmp[2];
            tmp[0] = text[2];
            tmp[1] = '\0';
            int x = atoi(tmp);
            tmp[0] = text[4];
            int y = atoi(tmp);
            syscall_set_cursor(x, y);
            output(&text[7], length - 7);

        }
        else if (length >= 2 && text[1] == 'c')
        {
            syscall_clear_screen();
            output(&text[2], length - 2);
        }
        else if (length >= 4 && text[1] == '[' && text[3] == 'D')
        {
            // backspace hack!
            char tmp[2];
            tmp[0] = text[2];
            tmp[1] = '\0';
            int n = atoi(tmp);
            left(n);
            output(&text[4], length - 4);
        }
        else if (length >= 4 && text[1] == '[' && text[3] == 'C')
        {
            // backspace hack!
            char tmp[2];
            tmp[0] = text[2];
            tmp[1] = '\0';
            int n = atoi(tmp);
            right(n);
            output(&text[4], length - 4);
        }
        else if (length >= COMMAND_LENGTH && memcmp(DRAW_CURSOR_COMMAND, text, COMMAND_LENGTH) == 0)
        {
            cursor(false);
            output(&text[COMMAND_LENGTH], length - COMMAND_LENGTH);
        }
        else if (length >= COMMAND_LENGTH && memcmp(ERASE_CURSOR_COMMAND, text, COMMAND_LENGTH) == 0)
        {
            cursor(true);
            output(&text[COMMAND_LENGTH], length - COMMAND_LENGTH);
        }
        else
        {
            outCharacter(c);
            output(&text[1], length - 1);
        }
    }
    else
    {
        outCharacter(c);
        output(&text[1], length - 1);
    }
}
#endif
char* strstr_n(const char* str1, const char* str2, int size)
{
    if (str1 == NULL || str2 == NULL) return NULL;
    int i = 0;
    for (const char* p1 = str1; i < size; p1++, i++) {
        for (const char* p1a = p1, * p2 = str2;; p1a++, p2++, i++) {
            if (*p2 == '\0') return (char*)p1;
            if (*p2 != *p1a) break;
        }
    }
    return NULL;
}


static void outLoop()
{
    const uint32_t BUFFER_SIZE = 1024;
    for (;;)
    {
        char buffer[BUFFER_SIZE];
        in.waitForRead();
        uint32_t size = in.read((uint8_t*)buffer, BUFFER_SIZE);
        buffer[size == BUFFER_SIZE ? BUFFER_SIZE - 1 : size] = '\0';

        // don't display ^EOP
        char* found = strstr_n(buffer, "^EOP", size);
        if (NULL != found)
        {
            memset(found, '\0', 4);
        }
        output(buffer, size);
    }
}

int main(int argc, char* argv[])
{
    parser = new CommandParser(new ScreenWriter());
    if (MONAPI_FALSE == monapi_notify_server_start("MONITOR.BIN"))
    {
        exit(-1);
    }

    syscall_mthread_create((uint32_t)outLoop);

    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
        case MSG_SCREEN_GET_STREAM_HANDLE:
            Message::reply(&msg, in.handle());
            break;
        default:
            break;
        }
    }

    return 0;
}
