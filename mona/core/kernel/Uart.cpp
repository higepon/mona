/*!
    \file  Uart.cpp
    \brief Uart (Universal Asynchronous Receiver Transmitter) driver.

    Copyright (c) 2007 Higepon
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/12/13
*/

#include "Uart.h"

Uart::Uart(Uart::Device device) : device_(device)
{
    // setup some parameters
    //   enable setuping baud rate.
    //   send/recive data size 8bit.
    //   no parity
    out8(LINE_CONTROL_REGISTER, (BAUD_RATE_SETUP_ENABLE | DATA_8BIT | PARITY_NONE));
    wait();

    // set up baud rate to BAUD_RATE
    out8(BAUD_RATE_LSB_REGISTER, (BAUD_RATE_VALUE & 0xff));
    wait();
    out8(BAUD_RATE_MSB_REGISTER, ((BAUD_RATE_VALUE >> 8) & 0xff));
    wait();

    // set up is done, so disable the port.
    out8(LINE_CONTROL_REGISTER, (BAUD_RATE_SETUP_DISABLE | DATA_8BIT | PARITY_NONE));
    wait();

    // disable loopback.
    // disable interrupts.
    // activate RTS pin.
    // activate DTR pin.
    out8(MODEM_CONTROL_REGISTER, 0x03);
    wait();
}

Uart::~Uart()
{
}

/*!
    \brief write char.

    \param c 1byte char.
    \author  Higepon
    \date    create:2008/12/13 update:
*/
void Uart::writeChar(char c)
{
    for (;;) {
        wait();
        if (in8(LINE_STATUS_REGISTER) & TRANSMIT_DATA_REGISTER_IS_EMPTY) break;
    }
    out8(TRANSMIT_DATA_REGISTER, c);
    wait();
}

/*!
    \brief read char.

    \return char.
    \author  Higepon
    \date    create:2008/12/13 update:
*/
int Uart::readChar()
{
    for (;;) {
        wait();
        if (in8(LINE_STATUS_REGISTER) & RECEIVED_DATA) break;
    }
    return in8(RECEIVE_DATA_REGISTER);
}

/*!
    \brief output 1byte value to Uart.

    \param reg register number.
    \param value 1byte value to write.
    \author  Higepon
    \date    create:2008/12/13 update:
*/
void Uart::out8(uint8_t reg, uint8_t value)
{
    outp8(device_ + reg, value);
}

/*!
    \brief input 1byte value from Uart.

    \param reg register number.
    \return 1byte input value.
    \author  Higepon
    \date    create:2008/12/13 update:
*/
uint8_t Uart::in8(uint8_t reg)
{
    return inp8(device_ + reg);
}

/*!
    \brief wait a few ms.

    \author  Higepon
    \date    create:2008/12/13 update:
*/
void Uart::wait()
{
    volatile int wait;
    for(wait = 0; wait < 10; wait++);
}
