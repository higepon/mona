/*!
    \file  ProcessManager.h
    \brief ProcessManager

    ProcessManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/19 update:$Date$
*/
#ifndef _MONA_PROCESSMANAGER_
#define _MONA_PROCESSMANAGER_

#include <monaKernel.h>

/*!
    process management
*/
class ProcessManager {

  public:
    void setTSS(TSS*, word, word, void (*f)(), dword, byte*, word, byte*, word);
    void switchProcess();
    void switchProcess(word);
    void printInfo();
};

#endif
