#ifndef _MONA_GLOBAL_
#define _MONA_GLOBAL_

#ifdef GLOBAL_VALUE_DEFINED
#define GLOBAL /* */
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
#define GLOBAL_VAL(v) /* */
#endif

#include <sys/types.h>
#include "info.h"
#include "kernel.h"
#include "VirtualConsole.h"
#include "FDCDriver.h"
#include "Semaphore.h"
#include "GDTUtil.h"
#include "IDTUtil.h"
#include "Process.h"
#include "string.h"
#include "PageManager.h"
#include "MemoryManager.h"
#include "Segments.h"
#include "sysresource.h"
#include "tester.h"
#include "vbe.h"
#include "FSOperation.h"
#include "IDManager.h"

GLOBAL VirtualConsole* g_console GLOBAL_VAL(0);      /*< pointer to console    */
GLOBAL VirtualConsole* g_log     GLOBAL_VAL(0);
GLOBAL StackView g_stack_view;                       /*< struct for stack view */
GLOBAL DokodemoView g_dokodemo_view;

GLOBAL FDCDriver* g_fdcdriver;

/* semaphore */
GLOBAL semaphore g_semaphore_console  GLOBAL_VAL(1);
GLOBAL semaphore g_semaphore_shared   GLOBAL_VAL(1);
GLOBAL semaphore g_semaphore_fd       GLOBAL_VAL(1);

GLOBAL int  g_info_level GLOBAL_VAL(DEV_ERROR);

/* expr:sysresource */
GLOBAL BitMap *g_irqMap;
GLOBAL IRQHandler g_irqHandlers[16];

/* GLOBAL DESCRIPTOR TABLE */
GLOBAL SegDesc* g_gdt;

/* INTERRUPT DESCRIPTOR TABLE */
GLOBAL GateDesc* g_idt;

GLOBAL dword g_total_system_memory;

/* Process */
GLOBAL TSS* g_tss;

GLOBAL PageEntry* g_page_directory;
GLOBAL PageManager* g_page_manager;

GLOBAL MemoryManager km; /* Kernel Memory Management */

GLOBAL List<SharedMemoryObject*>* g_sharedMemoryObjectList;

GLOBAL ThreadInfo* g_currentThread;
GLOBAL ThreadInfo* g_prevThread;
GLOBAL Scheduler* g_scheduler;
GLOBAL VesaInfo* g_vesaInfo;
GLOBAL VesaInfoDetail* g_vesaDetail;
GLOBAL Messenger* g_messenger;

GLOBAL FSOperation* g_fs;
GLOBAL IDManager*   g_id;

GLOBAL dword gt[128];

GLOBAL PsInfo g_ps;

GLOBAL dword g_irqWaitThread[16];

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define logprintf g_log->printf
#else
#define logprintf //
#endif

#endif
