/*!
    \file  SystemInfo.cpp
    \brief class SystemInfo

    class SystemInfo

    Copyright (c) 2002 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2002/12/31 update:$Date$
*/
#include<SystemInfo.h>
#include<monaVga.h>
#include<string.h>

/*!
    \brief destructor

    destructor

    \author HigePon
    \date   create:2002/12/31 update:
*/
SystemInfo::~SystemInfo() {
}

/*!
    \brief private constructor

    constructor

    \author HigePon
    \date   create:2002/12/31 update:
*/
SystemInfo::SystemInfo() {
}

/*!
    \brief check has cpuid

    check has cpuid

    \return true:has cpuid
    \author HigePon
    \date   create:2002/12/31 update:
*/
bool SystemInfo::hasCpuid() const {

    dword result = 0;

    asm volatile("pushf                \n"
                 "pop %%eax            \n"
                 "mov %%eax    , %%ecx \n"
                 "xor $0x200000, %%eax \n"
                 "push %%eax           \n"
                 "popf                 \n"
                 "pushf                \n"
                 "pop %%eax            \n"
                 "xor %%ecx, %%eax     \n"
                 "mov %%eax, %0        \n"
                 : "=m" (result) : /* no input */ : "ax", "cx");

    return result;
}

/*!
    \brief cpuid

    cpuid get cpu information

    \return true:has cpuid
    \author HigePon
    \date   create:2003/01/01 update:
*/
void SystemInfo::cpuid(dword id) {

    asm volatile("mov %4   , %%eax \n"
                 "cpuid            \n"
                 "mov %%ebx, %0    \n"
                 "mov %%ecx, %1    \n"
                 "mov %%edx, %2    \n"
                 "mov %%eax, %3    \n"
                 : "=m" (ebx_), "=m" (ecx_), "=m" (edx_), "=m" (eax_)
                 : "m"  (id) : "ax", "bx", "cx", "dx");
}

/*!
    \brief cpuid

    cpuid get cpu information

    \return true:has cpuid
    \author HigePon
    \date   create:2002/12/31 update:
*/
void SystemInfo::cpuid() {

    cpuid(0);

    vendorId_[0] = ebx_ & 0xff;
    vendorId_[1] = (ebx_ >> 8) & 0xff;
    vendorId_[2] = (ebx_ >> 16) & 0xff;
    vendorId_[3] = (ebx_ >> 24) & 0xff;
    vendorId_[4] = edx_ & 0xff;
    vendorId_[5] = (edx_ >> 8) & 0xff;
    vendorId_[6] = (edx_ >> 16) & 0xff;
    vendorId_[7] = (edx_ >> 24) & 0xff;
    vendorId_[8] = ecx_ & 0xff;
    vendorId_[9] = (ecx_ >> 8) & 0xff;
    vendorId_[10] = (ecx_ >> 16) & 0xff;
    vendorId_[11] = (ecx_ >> 24) & 0xff;
    vendorId_[12] = '\0';
    supportedId_ = eax_;

    if (!strcmp(vendorId_, "GenuineIntel"))      vendor_ = INTEL;
    else if (!strcmp(vendorId_, "AuthenticAMD")) vendor_ = AMD;
    else if (!strcmp(vendorId_, "CyrixInstead")) vendor_ = CYRIX;
    else if (!strcmp(vendorId_, "CentaurHauls")) vendor_ = CENTAUR;
    else if (!strcmp(vendorId_, "GenuineTMx86")) vendor_ = TRANSMETA;
    else                                         vendor_ = UNKNOWN_VENDOR;

    _sys_printf("vendor id = %s\n", vendorId_);
}

/*!
    \brief get the instance of this class

    get the instance of this class
    this class has no public constructor.

    \author HigePon
    \date   create:2002/12/31 update:
*/
SystemInfo& SystemInfo::instance() {
    static SystemInfo theInstance;
    return theInstance;
}
