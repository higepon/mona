/*!
    \file  kthread.cpp
    \brief kernel thread

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/01 update:$Date$
*/

#include<kthread.h>
#include<monaKernel.h>
#include<monaIhandler.h>
#include<monaIdt.h>
#include<pic.h>
#include<monaOperator.h>
#include<disp.h>

Kthread*    current = NULL; /*< pointer to current kernel thread    */
Kthread    runningList;
KthreadInfo kthreadInfo;    /*< common thread Information           */

/*!
    \brief init kernel thread

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_init() {

    console->printf("kthread init");

    kthread_init_list(&runningList);

    /* thread 1 */
    dword stack = kthread_allocate_stack();
    if (stack == NULL) {
        panic("idle thread:stack allocate error");
    }
    Kthread* disp1 = kthread_create_thread(stack, disp_name1);
    if (disp1 == NULL) {
        panic("idle thread:create thread error");
    }
    kthread_add_to_prev_list(&runningList, disp1);

    /* thread2 */
    dword stack2 = kthread_allocate_stack();
    if (stack2 == NULL) {
        panic("idle thread:stack allocate error");
    }
    Kthread* disp2 = kthread_create_thread(stack2, disp_name2);
    if (disp2 == NULL) {
        panic("idle thread:create thread error");
    }
    kthread_add_to_prev_list(&runningList, disp2);

    /* thread3 */
//      dword stack3 = kthread_allocate_stack();
//      if (stack3 == NULL) {
//          panic("idle thread:stack allocate error");
//      }
//      Kthread* disp3 = kthread_create_thread(stack3, disp_name3);
//      if (disp3 == NULL) {
//          panic("idle thread:create thread error");
//      }
//      kthread_add_to_prev_list(&runningList, disp3);

//      /* thread4 */
//      dword stack4 = kthread_allocate_stack();
//      if (stack4 == NULL) {
//          panic("idle thread:stack allocate error");
//      }
//      Kthread* disp4 = kthread_create_thread(stack4, disp_name4);
//      if (disp4 == NULL) {
//          panic("idle thread:create thread error");
//      }
    //    kthread_add_to_prev_list(&runningList, disp4);

    current = disp2;
    kthread_schedule();
}

/*!
    \brief thread tick

    increment thread tick

    \author HigePon
    \date   create:2003/03/01 update:
*/
void kthread_tick() {

    (kthreadInfo.tick)++;
    (current->tick)++;
    return;
}



#define KTHREAD_STACK_START 0x800000
#define KTHREAD_STACK_END   0x700000
#define KTHREAD_STACK_SIZE  0x2000
/*!
    \brief allcate stack for new thread

    \return stack address. NULL when not enough stack.

    \author HigePon
    \date   create:2003/03/02 update:
*/
dword kthread_allocate_stack() {

    static int allocated = 0;
    dword stack = KTHREAD_STACK_START - KTHREAD_STACK_SIZE * allocated;

    allocated++;

    /* stack not enough */
    if (stack < KTHREAD_STACK_END) return NULL;

    return stack;
}

/*!
    \brief thread yeild

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_yield() {

    /* software interrupt yeild */
    kthread_schedule();
}


/*!
    \brief thread yeild

    \param stack stack address
    \param f     entry point

    \return potinter to Kthread. NULL when failed

    \author HigePon
    \date   create:2003/03/02 update:
*/
Kthread* kthread_create_thread(dword stack, void (*f)()) {

    /* create kthread struct */
    Kthread* thread = (Kthread*)malloc(sizeof(Kthread));
    if (thread == NULL) return NULL;

    /* 0 clear */
    memset(thread, 0, sizeof(Kthread));

    /* create thread */
    thread->eip    = (dword)f;
    thread->cs     = 0x08;
    thread->eflags = 0x0200;
    thread->esp    = stack;
    thread->ebp    = stack;

    /* increment thread number */
    (kthreadInfo.threadNum)++;

    return thread;
}

/*!
    \brief schedule

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_schedule() {

    /* change runlist */
    Kthread* temp = kthread_get_next_from_list(&runningList);

    kthread_add_to_prev_list(&runningList, temp);

    current =  (&runningList)->next;

    /* switch */
    kthread_switch();
}

/*!
    \brief switch to next

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_switch() {

    arch_kthread_switch();
}


void kthread_init_list(Kthread* list) {

    list->prev = list;
    list->next = list;
    return;
}

bool kthread_is_list_empty(Kthread* list) {

    if (list->next == list) return true;
    return false;
}

/*!
    \brief add thread to list

    \param list   thread list
    \param thread thraed added to list

    \author HigePon
    \date   create:2003/03/01 update:2003/03/06
*/
void kthread_add_to_prev_list(Kthread* list, Kthread* thread) {

    thread->prev = list->prev;
    thread->next = list;
    list->prev->next = thread;
    list->prev = thread;
    return;
}

/*!
    \brief remove thread from list

    \param list   thread list
    \param thread thread to removed from list

    \author HigePon
    \date   create:2003/03/01 update:2003/03/06
*/
void kthread_remove_from_list(Kthread* list, Kthread* thread) {

    thread->prev->next = thread->next;
    thread->next->prev = thread->prev;
    return;
}


Kthread* kthread_get_next_from_list(Kthread* list) {

    Kthread* result;

    result = list->next;
    list->next = result->next;
    result->next->prev = list;
    return(result);
}
