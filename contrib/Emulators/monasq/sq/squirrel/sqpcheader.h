/*	see copyright notice in squirrel.h */
#ifndef _SQPCHEADER_H_
#define _SQPCHEADER_H_

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif 

#if defined( MONASQ )
#include <monapi.h>
#include <monalibc/assert.h>
//#include <monalibc/setjmp.h>
#include "misc/setjmp.h"

// default placement new.
//inline void *operator new( size_t size, void *buf ) { return buf; }
//inline void operator delete( void *p, void * ) {}

#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <new>
#endif

//squirrel stuff
#include <squirrel.h>
#include "sqobject.h"
#include "sqstate.h"


#endif //_SQPCHEADER_H_
