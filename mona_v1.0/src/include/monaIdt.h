/*!
    \file  monaIdt.h
    \brief definition for IDT

    definition for IDT
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/23 update:$Date$
*/

#ifndef _MONA_IDT_
#define _MONA_IDT_

#define HANDLER_NUM  256    /* number of handler     */
#define IDT_BASE     0x0000 /* idt base address      */
#define IDT_LOWBASE  0x0000 /* idt low base address  */
#define IDT_HIGHBASE 0x0000 /* idt high base address */
#define IDT_UNUSED   0x00   /* unused                */

#define iret() asm volatile("mov %ebp,%esp\n"  \
                            "pop %ebp\n iretl" \
                           ); /*!< \def iret */


/*! \struct idtr */
typedef struct idtr_st {
    unsigned short limit;    /*!< idtr limit           */
    unsigned short lowbase;  /*!< base address of idtr */
    unsigned short highbase; /*!< base address of idtr */
};

/*! \struct idt */
typedef struct idt_st {
    unsigned short offsetL;  /*!< 0-15bit of offset address */
    unsigned short selector; /*!< selector address          */
    unsigned char  unused;   /*!< unused                    */
    unsigned char  type;     /*!< type                      */
    unsigned short offsetH;  /*!< 16-32bit of offset address */
};


/*! \struct handler_st */
typedef struct handler_st {
    int  number;       /*< handler number              */
    void (*handler)(); /*< pointer to handler function */
};

void _sysSetIdt(void);
void _sysLoadIdtr(idtr_st*);
void setIdt(handler_st*, int selector);
void fault0dHandler();
unsigned char inportb(unsigned int);
#endif
