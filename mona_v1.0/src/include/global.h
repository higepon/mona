#ifdef GLOBAL_VALUE_DEFINED
#define GLOBAL /* */
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
#define GLOBAL_VAL(v) /* */
#endif

#include<kthread.h>
#include<monaKernel.h>
#include<VirtualConsole.h>

GLOBAL VirtualConsole* g_console;                    /*< pointer to console                  */

GLOBAL Kthread* g_kthread_current GLOBAL_VAL(NULL);  /*< pointer to current kernel thread    */
GLOBAL KthreadInfo g_kthreadInfo;                    /*< common thread Information           */

GLOBAL StackView g_stack_view;                       /*< struct for stack view               */
