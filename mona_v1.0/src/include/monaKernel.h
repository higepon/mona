/*!
    \file  monaKernel.h
    \brief definition for kernel

    definition for kernel & macros
    Copyright (c) 2002, 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#ifndef _MONA_KERNEL_
#define _MONA_KERNEL_

#include <monaTypes.h>

#define _sysLock()   asm volatile("cli")    /*!< \def disable interupts */
#define _sysUnlock() asm volatile("sti")    /*!< \def enable  interupts */
#define pusha()      asm volatile("pusha"); /*!< \def  pusha            */
#define popa()       asm volatile("popa");  /*!< \def  popa             */


/*!< \def _sys_dumpReg() */
#define _sysdumpReg(str, stopflag, unlockint) { \
    _sysLock();                                 \
    dword __eax, __ebx, __ecx, __edx;           \
    dword __esp, __ebp, __esi, __edi;           \
    dword __cs , __ds , __fs , __es;            \
    dword __gs, __ss  , __eflags;               \
    asm volatile("xor %%eax, %%eax \n"          \
                 "mov %%eax, %0    \n"          \
                 "mov %%ebx, %1    \n"          \
                 "mov %%ecx, %2    \n"          \
                 "mov %%edx, %3    \n"          \
                 "mov %%esp, %4    \n"          \
                 "mov %%ebp, %5    \n"          \
                 "mov %%esi, %6    \n"          \
                 "mov %%edi, %7    \n"          \
                 "mov %%cs , %8    \n"          \
                 "mov %%ds , %9    \n"          \
                 "mov %%es , %10   \n"          \
                 "mov %%fs , %11   \n"          \
                 "mov %%gs , %12   \n"          \
                 "mov %%ss , %13   \n"          \
                 "xor %%eax, %%eax \n"          \
                 "pushf            \n"          \
                 "pop %%eax        \n"          \
                 "mov %%eax, %14   \n"          \
                 : "=m" (__eax)                 \
                 , "=m" (__ebx)                 \
                 , "=m" (__ecx)                 \
                 , "=m" (__edx)                 \
                 , "=m" (__esp)                 \
                 , "=m" (__ebp)                 \
                 , "=m" (__esi)                 \
                 , "=m" (__edi)                 \
                 , "=m" (__cs)                  \
                 , "=m" (__ds)                  \
                 , "=m" (__es)                  \
                 , "=m" (__fs)                  \
                 , "=m" (__gs)                  \
                 , "=m" (__ss)                  \
                 , "=m" (__eflags)              \
                 : /* no input */               \
                );                              \
    _sys_printf("%s _sysdump()\n", str);        \
    _sys_printf("eax=%x ebx=%x ecx=%x edx=%x\n" \
                , __eax, __ebx, __ecx, __edx);  \
    _sys_printf("esp=%x ebp=%x esi=%x edi=%x\n" \
                , __esp, __ebp, __esi, __edi);  \
    _sys_printf("cs =%x ds =%x es =%x fs =%x\n" \
                , __cs , __ds , __es , __fs);   \
    _sys_printf("gs =%x ss =%x eflags=%x\n"     \
                , __gs , __ss , __eflags);      \
     if (unlockint)_sysUnlock();                \
     if (stopflag) while (true);                \
}

/*!< \def _sys_dumpStack() */
#define _sysdumpStack() {                     \
                                              \
    dword stack0, stack1, stack2;             \
                                              \
    asm volatile(                             \
                 "pusha                 \n"   \
                 "mov  32(%%esp), %%eax \n"   \
                 "mov  %%eax, %0        \n"   \
                 "mov  36(%%esp), %%eax \n"   \
                 "mov  %%eax, %1        \n"   \
                 "mov  40(%%esp), %%eax \n"   \
                 "mov  %%eax, %2        \n"   \
                 "popa                  \n"   \
                 : "=m"(stack0)               \
                 , "=m"(stack1)               \
                 , "=m"(stack2)               \
                );                            \
                                              \
    _sys_printf("_sysdumpStack(): %x %x %x\n" \
               , stack0, stack1, stack2);     \
}                                             \

#define _sysdumpStack2(str) {                          \
                                                       \
    dword* stack;                                      \
    asm volatile("mov %%ebp, %0 \n" : "=m"(stack));    \
    _sys_printf("_sysdumpStack2(%s:%x):%x %x %x\n"     \
              , str, stack, *(stack + 1), *(stack + 2) \
              , *(stack + 3));                         \
}

#define _sysdumpStack3(str) {                          \
                                                       \
    dword* stack;                                      \
    asm volatile("mov %%esp, %0 \n" : "=m"(stack));    \
    _sys_printf("_sysdumpStack3(%s:%x):%x %x %x\n"     \
              , str, stack, *(stack + 1), *(stack + 2) \
              , *(stack + 2));                         \
}

#define _sysdumpStack4()                             \
                                                     \
    static dword* stack;                             \
    asm volatile("mov %%ebp, %0 \n" : "=g"(stack));  \
    _sys_printf("%x  ", *(stack + 0));               \
    _sys_printf("%x  ", *(stack + 1));               \
    _sys_printf("%x  ", *(stack + 2));               \
    _sys_printf("%x  ", *(stack + 3));               \
    _sys_printf("%x  ", *(stack + 4));               \
    _sys_printf("%x  ", *(stack + 5));               \
    _sys_printf("%x  ", *(stack + 6));               \
    _sys_printf("%x  ", *(stack + 7));               \
    _sys_printf("%x  ", *(stack + 8));               \

#define _sysdumpStack5()                             \
                                                     \
    static dword* stack;                             \
    dword value[20];                                  \
    asm volatile("mov %%ebp, %0 \n" : "=g"(stack));  \
        value[0] = *(stack + 0);\
       value[1] = *(stack + 1);\
value[2] = *(stack + 2);\
value[3] = *(stack + 3);\
value[4] = *(stack + 4);\
value[5] = *(stack + 5);\
value[6] = *(stack + 6);\
value[7] = *(stack + 7);\
value[8] = *(stack + 8);\
value[9] = *(stack + 9);\
value[10] = *(stack + 10);\
value[11] = *(stack + 11);\
value[12] = *(stack + 12);\
value[13] = *(stack + 13);\
value[14] = *(stack + 14);\
value[15] = *(stack + 15);\
value[16] = *(stack + 16);\
value[17] = *(stack + 17);\
value[18] = *(stack + 18);\
value[19] = *(stack + 19);\
    _sys_printf("%x  ", value[0]);               \
    _sys_printf("%x  ", value[1]);               \
    _sys_printf("%x  ", value[2]);               \
    _sys_printf("%x  ", value[3]);               \
    _sys_printf("%x  ", value[4]);               \
    _sys_printf("%x  ", value[5]);               \
    _sys_printf("%x  ", value[6]);               \
    _sys_printf("%x  ", value[7]);               \
    _sys_printf("%x  ", value[8]);               \
    _sys_printf("%x  ", value[9] );               \
    _sys_printf("%x  ", value[10]);               \
    _sys_printf("%x  ", value[11]);               \
    _sys_printf("%x  ", value[12]);               \
    _sys_printf("%x  ", value[13]);               \
    _sys_printf("%x  ", value[14]);               \
    _sys_printf("%x  ", value[15]);               \
    _sys_printf("%x  ", value[16]);               \
    _sys_printf("%x  ", value[17]);               \
    _sys_printf("%x  ", value[18]);               \
    _sys_printf("%x  ", value[19]);               \

/*!
   \struct FARJMP
*/
typedef struct {
    dword offset;
    word selector;
} FARJMP;

/*!
    \struct TSS
*/
typedef struct {
    word  backlink;
    word  pad0;
    dword esp0;
    word  ss0;
    word  pad1;
    dword esp1;
    word  ss1;
    word  pad2;
    dword esp2;
    word  ss2;
    word  pad3;
    dword cr3;
    dword eip;
    dword eflags;
    dword eax;
    dword ecx;
    dword edx;
    dword ebx;
    dword esp;
    dword ebp;
    dword esi;
    dword edi;
    word  es;
    word  pad4;
    word  cs;
    word  pad5;
    word  ss;
    word  pad6;
    word  ds;
    word  pad7;
    word  fs;
    word  pad8;
    word  gs;
    word  pad9;
    word  ldt;
    word  padA;
    word  debugtrap;
    word  iobase;
} TSS;

void startKernel(void);
void panic(const char*);

#endif
