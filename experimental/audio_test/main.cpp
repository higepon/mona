#include <monalibc.h>
#include <pci/Pci.h>
#include <monapi.h>
#include "ES1370.h"

int main(int argc, char *argv[])
{
	ES1370Driver *driver;
	FILE* fp;
	unsigned char *buf;

	syscall_get_io();

	driver = new ES1370Driver;

	if( !driver->existDevice() )
	{
		printf("Couldn't find device `ES1370`\n");
		delete driver;
		return 1;
	}

	driver->setIRQ();
	driver->readConf();
	driver->dumpRegisters();

/*
	fp = fopen("/APPS/TEST.RAW", "r");
	if( fp == NULL )
	{
		printf("Couldn't open file `/APPS/TEST.RAW`\n");
		delete driver;
		return 1;
	}
*/
	buf = (unsigned char*)malloc(44100);
	for(int i = 0 ; i < 44100 ; i++ )
	{
		if( i % 20 < 10 )
			buf[i] = 110;
		else
			buf[i] = -110;
	}


//	fread(buf, 1, 0xFFFF, fp);
	driver->playData(buf, 44100);

//	driver->MessageLoop();

	free(buf);
	delete driver;
	return 0;
}
