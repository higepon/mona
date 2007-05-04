#include <monapi/Mutex.h>
#include <monalibc/stddef.h>
#include <monalibc/string.h>
#include <monalibc/limits.h>
#include <monapi/Message.h>
#include <monapi/io.h>
#include <monapi/Thread.h>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include "servers/audio.h"
#include "Audio.h"
#include "debug.h"

char *knownDevices[] = {"es1370"};
#define NumOfknownDevices ((int)(sizeof(knownDevices)/sizeof(knownDevices[0])))

extern "C" struct driver_desc *aud_es1370_get_desc();
extern "C" void aud_es1370_release_desc(struct driver_desc*);

Audio::Audio() : channelLength(0)
{
	commander = new ServerCommand(this);
	dmabuf = monapi_allocate_dma_memory(0x10000);
	tid_ = syscall_get_tid();
	notifers = new std::list<Notifer*>;
}

Audio::~Audio()
{
	monapi_deallocate_dma_memory(dmabuf, 0x10000);
	delete commander;
	delete notifers;
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
	this->init_driver();
	return this->messageLoop();
}

uint32_t Audio::tid()
{
	return tid_;
}

bool Audio::init_driver()
{
	driver->init_driver(makeID());
	driver->init_device();
	commander->driver = this->driver;
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
		driver = desc;
		break;
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
	MessageInfo msg;
	while(1)
	{
		if( MonAPI::Message::receive(&msg) ) continue;
		dprintf("#Audio: msg.header = %x\n", msg.header);
		switch(msg.header)
		{
			case MSG_AUDIO_SERVER_COMMAND:
			{
				commander->caller(msg.arg1, &msg);
				break;
			}
			case MSG_INTERRUPTED:
			{
				dputs("#Audio: MSG_ITERRUPTED");
				driver->emit_interrupted();
				std::for_each(notifers->begin(), notifers->end(), std::mem_fun(&Notifer::Interrupted));
				break;
			}
			default: break;
		}
	}
	return 0;
}
