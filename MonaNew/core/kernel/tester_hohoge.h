/*!
    \file  tester.h
    \brief definition for tester

    definition for tester
    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/11/11 update:$Date$
*/

#ifndef _MONA_TESTER_
#define _MONA_TESTER_

#include <sys/types.h>
#include <sys/error.h>
//#include "test_mjt.h"
#include "Process.h"
#include "KObject.h"

void hvectorTester();
void operatorTester();
void typeTester();
void processTester();
void process1Tester();
void process2Tester();
void process3Tester();
void STLTester();
void FDCTester();
void ELFTester(byte* out);
void FDCDriverTester();
void mmChangeTester();
void testFDWrite();
void keyStrokeTest();


/*----------------------------------------------------------------------
    KTimer
----------------------------------------------------------------------*/

/* typedef struct { */
/*     char* dir; */
/*     char* file; */
/*     byte* buffer; */
/*     dword size; */
/*     dword error; */
/* } IOStream; */

/* bool readFile(IOStream* io); */
/* bool writeFile(IOStream* io); */
/* bool writeFileAppend(IOStream* io); */

/* #define IO_ERROR_OPEN  1 */
/* #define IO_ERROR_CD    2 */
/* #define IO_ERROR_ALLOC 3 */
/* #define IO_ERROR_READ  4 */
/* #define IO_ERROR_CLOSE 5 */
/* #define IO_ERROR_WRITE 6 */

/* class Logger { */

/*   public: */
/*     Logger(char* dir, char* file); */
/*     virtual ~Logger(); */

/*   public: */
/*     void write(char ch); */
/*     void flush(); */

/*     static const char CR    = 0x0D; */
/*     static const char LF    = 0x0A; */

/*   private: */
/*     void writeBuf(char ch); */

/*   private: */
/*     char* dir_; */
/*     char* file_; */
/*     char buf_[1024]; */
/*     int pos_; */
/*     int prevpos_; */
/* }; */


#endif
