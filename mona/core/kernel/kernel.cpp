/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2002-2010 Higepon
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

    Copyright (c) 2002-2007 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#define GLOBAL_VALUE_DEFINED

#include <sys/types.h>
#include "global.h"
#include "kernel.h"
#include "operator.h"
#include "checker.h"
#include "GraphicalConsole.h"
#include "ihandlers.h"
#include "pic.h"
#include "BitMap.h"
#include "string.h"
#include "syscalls.h"
#include "PageManager.h"
#include "MemoryManager.h"
#include "vbe.h"
#include "VesaConsole.h"
#include "LogConsole.h"
#include "Loader.h"
#include "Scheduler.h"
#include "monaboot.h"
#include "RTC.h"
#include "apm.h"
#include "addressmap.h"
#include "Uart.h"
#include "KObjectService.h"

#ifdef __GNUC__
#define CC_NAME "gcc-%d.%d.%d"
#define CC_VER  __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__
#endif

const char* version = "Mona version.0.3.0 $Date::                           $";
uint32_t version_number  = 0x00000300;
void  mainProcess();

static int fileptr = KERNEL_BASE_ADDR + REL_KERNEL_ADDR, sizeptr = 0x00001100;

/*!
    \brief  mona kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author Higepon
    \date   create:2002/07/21 update:$Date$
*/
void startKernel()
{
    /* kernel memory range */
    km.initialize(0x200000, 0xBfffff);

    /* APM */
    g_apmInfo = new APMInfo;
    memcpy(g_apmInfo, (APMInfo*)0x900, sizeof(APMInfo));

    /* set segment */
    GDTUtil::setup();

    /* VESA */
    g_vesaInfo = new VesaInfo;
    memcpy(g_vesaInfo, (VesaInfo*)0x800, sizeof(VesaInfo));

    /* console */
    if (g_vesaInfo->sign[0] == 'N')
    {
        g_console = new GraphicalConsole();
        g_console->printf("VESA not supported. sorry kernel stop.\n");
        for (;;);
    }
    else
    {
        g_vesaDetail = new VesaInfoDetail;
        memcpy(g_vesaDetail, (VesaInfoDetail*)0x830, sizeof(VesaInfoDetail));
        g_console = new VesaConsole(g_vesaDetail);
        g_console->setCHColor(GP_LIGHTGREEN);
        g_console->setBGColor(GP_WHITE);
        g_console->clearScreen();
    }

    g_console->printf("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\n");
    g_console->printf("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\n");
    g_console->printf("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff  %s\n", version);
    g_console->printf("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff  ["CC_NAME" @ %s]\n", CC_VER, OSTYPE);
    g_console->printf("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff  Copyright (c) 2002-2010 higepon\n");
    g_console->printf("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\n");
    g_console->printf("\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\n");

#if 1
    int w = g_vesaDetail->xResolution;
    int bpp = g_vesaDetail->bitsPerPixel / 8;
    uint8_t *vram = (uint8_t*)g_vesaDetail->physBasePtr;

    switch (bpp) {
        case 2:
            for (int y = 0; y < 105; y++) {
                for (int x = 0; x < 55; x++) {
                    uint8_t *pixel = NULL;
                    uint16_t rgb16  = 0;
                    // hi 4bit
                    pixel = &vram[(x * 2 + y * w) * bpp];
                    rgb16 = (uint16_t)(((palette16[(monaboot[y][x] >> 4) & 0xF] >> 8) & 0xF800) |
                        ((palette16[(monaboot[y][x] >> 4) & 0xF] >> 5) & 0x07E0) |
                        ((palette16[(monaboot[y][x] >> 4) & 0xF] >> 3) & 0x001F));
                    *((uint16_t*)pixel) = rgb16;
                    // low 4bit
                    pixel = &vram[(x * 2 + 1 + y * w) * bpp];
                    rgb16 = (uint16_t)(((palette16[monaboot[y][x] & 0xF] >> 8) & 0xF800) |
                        ((palette16[monaboot[y][x] & 0xF] >> 5) & 0x07E0) |
                        ((palette16[monaboot[y][x] & 0xF] >> 3) & 0x001F));
                    *((uint16_t*)pixel) = rgb16;
                }
           }
            break;
        default:
            for (int y = 0; y < 105; y++) {
                for (int x = 0; x < 55; x++) {
                    uint8_t *pixel   = NULL;
                    uint8_t *p_color = NULL;
                    // hi 4bit
                    pixel = &vram[(x * 2 + y * w) * bpp];
                    p_color = (uint8_t*)&palette16[(monaboot[y][x] >> 4) & 0xF];
                    pixel[0] = p_color[0];
                    pixel[1] = p_color[1];
                    pixel[2] = p_color[2];
                    // low 4bit
                    pixel = &vram[(x * 2 + 1 + y * w) * bpp];
                    p_color = (uint8_t*)&palette16[monaboot[y][x] & 0xF];
                    pixel[0] = p_color[0];
                    pixel[1] = p_color[1];
                    pixel[2] = p_color[2];
                }
            }
            break;
    }
#endif

    // use COM1 serial port
    g_log = new LogConsole();
    // use COM2 serial port
    g_com2 = new Uart(Uart::COM2);

//    g_console->printf("read from COM2<%c>:", g_com2->readChar());



    pic_init();
    RTC::init();
    printOK("Setting PIC        ");
    IDTUtil::setup();
    printOK("Setting IDT        ");
    printOK("Setting GDT        ");

    checkTypeSize();
    printOK("Checking type size ");

    /* get total system memory */
    g_total_system_memory = MemoryManager::getPhysicalMemorySize();
    g_console->printf("\nSystem Total Memory %d[MB]. VRAM=%x Paging on \n", g_total_system_memory / 1024 / 1024, g_vesaDetail->physBasePtr);
    g_console->printf("VESA: %dx%d %dbpp\n", g_vesaDetail->xResolution, g_vesaDetail->yResolution, g_vesaDetail->bitsPerPixel);

    if( !g_apmInfo->isSupported )
    {
        g_console->printf("Warning: APM is not supported.\n");
    }
    else
    {
        apm_init();
    }

    dumpAddressMap();

    /* shared memory object */
    SharedMemoryObject::setup();
    /* IDManager */
    g_id = new IDManager();
    /* This mutex has no owner, so will never deleted */
    g_mutexShared = KObjectService::createNullOwner<KMutex>();
    /* Paging start */
    PhysicalAddress vramAddress = g_vesaDetail->physBasePtr;
    int vramSizeBytes = (g_vesaDetail->xResolution * g_vesaDetail->yResolution * g_vesaDetail->bitsPerPixel / 8);
    g_page_manager = new PageManager(g_total_system_memory, vramAddress, vramSizeBytes);
    g_page_directory = g_page_manager->makeFirstPageDirectory();
    g_page_manager->startPaging((PhysicalAddress)g_page_directory);

    /* dummy thread struct */
    Thread* dummy1 = new Thread();
    Thread* dummy2 = new Thread();
    g_prevThread    = dummy1->tinfo;
    g_currentThread = dummy2->tinfo;
    /* this should be called, before timer enabled */
    ProcessOperation::initialize(g_page_manager);
    g_scheduler = new Scheduler();

    /* messenger */
    g_messenger = new Messenger(g_scheduler);
    /* at first create idle process */
    Process* idleProcess = ProcessOperation::create(ProcessOperation::KERNEL_PROCESS, "IDLE");
    g_idleThread = ThreadOperation::create(idleProcess, (uint32_t)monaIdle);
    g_scheduler->Join(g_idleThread, ThreadPriority::Min);
    /* start up Process */
    Process* initProcess = ProcessOperation::create(ProcessOperation::KERNEL_PROCESS, "INIT");
    Thread*  initThread  = ThreadOperation::create(initProcess, (uint32_t)mainProcess);
    g_scheduler->Join(initThread);

    disableTimer();
    disableKeyboard();
    enableInterrupt();


    /* dummy thread struct */
    g_prevThread    = dummy1->tinfo;
    g_currentThread = dummy2->tinfo;
    g_prevThread->archinfo->cr3    = 1;
    g_currentThread->archinfo->cr3 = 2;

    // Just before enable timer, sync epochNanosec.
    RTC::syncEpochNanosec();

  /*
    g_console->printf("inb8:%x\n", inp8(0x21));
    g_console->printf("%s:%d\n", __FILE__, __LINE__);
*/
    enableTimer();
#ifdef HIGE

#endif

    for (;;);
}

/*!
    \brief  mona kernel panic

    kernel panic

    \author Higepon
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
    \author Higepon
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

void loadServer(const char* server, const char* name)
{
    g_console->printf("loading %s....", server);
    if (strstr(server, ".BIN"))
    {
        uint8_t* image = (uint8_t*)fileptr;
        uint32_t size = (*(int*)sizeptr) * 512;
        sizeptr += 4;
        fileptr += size;

        if (image == NULL)
        {
            g_console->printf("server %s not found\n", name);
            return;
        }

        g_console->printf("%s\n", Loader::Load(image, size, Loader::ORG, name, true, NULL) ? "NG" : "OK");
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
    uint8_t* buf = (uint8_t*)MONA_CFG_ADDR;
    int fileSize = (*(int*)sizeptr) * 512;
    sizeptr += 4;
    fileptr += (*(int*)sizeptr) * 512;
    sizeptr += 4;

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

    return 0;
}

inline intptr_t syscall0(intptr_t syscall_number)
{
    intptr_t ret = 0;
    asm volatile("movl $%c1, %%ebx \n"
                 "int  $0x80       \n"
                 "movl %%eax, %0   \n"
                 :"=m"(ret)
                 :"g"(syscall_number)
                 :"ebx"
                 );
    return ret;
}

void mainProcess()
{
    if (execSysConf() != 0)
    {
        g_console->printf("/MONA.CFG does not exist\n");
        for (;;);
    }

//    enableKeyboard();

#ifdef HIGE

#endif

    /* end */
    syscall0(SYSTEM_CALL_KILL);

    for (;;);
}
