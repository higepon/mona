/*!
  \file  Screen
  \brief class Screen

  Copyright (c) 2006 Higepon
  All rights reserved.
  License=MIT/X License

  \author  Higepon
  \version $Revision$
  \date   create:2006/10/16 update:$Date$
*/

#include <monapi.h>
#include <monalibc.h>
#include <servers/screen.h>
#include <monapi/terminal/CommandParser.h>
#include <monapi/strstr_n.h>
#include "ScreenWriter.h"

using namespace MonAPI;
using namespace MonAPI::terminal;

static Stream in;
Screen screen;

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
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (length == 0) return;
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    parser->parse((uint8_t*)text, length);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
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

static void outLoop()
{
    const uint32_t BUFFER_SIZE = 1024;
    for (;;)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        char buffer[BUFFER_SIZE];
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        in.waitForRead();
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        uint32_t size = in.read((uint8_t*)buffer, BUFFER_SIZE);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        buffer[size == BUFFER_SIZE ? BUFFER_SIZE - 1 : size] = '\0';
//         for (int i = 0; i < size; i++) {
//             _logprintf("<%c>", buffer[i]);
//         }

        // don't display ^EOP
        char* found = strstr_n(buffer, "^EOP", size);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        if (NULL != found)
        {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            memset(found, '\0', 4);
        }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        output(buffer, size);
    }
}

int main(int argc, char* argv[])
{
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    parser = new CommandParser(new ScreenWriter());
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if (monapi_notify_server_start("MONITOR.BIN") != M_OK)
    {
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        exit(-1);
    }
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    syscall_mthread_create(outLoop);
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    for (MessageInfo msg;;)
    {
        if (Message::receive(&msg) != 0) continue;

        switch (msg.header)
        {
        case MSG_SCREEN_GET_STREAM_HANDLE:
    logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            Message::reply(&msg, in.handle());
            break;
        default:
            break;
        }
    }

    return 0;
}
