/*!
    \file  kernel.h
    \brief definition for kernel

    definition for kernel & macros
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/21 update:$Date$
*/

#ifndef _HIGEPOS_KERNEL_
#define _HIGEPOS_KERNEL_

#define _sysLock()   asm("cli") /*! disable interupts */
#define _sysUnlock() asm("sti") /*! enable  interupts */

void startKernel(void);

#endif
