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

#ifndef _HIGE_POS_OPERATOR
#define _HIGE_POS_OPERATOR

void* operator new(unsigned long size);
void  operator delete(void* address);
int   getNumber();
char* getName();
#endif
