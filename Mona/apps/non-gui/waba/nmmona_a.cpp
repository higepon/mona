/*
Copyright (C) 1998, 1999, 2000 Wabasoft

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.
*/

/*

This file is the starting point when porting the WabaVM to a new platform.

To port the WabaVM, you need to fill in platform dependent code in 3 files:

- nm<platform>_a.c
- nm<platform>_b.c
- nm<platform>_c.c

The nm stands for 'native methods'. The 3 files contain:

- code to deal with platform-specific type sizes and byte order
- code to allocate a few blocks of memory
- code to load a class from memory or disk
- the main application loop
- native functions

This file contains the code to deal with platform-specific types and
allocation of blocks of memory. To port the VM, read though the comments
and look for the 'WHEN PORTING' comments and fill in what you need to.

You'll probably want to take a look at waba.c as you go along to see
where the code is used. A WabaVM is comprised of the 3 native
function files and the VM code itself contained in waba.c

*/

// WHEN PORTING: define FREE_ON_EXIT if you want all object destructors
// to be called when the program exits. Under OS's that release system
// resources automatically, you don't need to do this. On OS's that don't
// automatically release system resources (images, memory, etc.) that
// native functions may allocate, you should set this to 1 so when
// the waba program exits, all native method object destructors get called
// to clean things up
#define FREE_ON_EXIT 1

// WHEN PORTING: You need to define the basic types of uchar, int32, etc.
// for the platform being ported to as follows:
//
// - uchar is an 8 bit unsigned value
// - int32 is a signed 32 bit value
// - uint32 is an unsigned 32 bit value
// - float32 is a signed 32 bit floating point value
// - int16 is a signed 16 bit value
// - uint16 is an unsigned 16 bit value
//
// below is an example from PalmOS

#undef FREE_ON_EXIT
#undef SECURE_CLASS_HEAP
#define LOCK_CLASS_HEAP
#define UNLOCK_CLASS_HEAP

#define uchar unsigned char
#define int32 long
#define uint32 unsigned long
#define float32 float
#define int16 short
#define uint16 unsigned short

#define WABA_VERSION "0.0.9"
#define VM_OS        "MONA"
#define VM_USER      "MONA"
#define MAX_CLASS       64
#define g_mainWinOffX   176
#define g_mainWinOffY   250
#define g_mainWinWidth  200
#define g_mainWinHeight 200
#define DRAW_OVER 1
#define DRAW_AND  2
#define DRAW_OR   3
#define DRAW_XOR  4

// WHEN PORTING: You need to define functions that convert a string
// of 2 or 4 bytes in network byte order to a 16 or 32 bit value as follows:
//
// getUInt32() - converts 4 bytes to a unsigned 32 bit integer
// getUInt16() - converts 2 bytes to a unsigned 16 bit integer
// getInt32() - converts 4 bytes to a signed 32 bit integer
// getInt16() - converts 2 bytes to a signed 16 bit integer
//
// Here is an example from PalmOS, you can make these functions instead of
// a macro if you want the executable to be smaller

#define getUInt32(b) (uint32)( (uint32)((b)[0])<<24 | (uint32)((b)[1])<<16 | (uint32)((b)[2])<<8 | (uint32)((b)[3]) )
#define getUInt16(b) (uint16)(((b)[0]<<8)|(b)[1])
#define getInt32(b) (int32)( (uint32)((b)[0])<<24 | (uint32)((b)[1])<<16 | (uint32)((b)[2])<<8 | (uint32)((b)[3]) )
#define getInt16(b) (int16)(((b)[0]<<8)|(b)[1])

// WHEN PORTING: You need to define a function that converts 4 bytes in
// network byte order to a 32 bit floating point value
//
// Here is an example from PalmOS:

/* static */ float32 getFloat32(uchar *buf)
    {
    uint32 i;
    float32 f;

    // we need to make sure we're aligned before casting
    i = ((uint32)buf[0] << 24) | ((uint32)buf[1] << 16) | ((uint32)buf[2] << 8) | (uint32)buf[3];
    f = *((float32 *)&i);
    return f;
    }

/* static */ void *calloc(int size) {
char *p = (char*)malloc(size);
    if(p){
        memset(p, 0, size);
        return (void *)p; 
    }else{
        //mallocŽ¸”s
        exit(1);
        return NULL;
    }
    }

// WHEN PORTING: You need to define the following functions:
//
// xstrncmp(s1, s2, n) - compares 2 strings n bytes up to a 0 terminator
// xstrncpy(s1, s2, n) - copies 2 strings n bytes up to a 0 terminator
// xstrlen(s) - returns the length of a string
// xstrcat(dst, src) - concats one string to another
// xmemmove(dst, src, size) - copies size bytes from src to dst
// xmemzero(mem, len) - zeros out len bytes at location mem
//
// You could find the native functions that map to the above functions
// or (easier) just use the ones below. In any case, its probably best
// to use the ones below first and then you switch out after things
// are running in case the machines functions don't do exactly what
// is expected.

/* static */ int xstrncmp(char *s1, char *s2, uint32 n)
    {
    uint32 i;

    for (i = 0; i < n; i++)
        if (s1[i] != s2[i])
            return 1;
    return 0;
    }

/* static */ void xstrncpy(char *dst, char *src, uint32 n)
    {
    uint32 i;

    for (i = 0; i < n; i++)
        dst[i] = src[i];
    }

/* static */ uint32 xstrlen(char *s)
    {
    uint32 len;

    len = 0;
    while (*s != 0)
        {
        len++;
        s++;
        }
    return len;
    }

/* static */ void xstrcat(char *dst, char *src)
    {
    while (*dst != 0)
        dst++;
    while (*src != 0)
        *dst++ = *src++;
    *dst = 0;
    }

/* static */ void xmemmove(uchar *dst, uchar *src, uint32 size)
    {
    uint32 i;

    for (i = 0; i < size; i++)
        dst[i] = src[i];
    }

/* static */ void xmemzero(uchar *mem, uint32 len)
    {
    uint32 i;

    for (i = 0; i < len; i++)
        mem[i] = 0;
    }

// WHEN PORTING: You need to define a function that allocates a block
// of memory and a function that frees a block of memory. The WabaVM
// allocates 4 blocks of memory when it starts up and these methods
// are only used to allocate and free those 4 blocks of memory. When
// the VM is running, it does not call these functions. So, if you have
// a processor without any heap management, it should be rather easy
// to get the VM running. Just allocate 4 blocks of memory for it and
// return pointers to them below. You don't need a heap based malloc()
// implementation or anything to run with the WabaVM. The VM does its
// own memory management within the 4 blocks.
//
// The two functions you need to implement are:
//
// xmalloc(size) - allocates size bytes of memory and returns a pointer
// xfree(ptr) - frees the memory allocated previously
//
// On platforms that have malloc() and free, we can simply use them as
// follows:

#define xmalloc(size) calloc(size);

#define xfree(ptr) free(ptr)

// WHEN PORTING: That's it for the types and memory allocation, the next
// step is to implement the class loader and native functions in the
// nm<platform>_b.c and nm<platform>_c.c files
