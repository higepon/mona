/*!
    \file  SystemInfo.h
    \brief class SystemInfo

    class SystemInfo

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/31 update:$Date$
*/
#ifndef _MONA_SYSTEMINFO_
#define _MONA_SYSTEMINFO_

#include<monaTypes.h>
#include<VirtualConsole.h>

#define INTEL          0
#define AMD            1
#define CYRIX          2
#define CENTAUR        3
#define TRANSMETA      4
#define UNKNOWN_VENDOR 5

/*!
    system information class
    single pattern applyes the instance of this class
*/
class SystemInfo {

  private:
    SystemInfo();
    ~SystemInfo();
    SystemInfo(const SystemInfo&);
    SystemInfo& operator = (const SystemInfo&);
    inline void cpuid(dword);
    char vendorId_[13];
    byte vendor_;
    dword eax_;
    dword ebx_;
    dword ecx_;
    dword edx_;
    dword supportedId_;
  public:
    static SystemInfo& instance();
    bool hasCpuid() const;
    void printCpuid(VirtualConsole* console);
};


#endif
