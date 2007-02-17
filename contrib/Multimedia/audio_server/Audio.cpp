#include <monalibc/stddef.h>
#include <monalibc/string.h>
#include <monapi/Message.h>
#include <stdio.h>
#include <map>
#include "servers/audio.h"
#include "Audio.h"
#include "debug.h"

char *knownDevices[] = {"es1370"};
#define NumOfknownDevices ((int)(sizeof(knownDevices)/sizeof(knownDevices[0])))

extern "C" struct driver_desc *aud_es1370_get_desc();
extern "C" void aud_es1370_release_desc(struct driver_desc*);

Audio::Audio()
{
	drivers = new std::map<char*, struct driver_desc*>;
}

Audio::~Audio()
{
	delete drivers;
}

bool Audio::init(char *devices[], int devnum)
{
	if( devices == NULL )
	{
		return false;
	}
	if( !findDevices(devices, devnum) ) return false;
	return true;
}

int Audio::run()
{
	return this->messageLoop();
}

bool Audio::findDevices(char *devices[], int devnum)
{
	struct driver_desc *desc = NULL;
	bool foundAnything = false;
	for( int i = 0 ; i < devnum ; i++ )
	{
		desc = this->findDriver(devices[i]);
		if( desc == NULL ) continue;
		foundAnything = true;
		drivers->insert(
			std::map<char*, struct driver_desc*>::value_type(devices[i], desc));
	}
	if( !foundAnything ) return false;
	return true;
}

struct driver_desc *Audio::findDriver(const char* name)
{
	struct driver_desc *ret;
	for( int i = 0 ; i < NumOfknownDevices ; i++ )
	{
		if( !strcmp(name, knownDevices[i]) ) goto foundDevice;
	}
	return NULL;
foundDevice:
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

int Audio::messageLoop()
{
	for( MessageInfo msg ;; )
	{
		if( MonAPI::Message::receive(&msg) ) continue;
		switch(msg.header)
		{
			case MSG_AUDIO_SERVER_COMMAND:
			{
				if( msg.arg1 > NopCommand ) break;
				ServerCommand::caller(msg.arg1, this, &msg);
				break;
			}
			default: break;
		}
	}
	return 0;
}
