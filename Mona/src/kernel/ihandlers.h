/*!
    \file  ihandlers.h
    \brief interrupt handlers

    definition for IDT
    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/02/24 update:$Date$
*/
#ifndef _MONA_IHANDLER_
#define _MONA_IHANDLER_


extern "C" void arch_fdchandler(void);
extern "C" void arch_timerhandler(void);
extern "C" void arch_keystrokehandler(void);
extern "C" void arch_mousehandler(void);
extern "C" void arch_dummyhandler(void);
extern "C" void arch_switch_process(void);
extern "C" void arch_switch_process_to_user_mode(void);
extern "C" void arch_switch_process_to_v86_mode(void);
extern "C" void arch_fault0dhandler(void);
extern "C" void arch_syscall_handler(void);

extern "C" void MFDCHandler(void);
extern "C" void timerHandler(void);
extern "C" void keyStrokeHandler(dword scancode);
extern "C" void mouseHandler();
extern "C" void dummyHandler(void);
extern "C" void fault0dHandler(dword error);
extern "C" void doIrqHandler(int irq);

/* expr:IRQ Handler */
#define IRQHANDLERHEADER(x) extern "C" void irqHandler_##x(void); extern "C" void arch_irqhandler_##x(void)
IRQHANDLERHEADER(0);
IRQHANDLERHEADER(1);
IRQHANDLERHEADER(2);
IRQHANDLERHEADER(3);
IRQHANDLERHEADER(4);
IRQHANDLERHEADER(5);
IRQHANDLERHEADER(6);
IRQHANDLERHEADER(7);
IRQHANDLERHEADER(8);
IRQHANDLERHEADER(9);
IRQHANDLERHEADER(10);
IRQHANDLERHEADER(11);
IRQHANDLERHEADER(12);
IRQHANDLERHEADER(13);
IRQHANDLERHEADER(14);
IRQHANDLERHEADER(15);

#define FAULTHANDLERHEADER(x) extern "C" void cpufaultHandler_##x(void); extern "C" void arch_cpufaulthandler_##x(void)
FAULTHANDLERHEADER(0);
FAULTHANDLERHEADER(1);
FAULTHANDLERHEADER(5);
FAULTHANDLERHEADER(6);
FAULTHANDLERHEADER(7);
FAULTHANDLERHEADER(8);
FAULTHANDLERHEADER(a);
FAULTHANDLERHEADER(b);
//FAULTHANDLERHEADER(c);
//FAULTHANDLERHEADER(e);
FAULTHANDLERHEADER(10);
FAULTHANDLERHEADER(11);


extern "C" void cpufaultHandler_e(dword address, dword error);
extern "C" void cpufaultHandler_c(dword error);
extern "C" void arch_cpufaulthandler_c();
extern "C" void arch_cpufaulthandler_e();

void dokodemoView();

#endif
