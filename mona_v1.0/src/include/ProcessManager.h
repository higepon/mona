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

#define GDTNUM 7 /*!< \def number of entry gdt */

/*!
    process management
*/
class ProcessManager {

  public:
    ProcessManager();
    void setTSS(TSS*, word, word, void (*f)(), dword, byte*, word, byte*, word);
    void setDT(GDT*, dword, dword, byte);
    inline void switchProcess(dword) const;
    void printInfo();
    void multiTaskTester();
    void schedule();
    static ProcessManager& instance() {
        static ProcessManager theInstance;
        return theInstance;
    }
    byte stack[5120];
    TSS tss[2];
  private:
    void sgdt();
    inline void ltr(word) const;
    inline void lldt(word) const;
    inline void setNTflag1() const;
    inline void switchProcess();
    inline void switchProcess2();
    inline void initProcess(void (*f)());
    GDT* gdt_;
    word taskidx_;
};

#endif
