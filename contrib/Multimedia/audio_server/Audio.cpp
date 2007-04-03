#include <monalibc/stddef.h>
#include <monalibc/string.h>
#include <monapi/Message.h>
#include <monapi/io.h>
#include <stdio.h>
#include <vector>
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
	drivers = new std::vector<struct driver_desc*>;
	drivers_hash = new std::map<char*, int>;
	commander = new ServerCommand(this);
//	stream = new Stream;
	dmabuf = monapi_allocate_dma_memory(0x10000);
}

Audio::~Audio()
{
	monapi_deallocate_dma_memory(dmabuf, 0x10000);
//	delete stream;
	delete commander;
	delete drivers_hash;
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
	this->init_drivers();
	return this->messageLoop();
}

bool Audio::init_drivers()
{
	std::vector<struct driver_desc*>::iterator it = drivers->begin();
	while( it != drivers->end() )
	{
		(*it)->init_driver(makeID());
		(*it)->init_device();
		it++;
	}
	return true;
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
		drivers->push_back(desc);
		drivers_hash->insert(
			std::map<char*, int>::value_type(devices[i], drivers->size()-1));
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
	std::vector<struct driver_desc*>::iterator it = drivers->begin();
	MessageInfo msg;
	while(1)
	{
		if( MonAPI::Message::receive(&msg) ) continue;
		dprintf("#Audio: msg.header = %x\n", msg.header);
		switch(msg.header)
		{
			case MSG_AUDIO_SERVER_COMMAND:
			{
				if( msg.arg1 > NopCommand ) break;
				commander->caller(msg.arg1, &msg);
				break;
			}
			case MSG_INTERRUPTED:
			{
				dputs("#Aduio: MSG_ITERRUPTED");
				while( it != drivers->end() )
				{
					(*it)->emit_interrupted();
					it++;
				}
				break;
			}
			default: break;
		}
	}
	return 0;
}
