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

/*!
    abstract class MemoryManager
*/
class MemoryManager:public Object {
  public:
    virtual char* getMessage() = 0;
    virtual char* getName() =0;
};
