/*!
    \file  ProcessManager.h
    \brief ProcessManager

    ProcessManager, management of maltitask.

    Copyright (c) 2002,2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/19 update:$Date$
*/
#ifndef _MONA_PROCESSMANAGER_
#define _MONA_PROCESSMANAGER_

#include <monaKernel.h>
#include <IA32MemoryManager.h>

#define GDTNUM 7                   /*!< \def number of entry gdt   */
#define FIRST_PROCESS_STACK 0x9884 /*!< \def stack of firstprocess */

/*! \def switch process */
#define _switchProcess(currentProcess, nextProcess) \
                                                    \
    /* no switch */                                 \
    if (currentProcess == nextProcess) iret();      \
                                                    \
    _sys_printf("switchProcess");\
    _sysDumpProcess("next"   , pm.next);\
    _sysDumpProcess("current", pm.current);\
    /* switch to next */                            \
    asm volatile(                                   \
                 "movl  %%esp, %0    \n"            \
                 "movl  %1, %%eax    \n"            \
                 "movl  %2, %%ebx    \n"            \
                 "movl  %3, %%ecx    \n"            \
                 "movl  %4, %%edx    \n"            \
                 "movl  %5, %%edi    \n"            \
                 "movl  %6, %%esi    \n"            \
                 "movl  %7, %%esp    \n"            \
                 "movl  %8, %%ebp    \n"            \
                 "pushl %9           \n"            \
                 "pushl %10          \n"            \
                 "pushl %11          \n"            \
                 "iretl              \n"            \
                 : "=m"(currentProcess->esp)        \
                 : "m"(nextProcess->eax)            \
                 , "m"(nextProcess->ebx)            \
                 , "m"(nextProcess->ecx)            \
                 , "m"(nextProcess->edx)            \
                 , "m"(nextProcess->edi)            \
                 , "m"(nextProcess->esi)            \
                 , "m"(nextProcess->esp)            \
                 , "m"(nextProcess->ebp)            \
                 , "m"(nextProcess->eflags)         \
                 , "m"(nextProcess->cs)             \
                 , "m"(nextProcess->eip)            \
                 );                                 \
_sysdumpStack(); \
_sysdumpReg("reg", true, false);\


/*! \def save registers */
#define _saveRegisters(process) {            \
                                             \
    asm volatile("movl %%ebx,     %0     \n" \
                 "movl 4(%%ebp) , %%ebx  \n" \
                 "movl %%ebx    , %1     \n" \
                 "movl 8(%%ebp) , %%ebx  \n" \
                 "movl %%ebx    ,  %2    \n" \
                 "movl 12(%%ebp), %%ebx  \n" \
                 "movl %%ebx    ,  %3    \n" \
                 "movl (%%ebp), %%ebx    \n" \
                 "movl %%ebx    ,  %4    \n" \
                 "movl %%eax    ,  %5    \n" \
                 "movl %%ecx    ,  %6    \n" \
                 "movl %%edx    ,  %7    \n" \
                 "movl %%esi    ,  %8    \n" \
                 "movl %%edi    ,  %9    \n" \
                 "movl %%ebp    ,  %10   \n" \
                 : "=m"(process->ebx)        \
                 , "=m"(process->eip)        \
                 , "=m"(process->cs)         \
                 , "=m"(process->eflags)     \
                 , "=m"(process->ebp)        \
                 , "=m"(process->eax)        \
                 , "=m"(process->ecx)        \
                 , "=m"(process->edx)        \
                 , "=m"(process->esi)        \
                 , "=m"(process->edi)        \
                 , "=m"(process->esp)        \
                 );                          \
}                                            \

#define _sysDumpProcess(str, process) {           \
                                                  \
    _sys_printf("%s ", str);                      \
    _sys_printf("eax=(%x)", process->eax);        \
    _sys_printf("ebx=(%x)", process->ebx);        \
    _sys_printf("ecx=(%x)", process->ecx);        \
    _sys_printf("edx=(%x)", process->edx);        \
    _sys_printf("esi=(%x)", process->esi);        \
    _sys_printf("edi=(%x)", process->edi);        \
    _sys_printf("esp=(%x)", process->esp);        \
    _sys_printf("ebp=(%x)", process->ebp);        \
    _sys_printf("eflags=(%x)", process->eflags);  \
    _sys_printf("cs=(%x)", process->cs);          \
    _sys_printf("eip=(%x)", process->eip);        \
}                                                 \

/*! \def struct for process */
typedef struct Process {
    dword* esp;
    dword* ebp;
    dword  eip;
    dword  cs;
    dword  eflags;
    dword  eax;
    dword  ecx;
    dword  edx;
    dword  ebx;
    dword  esi;
    dword  edi;
};

/*!
    process management
*/
class ProcessManager {

  public:
    ProcessManager();
    void setTSS(TSS*, word, word, void (*f)(), dword, byte*, word, byte*, word);
    void setDT(GDT*, dword, dword, byte);
    void printInfo();
    void multiTaskTester();
    void schedule();
    static ProcessManager& instance() {
        static ProcessManager theInstance;
        return theInstance;
    }
    byte stack[5120];
    TSS tss[2];
    Process* current;
    Process* next;
  private:
    void sgdt();
    inline void ltr(word) const;
    inline void lldt(word) const;
    inline void setNTflag1() const;
    inline void initProcess(void (*f)());
    inline void switchProcess(dword) const;
    GDT* gdt_;
    Process process_[2];
};

#endif
