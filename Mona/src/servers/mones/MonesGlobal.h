/*!
    \file  MonesGlobal.h
    \brief グローバル インスタンス宣言

    Copyright (c) 2004 Yamami
    All rights reserved.
    License=MIT/X Licnese

    \author  Yamami 
    \version $Revision$
    \date   create:2004/09/04 update:$Date$
*/

#ifndef _MONES_GLOBAL_
#define _MONES_GLOBAL_

#ifdef MONES_GLOBAL_VALUE_DEFINED
#define GLOBAL /* */
#define GLOBAL_VAL(v) = (v)
#else
#define GLOBAL extern "C"
//#define GLOBAL extern 
#define GLOBAL_VAL(v) /* */
#endif


#include <sys/types.h>
#include "MonesConfig.h"
#include "MoEther.h"
#include "MoArp.h"


//GLOBAL VirtualConsole* g_console GLOBAL_VAL(0);      /*< pointer to console    */
//GLOBAL VirtualConsole* g_log     GLOBAL_VAL(0);

//MonesConfigをインスタンス化
GLOBAL MonesConfig G_MonesCon;
GLOBAL MoEther* g_MoEther;
GLOBAL MoArp* g_MoArp;

#endif
