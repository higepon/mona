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

extern void gdbCatchException(int exceptionVector);


extern "C" void arch_dummyhandler(void);
extern "C" void arch_exception0_divide_error();
extern "C" void arch_exception3_breakpoint();
extern "C" void arch_exception13_general_protection();
extern "C" void arch_switch_process(void);
extern "C" void arch_breakpoint_handler(void);
extern "C" void arch_switch_process_to_user_mode(void);
extern "C" void arch_switch_process_to_v86_mode(void);
extern "C" void arch_fault0dhandler(void);
extern "C" void arch_syscall_handler(void);

extern "C" void dummyHandler(void);
extern "C" void fault0dHandler(uint32_t error);

extern "C" void generalProtectionExceptionHandler(uint32_t error);
extern "C" void SendInterrupt(int irq);

/* IRQ Handler */
#define IRQHANDLERHEADER(x) extern "C" void irqHandler_##x(); extern "C" void arch_irqhandler_##x()
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

#define VECTOR_DIVIDE_ERROR_EXCEPTION 0
#define VECTOR_DEBUG_EXCEPTION 1
#define VECTOR_NMI_INTTERUPT 2
#define VECTOR_BREAKPOINT_EXCEPTION 3
#define VECTOR_OVERFLOW_EXCEPTION 4
#define VECTOR_BOUND_RANGE_EXCEEDED_EXCEPTION 5
#define VECTOR_INVALID_OP_CODE_EXCEPTION 6
#define VECTOR_DEVICE_NOT_AVAILABLE__EXCEPTION 7
#define VECTOR_DOUBLE_FAULT_EXCEPTION 8
#define VECTOR_COPROCESSOR_SEGMENT_OVERRUN 9
#define VECTOR_INVALID_TSS_EXCEPTION 10
#define VECTOR_SEGMENT_NOT_PRESENT 11
#define VECTOR_STACK_FAULT_EXCEPTION 12
#define VECTOR_GENERAL_PROTECTION_EXCEPTION 13
#define VECTOR_PAGE_FAULT_EXCEPTION 14
#define VECTOR_X87_FLOATING_POINT_ERROR 16
#define VECTOR_ALIGNMENT_CHECK_EXCEPTION 17
#define VECTOR_MACHINE_CHECK_EXCEPTION 18
#define VECTOR_SIMD_FLOATING_POINT_EXCEPTION 19




extern "C" void cpufaultHandler_e(uint32_t address, uint32_t error);
extern "C" void cpufaultHandler_c(uint32_t error);
extern "C" void arch_cpufaulthandler_c();
extern "C" void arch_cpufaulthandler_e();

void dokodemoView();

#endif
