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
#include<monaOperator.h>
#include<monaIhandler.h>
#include<monaIdt.h>

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

    dword stack = kthread_allocate_stack();
    if (stack == NULL) {
        panic("idle thread:stack allocate error");
    }

    /* create idle thread */
    Kthread* idle = kthread_create_thread(stack, kthread_idle);
    if ( idle == NULL) {
        panic("idle thread:create thread error");
    }

    kthread_add_to_prev_list(&runningList, idle);

    dword stack2 = kthread_allocate_stack();
    if (stack2 == NULL) {
        panic("idle thread:stack allocate error");
    }

    /* create idle thread */
    Kthread* idle2 = kthread_create_thread(stack2, kthread_idle2);
    if ( idle2 == NULL) {
        panic("idle thread:create thread error");
    }

    kthread_add_to_prev_list(&runningList, idle2);

    current = idle2;
    //    kthread_schedule();
}

void kthread_printInfo() {

    Kthread* runlist = &runningList;
    console->printf("kthread:runlist[%x][%x][x]\n", runlist, runlist->next, runlist->next->next);

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
    thread->eflags = 0x0200046;
    thread->esp    = stack;
    thread->ebp    = stack;

    /* increment thread number */
    (kthreadInfo.threadNum)++;

    return thread;
}


extern "C" void write_font(int a, char b, char c);
extern "C" char pos_x;
extern "C" char pos_y;

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_idle() {

    console->printf("kthread idle\n");
    while (true) {
       console->printf("[1]");}

//      dword color = 0;

//      while (true) {

//          if (current->tick % 50) continue;
//          disableInterrupt();

//          int x = pos_x;
//          int y = pos_y;

//          pos_x = 20, pos_y = 1;
//          write_font('$', color%2 + 5, 0);
//          color++;
//          pos_x = x;
//          pos_y = y;
//          enableInterrupt();
//      }
}

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_idle2() {

    while (true)    { enableInterrupt();console->printf("[2]");}
//      dword color = 0;

//      while (true) {

//          disableInterrupt();

//          int x = pos_x;
//          int y = pos_y;

//          pos_x = 22, pos_y = 1;
//          write_font('$', color%15, 0);
//          color++;
//          pos_x = x;
//          pos_y = y;
//          enableInterrupt();
//      }
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

    //    current =  (&runningList)->next;

    /* switch */
    kthread_switch();
}

/*!
    \brief switch to next

    \author HigePon
    \date   create:2003/03/02 update:
*/
void kthread_switch() {

    console->printf("switch\n");
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
