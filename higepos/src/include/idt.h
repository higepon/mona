/*!
    \file  idt.h
    \brief definition for IDT

    definition for IDT
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/23 update:$Date$
*/

#define HANDLER_NUM  256    /* number of handler     */
#define IDT_BASE     0x6800 /* idt base address      */
#define IDT_LOWBASE  0x6800 /* idt low base address  */
#define IDT_HIGHBASE 0x0000 /* idt high base address */
#define IDT_UNUSED   0x00   /* unused                */

/*! struct for idtr */
typedef struct idtr_st {
    unsigned short limit;    /*! idtr limit           */
    unsigned short lowbase;  /*! base address of idtr */
    unsigned short highbase; /*! base address of idtr */
};

/*! struct for idt */
typedef struct idt_st {
    unsigned short offsetL;  /*! 0-15bit of offset address */
    unsigned short selector; /*! selector address          */
    unsigned char  unused;   /*! unused                    */
    unsigned char  type;     /*! type                      */
    unsigned short offsetH;  /*!16-32bit of offset address */
};


/*! struct for interrupt handler */
typedef struct handler_st {
    int  number;       /* handler number              */
    void (*handler)(); /* pointer to handler function */
};

void _sysSetIdt(void);
void _sysLoadIdtr(idtr_st*);
void setIdt(handler_st*, int selector);
unsigned char inportb(unsigned int);
