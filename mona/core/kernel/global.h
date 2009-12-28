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
#include "kernel.h"
#include "VirtualConsole.h"
#include "Semaphore.h"
#include "GDTUtil.h"
#include "IDTUtil.h"
#include "Process.h"
#include "string.h"
#include "PageManager.h"
#include "MemoryManager.h"
#include "Segments.h"
#include "vbe.h"
#include "IDManager.h"
#include "Scheduler.h"
#include "Messenger.h"
#include "apm.h"
#include "NicFactory.h"

class Uart;

GLOBAL VirtualConsole* g_console GLOBAL_VAL(0);      /*< pointer to console    */
GLOBAL VirtualConsole* g_log     GLOBAL_VAL(0);
GLOBAL StackView g_stack_view;                       /*< struct for stack view */
GLOBAL DokodemoView g_dokodemo_view;

/* semaphore */
GLOBAL semaphore g_semaphore_console  GLOBAL_VAL(1);
GLOBAL semaphore g_semaphore_shared   GLOBAL_VAL(1);

/* GLOBAL DESCRIPTOR TABLE */
GLOBAL SegDesc* g_gdt;

/* INTERRUPT DESCRIPTOR TABLE */
GLOBAL GateDesc* g_idt;

GLOBAL uint32_t g_total_system_memory;

/* Process */
GLOBAL volatile TSS* g_tss;

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

GLOBAL IDManager*   g_id;

GLOBAL uint32_t gt[128];

GLOBAL PsInfo g_ps;

GLOBAL IRQInfo g_irqInfo[16];

GLOBAL Thread* g_idleThread;

GLOBAL uint32_t g_mutexShared;

GLOBAL SharedMemoryObject* g_dllSharedObject;

GLOBAL APMInfo *g_apmInfo;

GLOBAL Uart* g_com2;

GLOBAL bool g_isRemoteDebug   GLOBAL_VAL(true);

#ifdef USE_BOOTMGR
GLOBAL BootManager* g_bootManager;
#endif

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define logprintf g_log->printf
#else
#define logprintf //
#endif

#endif
