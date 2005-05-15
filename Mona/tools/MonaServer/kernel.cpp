/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2002-2004 Higepon
    Copyright (c) 2002-2003 Guripon
    Copyright (c) 2003      .mjt
    Copyright (c) 2004      Gaku

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to
    do so, provided that the above copyright notice(s) and this permission
    notice appear in all copies of the Software and that both the above
    copyright notice(s) and this permission notice appear in supporting
    documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
    LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
    ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not
    be used in advertising or otherwise to promote the sale, use or other
    dealings in this Software without prior written authorization of the
    copyright holder.
*/

/*!
    \file   kernel.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.

    Copyright (c) 2002,2003, 2004 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#define WIN32_LEAN_AND_MEAN		// Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル :
#include <windows.h>
#include <stdafx.h>

#undef _MONA_GLOBAL_
#define GLOBAL_VALUE_DEFINED
#include "global.h"

#ifdef __GNUC__
#define CC_NAME "gcc-%d.%d.%d"
#define CC_VER  __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__
#else
#define CC_NAME "cl-%d"
#define CC_VER _MSC_VER
#define OSTYPE "Microsoft Windows"
#endif

const char* version = "Mona version.0.2.0 $Date$";
dword version_number  = 0x00000200;
void  mainProcess();

/*!
    \brief  mona kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:2003/06/08
*/
void startKernel(void)
{
    /* console */
    g_console = new VesaConsole(&g_vesaDetail);
    g_console->setCHColor(GP_LIGHTGREEN);
    g_console->setBGColor(GP_WHITE);
    g_console->clearScreen();

    g_console->printf("%s ["CC_NAME" @ %s]\n", version, CC_VER, OSTYPE);
    g_console->printf("Copyright (c) 2002-2004 higepon\n\n");

    printOK("Setting PIC        ");
    printOK("Setting IDT        ");
    printOK("Setting GDT        ");
    printOK("Checking type size ");

    /* get total system memory */
	MEMORYSTATUS mst;
	GlobalMemoryStatus(&mst);
    g_total_system_memory = (dword)mst.dwTotalPhys;
    g_console->printf("\nSystem Total Memory %d[MB]. VRAM=%x Paging on \n", g_total_system_memory / 1024 / 1024, g_vesaDetail.physBasePtr);
    g_console->printf("VESA: %dx%d %dbpp\n", g_vesaDetail.xResolution, g_vesaDetail.yResolution, g_vesaDetail.bitsPerPixel);


#if 0 ///
	/* start up Process */
    Process* initProcess = ProcessOperation::create(ProcessOperation::KERNEL_PROCESS, "INIT");
    Thread*  initThread  = ThreadOperation::create(initProcess, (dword)mainProcess);
    g_scheduler->Join(initThread);
#endif

    g_info_level = INFO_MSG;
}

/*!
    \brief  mona kernel panic

    kernel panic

    \author HigePon
    \date   create:2002/12/02 update:2003/03/01
*/
void panic(const char* msg)
{
    g_console->setCHColor(GP_RED);
    g_console->printf("kernel panic\nMessage:%s\n", msg);
    for (;;);
}

void checkMemoryAllocate(void* p, const char* msg)
{
    if (p != NULL) return;
    panic(msg);
}

/*!
    \brief print OK

    print "msg             [OK]"

    \param msg message
    \author HigePon
    \date   create:2003/01/26 update:2003/01/25
*/
inline void printOK(const char* msg)
{
    static int i = 0;

    if (i % 2) g_console->printf("   ");

    g_console->printf((char*)msg);
    g_console->printf("[");
    g_console->setCHColor(GP_LIGHTBLUE);
    g_console->printf("OK");
    g_console->setCHColor(GP_LIGHTGREEN);
    g_console->printf("]");

    if (i % 2) g_console->printf("\n");
    i++;
}

#if 0 ///
void loadServer(const char* server, const char* name)
{
    g_console->printf("loading %s....", server);
    if (strstr(server, ".BIN"))
    {
        g_console->printf("%s\n", Loader::Load(server, name, true, NULL) ? "NG" : "OK");
    }
    else
    {
        g_console->printf("unknown server type!\n");
    }

    MessageInfo msg;
    for (;;)
    {
        if (g_messenger->receive(g_currentThread->thread, &msg)) continue;

        if (msg.header == MSG_SERVER_START_OK) break;
    }
    return;
}

int execSysConf()
{
    /* only one process can use fd */
    while (Semaphore::down(&g_semaphore_fd));

    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();
    g_fdcdriver->recalibrate();

    /* file open */
    if (!(g_fs->open("/MONA.CFG", 1)))
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return 1;
    }

    /* get file size and allocate buffer */
    int fileSize  = g_fs->size();

    int readTimes = (fileSize + 512 - 1) / 512;
    byte* buf     = (byte*)malloc(512 * readTimes);

    memset(buf, 0, 512 * readTimes);
    if (buf == NULL)
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return 2;
    }

    /* read */
    if (!g_fs->read(buf, fileSize))
    {
        free(buf);
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
        return g_fs->getErrorNo();
    }

    /* close */
    if (!g_fs->close())
    {
        g_fdcdriver->motorAutoOff();
        Semaphore::up(&g_semaphore_fd);
    }

    g_fdcdriver->motorAutoOff();
    Semaphore::up(&g_semaphore_fd);

    /* execute */
    char line[256];
    int linepos = 0;
    for (int pos = 0; pos <= fileSize; pos++) {
        char ch = pos < fileSize ? (char)buf[pos] : '\n';
        if (ch == '\r' || ch == '\n') {
            if (linepos > 0) {
                line[linepos] = '\0';
                if (strstr(line, "SERVER=") == line) {
                    const char* server = &line[7];
                    const char* name = &line[linepos - 1];
                    for (;; name--) {
                        if (*name == '/') {
                            name++;
                            break;
                        }
                        if (name == server) break;
                    }
                    enableTimer(); // qemu need this why?

                    loadServer(server, name);

                }
                linepos = 0;
            }
        } else if (linepos < 255) {
            line[linepos++] = ch;
        }
    }

    free(buf);

    return 0;
}

void mainProcess()
{
    /* FDC do not delete */
    enableFDC();
    g_fdcdriver = new FDCDriver();
    g_fdcdriver->motor(ON);
    g_fdcdriver->recalibrate();

    g_fdcdriver->recalibrate();

    g_fs = new FSOperation();

    if (g_fs == NULL || !(g_fs->initialize((IStorageDevice*)g_fdcdriver)))
    {
        g_console->printf("FSOperation::initialize error\n");
        for (;;);
    }

    g_fdcdriver->motorAutoOff();

    if (execSysConf() != 0)
    {
        g_console->printf("/MONA.CFG does not exist\n");
        for (;;);
    }

    enableKeyboard();

    /* end */
    int result;

    SYSCALL_0(SYSTEM_CALL_KILL, result);

    for (;;);
}
#endif
