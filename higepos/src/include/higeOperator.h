/*!
    \file   higeOperator.h
    \brief  define operator new & delete

    define operator new & delete

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/08/08 update:$Date$
*/
void* operator new(unsigned long size);
void  operator delete(void* address);
