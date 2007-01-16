#include <monalibc/stddef.h>
#include <monalibc/string.h>
#include "Audio.h"

char *knownDevices[] = {"auto", "es1370"};
#define NumOfknownDevices ((int)(sizeof(knownDevices)/sizeof(knownDevices[0])))

extern "C" struct driver_desc *aud_es1370_get_desc();

Audio::Audio()
{
}

Audio::~Audio()
{
}

bool Audio::init(char *devices[])
{
	if( devices == NULL )
	{
		findDevices();
	}
	return false;
}

bool Audio::findDevices()
{
	return false;
}

struct driver_desc *Audio::findDriver(const char* name)
{
	struct driver_desc *ret;
	for( int i = 0 ; i < NumOfknownDevices ; i++ )
	{
		if( !strcmp(name, knownDevices[i]) ) goto deviceFound;
	}
	return NULL;
deviceFound:
	if( !strcmp(name, "es1370") )
	{
		ret = aud_es1370_get_desc();
	}
	else
	{
		return NULL;
	}
	return ret;
}
