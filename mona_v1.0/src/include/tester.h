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

#include <types.h>
#include <test_mjt.h>
#include <Process.h>

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
void keyStrokeTest();
int loadProcess(const char* path, const char* file, bool isUser);
int send(const char* name, Message* message);
int receive(Process* process, Message* message);
#endif
