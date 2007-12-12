/*!
    \file  Uart.h
    \brief Uart (Universal Asynchronous Receiver Transmitter) driver.

    Copyright (c) 2007 Higepon
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/12/13
*/

#ifndef __UART_H__
#define __UART_H__

#include "kernel.h"
#include "io.h"

class Uart
{
public:
    // devices
    typedef enum {
        COM1 = 0x03F8,
        COM2 = 0x02F8
    } Device;

public:
    Uart(Uart::Device device);
    virtual ~Uart();

    void writeChar(char c);


protected:
    enum {
        LINE_CONTROL_REGISTER   = 3,
        MODEM_CONTROL_REGISTER  = 4,
        BAUD_RATE_LSB_REGISTER  = 0,
        BAUD_RATE_MSB_REGISTER  = 1,
        CLOCK_BASE              = (1843200 / 16),
        BAUD_RATE               = 9600,
        BAUD_RATE_VALUE         = CLOCK_BASE / BAUD_RATE,
        BAUD_RATE_SETUP_ENABLE  = 0x80,
        BAUD_RATE_SETUP_DISABLE = 0x00,
        DATA_8BIT       = 0x03,
        STOP_2BIT       = 0x04,
        PARITY_NONE     = 0x00,
    };

    void wait();
    void out8(uint8_t reg, uint8_t value);

    Uart::Device device_;
};

#endif // __UART_H__
