#include "global.h"
#include "addressmap.h"

#define output g_log

void dumpAddressMap()
{
	void *p = (void*)ADDRESSMAP_INFO_ADDR;
	struct memorymap map;
	char *addr = (char*)p;
	unsigned short *q = (unsigned short*)p;
	int n = (int)*q;

	addr += 20;
	output->printf("Addressmap\n");
	output->printf("n = %d\n", n);
	for( unsigned int i = 0 ;
		i < (n / sizeof(map)) ;
		i++,addr += sizeof(map) )
	{
		memcpy(&map, addr, sizeof(map));
		output->printf("%x : ", map.BaseAddrLow);
		output->printf("%x : ", map.LengthLow);
		output->printf("%x\n", map.Type);
	}
}

