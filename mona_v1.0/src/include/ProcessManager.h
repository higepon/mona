/*!
    \file  ProcessManager.h
    \brief class ProcessManager

    class ProcessManager

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/07/06 update:$Date$
*/
#ifndef _MONA_PROCESS_MANAGER_
#define _MONA_PROCESS_MANAGER_

#include<types.h>
#include<Process.h>
#include<ihandlers.h>
#include<Scheduler.h>

/*!
    class ProcessManager
*/
class ProcessManager {

  public:
    ProcessManager();

    virtual ~ProcessManager() {
    }

  protected:
    PTE*  allocatePageDir();
    virtual_addr allocateStack();
    dword allocatePID();
    void switchProcess();

  public:
    void schedule();
    bool addProcess(Process* process, virtual_addr entry);

  private:
    dword pid_;
    Scheduler scheduler_;
};

#endif
