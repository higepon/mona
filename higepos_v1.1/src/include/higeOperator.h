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

#include<higeTypes.h>

#ifndef _HIGE_POS_OPERATOR
#define _HIGE_POS_OPERATOR

void* operator new(size_t size);
void  operator delete(void* address);
void* malloc(unsigned long size);
void __builtin_delete(void* address);
void* __builtin_new(unsigned long size);
void* __builtin_vec_new(unsigned long size);
void __builtin_vec_delete(void* address);
#endif
