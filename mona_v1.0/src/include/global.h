#ifdef GLOBAL_VALUE_DEFINED
#define GLOBAL /* */
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
#define GLOBAL_VAL(v) /* */
#endif

#include<kthread.h>
#include<kernel.h>
#include<VirtualConsole.h>
#include<MFDCDriver.h>
#include<Semaphore.h>

GLOBAL VirtualConsole* g_console;                    /*< pointer to console                  */

GLOBAL Kthread* g_kthread_current GLOBAL_VAL(NULL);  /*< pointer to current kernel thread    */
GLOBAL Kthread* g_kthread_idle    GLOBAL_VAL(NULL);  /*< pointer to kernel idle thread       */

GLOBAL KthreadInfo g_kthreadInfo;                    /*< common thread Information           */

GLOBAL StackView g_stack_view;                       /*< struct for stack view               */

GLOBAL MFDCDriver* g_fdcdriver;

GLOBAL dword g_demo_step GLOBAL_VAL(0);

/* semaphore */
GLOBAL semaphore g_semaphore_console GLOBAL_VAL(1);
