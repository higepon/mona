/*!
    \file  addressmap.h
    \brief AddressMap

    Copyright (c) 2008 Yume
    WITHOUT ANY WARRANTY

    \author  Yume
    \version $Revision: 2700 $
    \date   create:2004/01/17 update:$Date: 2005-07-17 22:27:24 +0900 (日, 17  7 2005) $
*/

#ifndef _MONA_ADDRESSMAP_
#define _MONA_ADDRESSMAP_

#include <sys/types.h>

#define ADDRESSMAP_INFO_ADDR 0x930

struct memorymap
{
	uint32_t BaseAddrLow;
	uint32_t BaseAddrHigh;
	uint32_t LengthLow;
	uint32_t LengthHigh;
	uint32_t Type;
};

void dumpAddressMap();

#endif
