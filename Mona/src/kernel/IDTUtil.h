/*!
    \file  IDTUtil.h
    \brief some functions for IDT set up

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/08 update:$Date$
*/
#ifndef _MONA_IDTUTIL_
#define _MONA_IDTUTIL_

#include "GDTUtil.h"

#define IHANDLER_NUM  256

typedef GDTR IDTR;

/*! \struct GateDesc */
typedef struct {
    word offsetL;  /*!< 0-15bit of offset address */
    word selector; /*!< selector address          */
    byte unused;   /*!< unused                    */
    byte type;     /*!< type                      */
    word offsetH;  /*!< 16-32bit of offset address */
} GateDesc;

/*! \struct InterruptHandlers */
typedef struct {
    int  number;       /*< handler number              */
    void (*handler)(); /*< pointer to handler function */
} InterruptHandlers;

class IDTUtil {

  public:
    static void lidt(IDTR* idtr);
    static void setGateDesc(GateDesc* descZero, word selector, InterruptHandlers* handler);
    static void setup();
};

#endif
