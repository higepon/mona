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


/*!< \def _sys_dump() */
#define _sysdump(str, stopflag, unlockint) {    \
    _sysLock();                                 \
    dword __eax, __ebx, __ecx, __edx;           \
    dword __esp, __ebp, __esi, __edi;           \
    dword __cs , __ds , __fs , __es;            \
    dword __gs, __ss  , __eflags;               \
    asm volatile("mov %%eax, %0  \n"            \
                 "mov %%ebx, %1  \n"            \
                 "mov %%ecx, %2  \n"            \
                 "mov %%edx, %3  \n"            \
                 "mov %%esp, %4  \n"            \
                 "mov %%ebp, %5  \n"            \
                 "mov %%esi, %6  \n"            \
                 "mov %%edi, %7  \n"            \
                 "mov %%cs , %8  \n"            \
                 "mov %%ds , %9  \n"            \
                 "mov %%es , %10 \n"            \
                 "mov %%fs , %11 \n"            \
                 "mov %%gs , %12 \n"            \
                 "mov %%ss , %13 \n"            \
                 "pushf          \n"            \
                 "pop %%eax      \n"            \
                 "mov %%eax, %14 \n"            \
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
                 : "ax"                         \
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
