/*!
    \file  MemoryManager.h
    \brief Abstract class MemoryManager

    Abstract class MemoryManager

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/03 update:$Date$
*/
#include<Object.h>

#ifndef _HIGEPOS_MEMORYMANAGER_
#define _HIGEPOS_MEMORYMANAGER_

/*!
    abstract class MemoryManager
*/
class MemoryManager:virtual Object {
  public:
    virtual char* getMessage() = 0;
    virtual char* getName() =0;
    virtual unsigned long allocateMemory(unsigned long) = 0;
    virtual unsigned long freeMemory(unsigned long) = 0;
};

#endif
