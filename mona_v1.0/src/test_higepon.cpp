#include<tester.h>
#include<FAT12.h>
#include<FDCDriver.h>
#include<global.h>
#include<io.h>
#include<GraphicalConsole.h>
#include<operator.h>
#include<z.h>
#include<MemoryManager.h>
#include<KeyBoardManager.h>
#include<elf.h>
#include<userlib.h>
#include<Process.h>

/*!

\brief  test code for higepon

Copyright (c) 2002,2003 Higepon
All rights reserved.
License=MIT/X Licnese

\author  HigePon
\version $Revision$
\date   create:2003/05/18 update:$Date$
*/


extern "C" void put_pixel(int x, int y, char color);

typedef struct {
    dword a;
    dword b;
    dword width;
    dword height;
} ARGBHeader;

static bool drawARGB(byte* rgba, int x, int y, int size);
static int getColorNumber(byte* rgba);

struct read_info {
    FAT12 *fat;
    int sz;
} read_info;

#define SHARED_MM_ERROR -1
#define FAT_INIT_ERROR  -2
#define FAT_OPEN_ERROR  -3

bool writeFileAppend(IOStream* io) {

    IOStream src;
    src.dir  = io->dir;
    src.file = io->file;

    if (!readFile(&src)) {
        io->error = src.error;
        return false;
    }




}

bool readFile(IOStream* io) {

    /* prepare */
    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    /* Change Directory */
    if (!g_fat12->changeDirectoryRelative(io->dir)) {
        io->error = IO_ERROR_CD;
        return false;
    }

    /* file open */
    if (!g_fat12->open(io->dir, io->file, FAT12::READ_MODE)) {
        io->error = IO_ERROR_OPEN;
        return false;
    }

    /* get file size and allocate memory */
    io->size = g_fat12->getFileSize();
    int   readTimes = io->size / 512 + (io->size % 512 ? 1 : 0);
    io->buffer      = new byte[512 * readTimes];
    if (io->buffer == NULL) {
        io->error = IO_ERROR_ALLOC;
        return false;
    }
    memset(io->buffer, 0, 512 * readTimes);

    /* read */
    for (int i = 0; i < readTimes; i++) {
        if (!g_fat12->read(io->buffer + 512 * i)) {
            io->error = IO_ERROR_READ;
            return false;
        }
    }

    /* close */
    if (!g_fat12->close()) {
        io->error = IO_ERROR_CLOSE;
        return false;
    }
    g_fdcdriver->motor(OFF);
    return true;
}

bool writeFile(IOStream* io) {

    char buf[32];
    buf[0] = '\0';

    /* prepare */
    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    /* Change Directory */
    if (!g_fat12->changeDirectoryRelative(io->dir)) {
        io->error = IO_ERROR_CD;
        return false;
    }

    /* try to create file */
    strcpy(buf, io->file);
    char* file = strtok(buf ,     ".");
    char* ext  = strtok(NULL,     ".");
    g_fat12->createFlie(file, ext);

    /* file open */
    if (!g_fat12->open(io->dir, io->file, FAT12::WRITE_MODE)) {
        io->error = IO_ERROR_OPEN;
        return false;
    }

    /* get file size and allocate memory */
    int   writeTimes = io->size / 512 + (io->size % 512 ? 1 : 0);

    /* write */
    for (int i = 0; i < writeTimes; i++) {
        if (!g_fat12->write(io->buffer + 512 * i)) {
            io->error = IO_ERROR_WRITE;
            return false;
        }
    }

    /* close */
    if (!g_fat12->close()) {
        io->error = IO_ERROR_CLOSE;
        return false;
    }
    g_fdcdriver->motor(OFF);
    return true;
}


void testFDWrite() {

    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    if (!g_fat12->changeDirectoryRelative(".")) {
        g_console->printf("change dir error %d\n", g_fat12->getErrorNo());
        return;
    }

    if (!g_fat12->createFlie("MONA", "LOG") && g_fat12->getErrorNo() != 6) {
        g_console->printf("can not create file  %d\n", g_fat12->getErrorNo());
        return;
    }

    if (!g_fat12->open(".", "MONA.LOG", FAT12::WRITE_MODE)) {
        g_console->printf("can not open\n");
        return;
    }

    byte text[512];
    memset(text, 'M', 512);
    if (!g_fat12->write(text)) {
        g_console->printf("write error\n");
        return;
    }

    if (!g_fat12->close()) {
        g_console->printf("close errror\n");
        return;
    }
    g_fdcdriver->motor(false);
    return;
}

int send(const char* name, Message* message) {

    Process* process;
    Message* kmessage;

    if (message == (Message*)NULL) {
        return -1;
    }

    if ((process = g_processManager->find(name)) == (Process*)NULL) {
        return -1;
    }

    if ((kmessage = new Message) == NULL) {
        return -1;
    }

    memcpy(kmessage, message, sizeof(Message));
    process->getMessageList()->add(kmessage);
    return 0;
}

int receive(Process* process, Message* message) {

    Message* from = process->getMessageList()->get(0);
    if (from == (Message*)NULL) {
        return -1;
    }

    memcpy(message, from, sizeof(Message));
    free(from);
    process->getMessageList()->removeAt(0);
    return 0;
}

int read(input_stream *p, int sz)
{
    if (0 >= read_info.sz)
        return 0;

    if (!read_info.fat->read(p->bf)) {
        g_console->printf("error read\n");
        return 0;
    }

    if (512 > read_info.sz)
        return read_info.sz;

    read_info.sz -= 512;

    return 512;
}

int write(output_stream *p, int sz)
{
    *((int*)(p)->data) = sz;

    return sz;
}


bool drawARGB(byte* rgba, int x, int y, int size) {

    int startx = x;
    int starty = y;
    ARGBHeader* header = (ARGBHeader*)rgba;

    /* check identifier */
    if (header->a != 0x41524742) return false;

    /* draw loop */
    for (int i = sizeof(ARGBHeader) / 4; i < size / 4; i++) {

        if (x >= startx + (int)(header->width)) {

            y++;
            x = startx;
        }

        if (y >= starty + (int)(header->height)) break;

        put_pixel(x, y, getColorNumber(&(rgba[i * 4])));
        x++;
    }

    return true;
}

int getColorNumber(byte* rgba) {

    byte r = rgba[2];
    byte g = rgba[1];
    byte b = rgba[0];
    int result;

    if (r == 0x00 && g == 0x00 && b == 0x00) result = 0;
    else if (r == 0x00 && g == 0x00 && b == 0x80) result = 1;
    else if (r == 0x00 && g == 0x80 && b == 0x00) result = 2;
    else if (r == 0x00 && g == 0x80 && b == 0x80) result = 3;
    else if (r == 0x80 && g == 0x00 && b == 0x00) result = 4;
    else if (r == 0x80 && g == 0x00 && b == 0x80) result = 5;
    else if (r == 0x80 && g == 0x80 && b == 0x00) result = 6;
    else if (r == 0xc0 && g == 0xc0 && b == 0xc0) result = 7;
    else if (r == 0x80 && g == 0x80 && b == 0x80) result = 8;
    else if (r == 0x00 && g == 0x00 && b == 0xff) result = 9;
    else if (r == 0x00 && g == 0xff && b == 0x00) result = 10;
    else if (r == 0x00 && g == 0xff && b == 0xff) result = 11;
    else if (r == 0x00 && g == 0xff && b == 0xff) result = 12;
    else if (r == 0xff && g == 0x00 && b == 0x00) result = 13;
    else if (r == 0xff && g == 0x00 && b == 0xff) result = 14;
    else result = 15;

    return result;
}
