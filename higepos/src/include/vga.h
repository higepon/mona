/*!
    \file  vga.h
    \brief definition for VGA

    definition for VGA
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/07/20 update:$Date$
*/
#ifndef _HIGEPOS_VGA_
#define _HIGEPOS_VGA_

#define VRAM ((char*)0xB8000)                 /*! VRAM Address */
#define MAX_HEIGHT 25                         /*! MAX_HEIGHT   */
#define MAX_WIDTH  80                         /*! MAX_WIDTH    */
#define VRAMSIZE (MAX_HEIGHT * MAX_WIDTH * 2) /*! VRAMSIZE     */

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

#endif
