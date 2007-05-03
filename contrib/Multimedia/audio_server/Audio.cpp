#include <monapi/Mutex.h>
#include <monalibc/stddef.h>
#include <monalibc/string.h>
#include <monalibc/limits.h>
#include <monapi/Message.h>
#include <monapi/io.h>
#include <monapi/Thread.h>
#include <stdio.h>
#include <vector>
#include <functional>
#include <algorithm>
#include <map>
#include "servers/audio.h"
#include "Audio.h"
#include "debug.h"

char *knownDevices[] = {"es1370"};
#define NumOfknownDevices ((int)(sizeof(knownDevices)/sizeof(knownDevices[0])))

extern "C" struct driver_desc *aud_es1370_get_desc();
extern "C" void aud_es1370_release_desc(struct driver_desc*);

MonAPI::Mutex *mutex;

Audio::Audio() : channelLength(0)
{
	drivers = new std::vector<struct driver_desc*>;
	drivers_hash = new std::map<char*, int>;
	commander = new ServerCommand(this);
	reservoir = new HList<void*>;
	dmabuf = monapi_allocate_dma_memory(0x10000);
	command_thread = new MonAPI::Thread(&ServerCommand::command_thread_main_loop, (void*)this, NULL);
	tid_ = syscall_get_tid();
	mutex = new MonAPI::Mutex;
	notifers = new std::list<IntNotifer*>;
}

Audio::~Audio()
{
	monapi_deallocate_dma_memory(dmabuf, 0x10000);
	delete reservoir;
	delete commander;
	delete drivers_hash;
	delete drivers;
	delete mutex;
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
	this->init_drivers();
	command_thread->start();
	return this->messageLoop();
}

uint32_t Audio::tid()
{
	return tid_;
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
	struct driver_desc* dd = *it;
	MessageInfo msg;
	while(1)
	{
		if( MonAPI::Message::receive(&msg) ) continue;
		dprintf("#Audio: msg.header = %x\n", msg.header);
		switch(msg.header)
		{
			case MSG_AUDIO_SERVER_COMMAND:
			{
				if( msg.arg1 > GetThreadID ) break;
				commander->caller(msg.arg1, &msg);
				break;
			}
			#if 1
			case MSG_INTERRUPTED:
			{
				dputs("#Audio: MSG_ITERRUPTED");
				mutex->lock();
				dd->emit_interrupted();
				std::for_each(notifers->begin(), notifers->end(), std::mem_fun(&IntNotifer::Interrupted));
				mutex->unlock();
				/*
				while( it != drivers->end() )
				{
					(*it)->emit_interrupted();
					it++;
				}
				*/
				break;
			}
			#endif
			default: break;
		}
	}
	return 0;
}
