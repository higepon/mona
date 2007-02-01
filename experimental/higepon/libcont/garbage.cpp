/*!
  \file   main.cpp
  \brief  libcont

  Copyright (c) 2007 Higepon
  WITHOUT ANY WARRANTY

  \author  Higepon
  \version $Revision$
  \date   create:2007/01/30 update:$Date$
*/
#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif
#include <ucontext.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>

#define PTR_MANGLE(var)       asm ("xorl %%gs:%c2, %0"                      \
                                     : "=r" (var)                             \
                                     : "0" (var),                             \
                                       "i" (offsetof (tcbhead_t,              \
                                                      pointer_guard)))
#  define PTR_DEMANGLE(var)     PTR_MANGLE (var)

#if __GNUC__ == 4
void* get_stack_pointer2() __attribute__ ((noinline));
void* get_stack_pointer() __attribute__ ((noinline));
#endif


void* get_stack_base_pointer();

typedef struct Continuation
{
    jmp_buf env;
    ucontext_t context;
    uint32_t stackSize;
    void* stack;
};

inline int cont_store(Continuation* cont)
{
    return setjmp(cont->env);
}

inline void cont_restore(Continuation* cont)
{
    return longjmp(cont->env, 1);
}

static Continuation cont1;
static Continuation cont2;
static void* start_stack_pointer;

void test1_1(int i)
{
    printf("%s %d\n", __func__, i);
    if (i == 5)
    {
        printf("try restore from continuation\n");
        cont_restore(&cont1);
    }
    test1_1(i + 1);
}

void test1()
{
    if (setjmp(cont1.env) == 0)
    {
        test1_1(0);
    }
    else
    {
    }
}

void test2_10(int i)
{
    if (i == 0)
    {
        void* stack_pointer = get_stack_pointer();
        printf("%d\n", (uint32_t)start_stack_pointer - (uint32_t)stack_pointer);
        cont2.stackSize = (uint32_t)start_stack_pointer - (uint32_t)stack_pointer;
        cont2.stack = (uint8_t*)malloc(cont2.stackSize);
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        memcpy(cont2.stack, stack_pointer, cont2.stackSize);

        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        if (setjmp(cont2.env) == 0)
        {

        }
        else
        {
            printf("restore from continuation\n");fflush(stdout);
            i = 4;
        }
    }
    printf("%s %d\n", __func__, i);

    if (i == 5)
    {

        return;
    }
    test2_10(i + 1);
}

int test2_4()
{
    int a = 1234;
    void* p = get_stack_pointer();
    void* pp2 = get_stack_pointer2();
    int jmp_result = setjmp(cont2.env);
    void* pp3;
    void* pp4;
    asm volatile("movl %%esp, %0\n" : /* no output */ : "m" (pp3));
   asm volatile("movl %%ebp, %0\n" : /* no output */ : "m" (pp4));
    printf("saved ebp = %x\n", cont2.env[0].__jmpbuf[3]);fflush(stdout);
    void* pp5 = (void*)cont2.env[0].__jmpbuf[4];
    PTR_DEMANGLE(pp5);
    printf("saved esp = %x\n", pp5);fflush(stdout);
    printf("saved eip = %x\n", cont2.env[0].__jmpbuf[5]);fflush(stdout);

    printf("saved pc = %x\n", &test2_4);fflush(stdout);

    printf("2_4 esp %s %x\n", __func__, pp3);
    printf("2_4 ebp %s %x\n", __func__, pp4);


//  ctx->cc[0].__jmpbuf[0].__pc = func;
//  ctx->cc[0].__jmpbuf[0].__sp = stack;

    printf("%d address=%x\n", a, &a);
    return jmp_result;
}

int test2_3()
{
    printf("%s\n", __func__);
    return test2_4();
}

int test2_2()
{
    printf("%s\n", __func__);
    return test2_3();
}

int test2_1()
{
    printf("%s\n", __func__);
    return test2_2();
}

void stack_destroy()
{
    char buf[250];
    memset(buf, 0xff, 250);
    printf("%x", buf[249]);
}

int test2()
{
    if (0 == test2_1())
    {
        printf("try restore\n");fflush(stdout);
        printf("saved stack = %x\n", cont2.env[0].__jmpbuf[3]);fflush(stdout);
        stack_destroy();
        printf("current stack = %x\n", get_stack_pointer());

        exit(-1);
        cont_restore(&cont2);
    }
    else
    {
        printf("done\n");
    }
}

void* get_stack_pointer2()
{
    uint32_t a;
    uint32_t *b = &a; // to avoid compiler warning about unused variables
    return (void*)((unsigned int)b + 8);
}

void* get_stack_pointer()
{
    register void* stack_pointer asm ("%esp");
    return (void*)((unsigned int)stack_pointer + 8);
}


void* get_stack_base_pointer()
{
    register void* stack_pointer asm ("%ebp");
    return (void*)((unsigned int)stack_pointer + 8);
}

int main(int argc, char *argv[])
{
#if defined(__GLIBC__) && defined(__GLIBC_MINOR__) \
    && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 0 && defined(JB_PC) && defined(JB_SP)
#error "1PCL: Unsupported setjmp/longjmp platform. Please report to <davidel@xmailserver.org>"
#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__) \
    && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 0 && defined(__mc68000__)
#error "21PCL: Unsupported setjmp/longjmp platform. Please report to <davidel@xmailserver.org>"
#elif defined(__GNU_LIBRARY__) && defined(__i386__)
//#error "3PCL: Unsupported setjmp/longjmp platform. Please report to <davidel@xmailserver.org>"
#elif defined(_WIN32) && defined(_MSC_VER)
#error "4PCL: Unsupported setjmp/longjmp platform. Please report to <davidel@xmailserver.org>"
#elif defined(__GLIBC__) && defined(__GLIBC_MINOR__) \
    && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 0 && (defined(__powerpc64__) || defined(__powerpc__))
#error "5PCL: Unsupported setjmp/longjmp platform. Please report to <davidel@xmailserver.org>"
#else
#error "6PCL: Unsupported setjmp/longjmp platform. Please report to <davidel@xmailserver.org>"
#endif

    start_stack_pointer = get_stack_pointer();
//    test1();
    test2();
    return 0;
}

//     void* stack_pointer = get_stack_pointer();
//     printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//     memcpy((void*)((uint32_t)stack_pointer + cont2.stackSize), cont2.stack, cont2.stackSize);
//     printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
