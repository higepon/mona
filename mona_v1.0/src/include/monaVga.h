/*!
    \file  monaVga.h
    \brief definition for VGA

    definition for VGA
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/20 update:$Date$
*/
#ifndef _MONA_VGA_
#define _MONA_VGA_

#include<monaTypes.h>

#define VRAM ((char*)0xB8000)                          /*! VRAM Address */
#define MAX_HEIGHT 25                                  /*! MAX_HEIGHT   */
#define MAX_WIDTH  80                                  /*! MAX_WIDTH    */
#define VRAM_SIZE (MAX_HEIGHT * MAX_WIDTH + MAX_WIDTH) /*! VRAMSIZE     */

#define CH_WHITE   0x0F
#define CH_YELLOW  0x0E
#define CH_FUCHSIA 0x0D
#define CH_RED     0x0C
#define CH_AQUA    0x0B
#define CH_LIME    0x0A
#define CH_BLUE    0x09
#define CH_GRAY    0x08
#define CH_SILVER  0x07
#define CH_ORANGE  0x06
#define CH_PURPLE  0x05
#define CH_MAROON  0x04
#define CH_TEAL    0x03
#define CH_GREEN   0x02
#define CH_NAVY    0x01
#define CH_BLACK   0x00
#define BG_WHITE   0xF0
#define BG_YELLOW  0xE0
#define BG_FUBGSIA 0xD0
#define BG_RED     0xC0
#define BG_AQUA    0xB0
#define BG_LIME    0xA0
#define BG_BLUE    0x90
#define BG_GRAY    0x80
#define BG_SILVER  0x70
#define BG_ORANGE  0x60
#define BG_PURPLE  0x50
#define BG_MAROON  0x40
#define BG_TEAL    0x30
#define BG_GREEN   0x20
#define BG_NAVY    0x10
#define BG_BLACK   0x00

void _sysInitVga(void);
void _sysClearScreen();
void _sysWriteVram(int, int, char);
void _sysPutCharacter(char);
void _sysPrint(char*);
void _sysPrintInt(int);
void _sysPrintln(char*);
void _sysPrintlnInt(int);
void _sys_printf (char *, ...);
void newLine(void);
void forwardCursor(void);
void forwardCursor(int);
void backwardCursor(void);
void backwardCursor(int);
void setCursor(int, int);
void scrollUp();
void _sysPutInt(size_t, int);
void _sysSetColor(char);
#endif
