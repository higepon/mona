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
    /* switch to next */                            \
    asm volatile(                                   \
                 "movl %%ebp, %%esp\n  "            \
                 "pushal           \n"              \
                 "mov %%esp, %0    \n"              \
                 "mov %1, %%esp    \n"              \
                 "popal            \n"              \
                 "addl $4, %%esp   \n"              \
                 "iretl            \n"              \
                 : "=m" (currentProcess->esp)       \
                 : "m" (nextProcess->esp)           \
                 );                                 \

/*! \def save all registers */
#define _saveRegisters(process) {            \
                                             \
    asm volatile("movl %%ebx,     %0     \n" \
                 "movl 4(%%ebp) , %%ebx  \n" \
                 "movl %%ebx    , %1     \n" \
                 "movl 8(%%ebp) , %%ebx  \n" \
                 "movl %%ebx    ,  %2    \n" \
                 "movl 12(%%ebp), %%ebx  \n" \
                 "movl %%ebx    ,  %3    \n" \
                 "movl %%eax    ,  %4    \n" \
                 "movl %%ecx    ,  %5    \n" \
                 "movl %%edx    ,  %6    \n" \
                 "movl %%esi    ,  %7    \n" \
                 "movl %%edi    ,  %8    \n" \
                 : "=m"(process->ebx)        \
                 , "=m"(process->eip)        \
                 , "=m"(process->cs)         \
                 , "=m"(process->eflags)     \
                 , "=m"(process->eax)        \
                 , "=m"(process->ecx)        \
                 , "=m"(process->edx)        \
                 , "=m"(process->esi)        \
                 , "=m"(process->edi)        \
                 );                          \
}                                            \

/*! \def struct for process */
typedef struct Process {
    dword* esp;
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
