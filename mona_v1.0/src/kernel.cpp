/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2002,2003,2004 Higepon
    Copyright (c) 2002,2003      Guripon
    Copyright (c) 2003           .mjt

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
    (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
    publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
    provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both
    the above copyright notice(s) and this permission notice appear in supporting documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS
    NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
    WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written authorization of the copyright holder.
*/

/*!
    \file   kernel.cpp
    \brief  mona kernel start at this point

    mona kernel start at this point.
    before startKernel, os entered protected mode.

    Copyright (c) 2002,2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#define GLOBAL_VALUE_DEFINED

#include <types.h>
#include <global.h>
#include <kernel.h>
#include <operator.h>
#include <tester.h>
#include <checker.h>
#include <KeyBoardManager.h>
#include <FDCDriver.h>
#include <GraphicalConsole.h>
#include <ihandlers.h>
#include <pic.h>
#include <BitMap.h>
#include <FAT12.h>
#include <string.h>
#include <syscalls.h>
#include <userlib.h>
#include <PageManager.h>
#include <elf.h>
#include <MemoryManager.h>
#include <vbe.h>

char* version = "Mona version.0.1.4 $Date$";

extern int pos_x;
extern int pos_y;
void printInfo() {

    g_console->printf("loadPloadProcess=%s\n", loadProcess(".", "USER.ELF", true) ? "NG" : "OK");
    g_console->printf("loadPloadProcess=%s\n", loadProcess(".", "TEST.ELF", true) ? "NG" : "OK");


    for (;;) {

        //        while (Semaphore::down(&g_semaphore_console));
        int tempx = pos_x;
        int tempy = pos_y;

        pos_x = 0;
        pos_y = 0;
        //g_processManager->printProcess();

        pos_x = tempx;
        pos_y = tempy;
        //        Semaphore::up(&g_semaphore_console);

    }
}

/*!
    \brief  mona kernel start at this point

    cstart call this function.
    actually, kernel starts at this point

    \author HigePon
    \date   create:2002/07/21 update:2003/06/08
*/
void startKernel(void) {

    /* kernel memory range */
    km.initialize(0x200000, 0x7fffff);

    /* set segment */
    GDTUtil::setup();

    /* initialze console */
    g_console = new GraphicalConsole();

    pic_init();
    printOK("Setting PIC        ");

    IDTUtil::setup();
    printOK("Setting IDT        ");
    printOK("Setting GDT        ");

    checkTypeSize();
    printOK("Checking type size ");

    /* get total system memory */
    g_total_system_memory = MemoryManager::getPhysicalMemorySize();
    g_console->printf("\nSystem TotalL Memory %d[MB]. Paging on\n", g_total_system_memory / 1024 / 1024); //

    /* shared memory object */
    SharedMemoryObject::setup();

    /* paging start */
    g_page_manager = new PageManager(g_total_system_memory);
    g_page_manager->setup();

    /* test for vesa */
//     VesaInfo* vesaInfo = new VesaInfo;
//     memcpy(vesaInfo, (VesaInfo*)0x800, sizeof(VesaInfo));
//     if (vesaInfo->sign[0] == 'N') {
//         g_console->printf("VESA not supported[%c]\n", vesaInfo->sign[1]);
//     } else {

//         VesaInfoDetail* vesaDetail = new VesaInfoDetail;
//         memcpy(vesaDetail, (VesaInfoDetail*)0x830, sizeof(VesaInfoDetail));
//         g_console->printf("VESA supported version=%d.%d [%x] \n", vesaInfo->versionH, vesaInfo->versionL, vesaInfo->oemStringPtr);
//         g_console->printf("(modeAttributes     =%x)", vesaDetail->modeAttributes     );
//         g_console->printf("(winAAttributes     =%x)", vesaDetail->winAAttributes     );
//         g_console->printf("(winBAttributes     =%x)", vesaDetail->winBAttributes     );
//         g_console->printf("(winGranularity     =%x)", vesaDetail->winGranularity     );
//         g_console->printf("(winSize            =%x)", vesaDetail->winSize            );
//         g_console->printf("(winASegment        =%x)", vesaDetail->winASegment        );
//         g_console->printf("(winBSegment        =%x)", vesaDetail->winBSegment        );
//         g_console->printf("(winFuncPtr         =%x)", vesaDetail->winFuncPtr         );
//         g_console->printf("(bytesPerScanLine   =%x)", vesaDetail->bytesPerScanLine   );
//         g_console->printf("(xResolution        =%x)", vesaDetail->xResolution        );
//         g_console->printf("(yResolution        =%x)", vesaDetail->yResolution        );
//         g_console->printf("(xCharSize          =%x)", vesaDetail->xCharSize          );
//         g_console->printf("(yCharSize          =%x)", vesaDetail->yCharSize          );
//         g_console->printf("(numberOfPlanes     =%x)", vesaDetail->numberOfPlanes     );
//         g_console->printf("(bitsPerPixel       =%x)", vesaDetail->bitsPerPixel       );
//         g_console->printf("(numberOfBanks      =%x)", vesaDetail->numberOfBanks      );
//         g_console->printf("(memoryModel        =%x)", vesaDetail->memoryModel        );
//         g_console->printf("(bankSize           =%x)", vesaDetail->bankSize           );
//         g_console->printf("(numberOfImagePages =%x)", vesaDetail->numberOfImagePages );
//         g_console->printf("(reserved           =%x)", vesaDetail->reserved           );
//         g_console->printf("(redMaskSize        =%x)", vesaDetail->redMaskSize        );
//         g_console->printf("(redFieldPosition   =%x)", vesaDetail->redFieldPosition   );
//         g_console->printf("(greenMaskSize      =%x)", vesaDetail->greenMaskSize      );
//         g_console->printf("(greenFieldPosition =%x)", vesaDetail->greenFieldPosition );
//         g_console->printf("(blueMaskSize       =%x)", vesaDetail->blueMaskSize       );
//         g_console->printf("(blueFieldPosition  =%x)", vesaDetail->blueFieldPosition  );
//         g_console->printf("(rsvdMaskSize       =%x)", vesaDetail->rsvdMaskSize       );
//         g_console->printf("(directColorModeInfo=%x)", vesaDetail->directColorModeInfo);
//     }

    /* FDC do not delete */
    g_fdcdriver = new FDCDriver();

    /* this should be called, before timer enabled */
    ThreadManager::setup();
    g_processManager = new ProcessManager(g_page_manager);

    /* add testProces1(testThread1) */
     Process* testProcess1 = g_processManager->create(ProcessManager::KERNEL_PROCESS, "TEST1");
     g_processManager->add(testProcess1);
     Thread*   testThread1  = g_processManager->createThread(testProcess1, (dword)printInfo);
     g_processManager->join(testProcess1, testThread1);

    /* initilize keyboard */
    KeyBoardManager& km = KeyBoardManager::instance();
    km.init();

#ifdef HIGE

    disableTimer();
    enableKeyboard();
    enableInterrupt();

    g_info_level = MSG;
    enableTimer();
#endif

    for (;;);
}

/*!
    \brief  mona kernel panic

    kernel panic

    \author HigePon
    \date   create:2002/12/02 update:2003/03/01
*/
void panic(const char* msg) {

    g_console->setCHColor(GP_RED);
    g_console->printf("kernel panic\nMessage:%s\n", msg);
    for (;;);
}

void checkMemoryAllocate(void* p, const char* msg) {

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
inline void printOK(const char* msg) {

    static int i = 0;

    if (i % 2) g_console->printf("   ");

    g_console->printf((char*)msg);
    g_console->printf("[");
    g_console->setCHColor(GP_LIGHTBLUE);
    g_console->printf("OK");
    g_console->setCHColor(GP_WHITE);
    g_console->printf("]");

    if (i % 2) g_console->printf("\n");
    i++;
}

/*!
    \brief print Banner

    print Banner

    \author HigePon
    \date   create:2003/01/26 update:2003/01/25
*/
inline void printBanner() {

    g_console->printf("------------------------------------------------------\n");
    g_console->printf("   Thanks for choosing MONA!                          \n");
    g_console->printf("            /\x18__/\x18                                    \n");
    g_console->printf("           ( ;'[]`)  < Ž³ŽÜŽÜŽÜŽ§ŽÝ!!                       \n");
    g_console->printf("           (      )                                   \n");
    g_console->printf("------------------------------------------------------\n");
    g_console->printf("%s\n\n", version);
    g_console->setCHColor(GP_WHITE);
}
