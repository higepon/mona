#ifdef GLOBAL_VALUE_DEFINED
#define GLOBAL /* */
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
#define GLOBAL_VAL(v) /* */
#endif

#include<kthread.h>
#include<info.h>
#include<kernel.h>
#include<VirtualConsole.h>
#include<FDCDriver.h>
#include<Semaphore.h>
#include<GDTUtil.h>
#include<IDTUtil.h>
#include<PagingUtil.h>
#include <sysresource.h> /* (expr) */

GLOBAL VirtualConsole* g_console;                    /*< pointer to console                  */

GLOBAL Kthread* g_kthread_current GLOBAL_VAL(NULL);  /*< pointer to current kernel thread    */
GLOBAL Kthread* g_kthread_idle    GLOBAL_VAL(NULL);  /*< pointer to kernel idle thread       */

//GLOBAL KthreadInfo g_kthreadInfo;                    /*< common thread Information           */
GLOBAL volatile KthreadInfo g_kthreadInfo;                    /* for poor optimize */

GLOBAL StackView g_stack_view;                       /*< struct for stack view               */

GLOBAL FDCDriver* g_fdcdriver;

GLOBAL dword g_demo_step GLOBAL_VAL(0);

/* semaphore */
GLOBAL semaphore g_semaphore_console GLOBAL_VAL(1);

/* expr:sysresource */
GLOBAL BitMap *g_irqMap;
GLOBAL IRQHandler g_irqHandlers[16];

/* Process name refered by info.cpp */
GLOBAL char g_process_name[16];
GLOBAL int  g_info_level GLOBAL_VAL(DEV_ERROR);

/* GLOBAL DESCRIPTOR TABLE */
GLOBAL SegDesc* g_gdt;

/* INTERRUPT DESCRIPTOR TABLE */
GLOBAL GateDesc* g_idt;

GLOBAL dword g_total_system_memory;

/* Paging */
GLOBAL PTE* g_page_dir;
GLOBAL PTE* g_page_tbl;
