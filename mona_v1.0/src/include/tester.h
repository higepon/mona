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
#include <IStorageDevice.h>
#include <error.h>

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

class RTC {

  public:
    static void init();
    static void getDate(KDate* date);

  private:
    static byte read(byte reg);
    static void write(byte reg, byte value);
    static int readDateOnce(KDate* date);
    inline static int convert(byte value) {
        return (value & 0x0f) + ( (value >> 4) * 10);
    }

  public:
    static const byte RTC_ADRS  = 0x70;
    static const byte RTC_DATA  = 0x71;
    static const byte RTC_SEC   = 0x00;
    static const byte RTC_MIN   = 0x02;
    static const byte RTC_HOUR  = 0x04;
    static const byte RTC_DOW   = 0x06;
    static const byte RTC_DAY   = 0x07;
    static const byte RTC_MONTH = 0x08;
    static const byte RTC_YEAR  = 0x09;
};

class Mouse {

  public:
    static int init();
    static int waitWritable();
    static int waitReadable();

  public:
    static const int MOUSE_TIMEOUT = 50000;
};

typedef struct {
    char* dir;
    char* file;
    byte* buffer;
    dword size;
    dword error;
} IOStream;

bool readFile(IOStream* io);
bool writeFile(IOStream* io);
bool writeFileAppend(IOStream* io);

#define IO_ERROR_OPEN  1
#define IO_ERROR_CD    2
#define IO_ERROR_ALLOC 3
#define IO_ERROR_READ  4
#define IO_ERROR_CLOSE 5
#define IO_ERROR_WRITE 6

class Logger {

  public:
    Logger(char* dir, char* file);
    virtual ~Logger();

  public:
    void write(char ch);
    void flush();

    static const char CR    = 0x0D;
    static const char LF    = 0x0A;

  private:
    void writeBuf(char ch);

  private:
    char* dir_;
    char* file_;
    char buf_[1024];
    int pos_;
    int prevpos_;
};

class Messenger {

  public:
    Messenger::Messenger(int size);
    virtual ~Messenger();

  public:
    int send(const char* name, MessageInfo* message);
    int send(dword pid, MessageInfo* message);
    int receive(Thread* thread, MessageInfo* message);

  private:
    MessageInfo* allocateMessageInfo();

  private:
    int size_;
    int allocated_;
    MessageInfo* info_;
};

#endif
