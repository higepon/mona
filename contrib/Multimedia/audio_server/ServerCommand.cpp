#include <Audio.h>
#include <monapi/Message.h>
#include <monapi/cmemoryinfo.h>
#include <stdint.h>
#include <string.h>
#include "servers/audio.h"
#include "debug.h"

/* A version number is as the BCD. */
static uint32_t major_version = 0x00000000;
static uint32_t minor_version = 0x00000001;

#define NumOfMembers ((int)(sizeof(memberTable)/sizeof(memberTable[0])))

void ServerCommand::command_thread_main_loop(void* arg)
{
	Audio *parent = (Audio*)arg;
	MessageInfo msg;
	parent->tids[COMMAND_THREAD-1] = syscall_get_tid();
	dprintf("COMMAND_THREAD = %x\n", syscall_get_tid());
	while(1)
	{
		if( MonAPI::Message::receive(&msg) ) continue;
		switch(msg.header)
		{
			case MSG_AUDIO_SERVER_COMMAND:
			{
				if( msg.arg1 > NopCommand ) break;
				parent->commander->caller(msg.arg1, &msg);
				break;
			}
			default: break;
		}
	}
}

int (ServerCommand::*memberTable[])(MessageInfo*) = {
	&ServerCommand::GetThreadID,
	&ServerCommand::GetServerVersion,
	&ServerCommand::Nop,
	&ServerCommand::AllocateChannel,
	&ServerCommand::PrepareChannel,
	&ServerCommand::StartChannel,
	&ServerCommand::StopChannel,
	&ServerCommand::ReleaseChannel,
	&ServerCommand::Nop,
	&ServerCommand::Nop,
	&ServerCommand::SetBuffer,
	&ServerCommand::CreateDataStream,
	&ServerCommand::CreateChannelObject,
};

ServerCommand::ServerCommand(Audio *_parent) : parent(_parent)
{
}

ServerCommand::~ServerCommand()
{
}

int ServerCommand::Nop(MessageInfo *msg)
{
	return 0;
}

int ServerCommand::caller(int number, MessageInfo *msg)
{
	dprintf("number = %d\n", number);
	if( number >= NumOfMembers ) return -1;
	if( memberTable[number] == NULL ) return -1;
	return (this->*memberTable[number])(msg);
}

int ServerCommand::GetThreadID(MessageInfo *msg)
{
	switch(msg->arg2)
	{
		case NOTIFY:
		{
		/*
			parent->tids[msg->arg3-1] = msg->from;
			dprintf("ThreadID: %x:%x\n", msg->arg3, msg->from);
			MonAPI::Message::reply(msg, 0);
			*/
			break;
		}
		case COMMAND_THREAD:
			MonAPI::Message::reply(msg, parent->tids[COMMAND_THREAD-1]);
			break;
		default: break;
	}
	return 0;
}

int ServerCommand::GetServerVersion(MessageInfo *msg)
{
	/* arg2 = major_version, arg3 = minor_version */
	MonAPI::Message::reply(msg, major_version, minor_version);
	return 0;
}

int ServerCommand::AllocateChannel(MessageInfo *msg)
{
	ch_t ch;
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	ch = (*it)->create_channel();
	MonAPI::Message::reply(msg, ch);
	return 0;
}

int ServerCommand::PrepareChannel(MessageInfo *msg)
{
	ch_t ch;
	int ret = 0;
	struct audio_server_channel_info ci;
	memcpy(&ci, msg->str, sizeof(ci));
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	ret = (*it)->prepare_channel((ch_t)ci.channel, ci.samplerate, ci.bitspersample, 1);
	MonAPI::Message::reply(msg, ret);
	return ret;
}

int ServerCommand::ReleaseChannel(MessageInfo *msg)
{
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	(*it)->destroy_channel(msg->arg2);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

/*
 * arg1 is an number of a channel.
 * str contains data of struct audio_server_buffer_info.
 */
int ServerCommand::SetBuffer(MessageInfo *msg)
{
	struct audio_server_buffer_info bufinfo;
	monapi_cmemoryinfo *mi;
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	dprintf("msg->str = %x\n", msg->str);
	dprintf("dmabuf = %x\n", parent->dmabuf);
	memcpy(&bufinfo, msg->str, sizeof(bufinfo));

	mi = monapi_cmemoryinfo_new();
	mi->Handle = bufinfo.handle;
	mi->Size = bufinfo.size;
	monapi_cmemoryinfo_map(mi);

	memcpy(parent->dmabuf, (void*)mi->Data, bufinfo.size);
	(*it)->set_buffer(msg->arg2, parent->dmabuf, bufinfo.size);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

int ServerCommand::StartChannel(MessageInfo *msg)
{
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	(*it)->start_channel(msg->arg2, 1);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

int ServerCommand::StopChannel(MessageInfo *msg)
{
	std::vector<struct driver_desc*>::iterator it;
	it = parent->drivers->begin();
	(*it)->stop_channel(msg->arg2);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

int ServerCommand::CreateDataStream(MessageInfo *msg)
{
	ch_t ch = msg->arg2;
	MonAPI::Message::reply(msg, 0);
	return 0;
}

int ServerCommand::CreateChannelObject(MessageInfo *msg)
{
	Channel **channels = parent->channels;
	Channel **p;
	size_t channelLength = parent->channelLength;
	int handle = 0;
	for( size_t i = 0 ; i < channelLength ; i++ )
	{
		if( channels[i] == NULL )
		{
			channels[i] = new Channel;
			handle = i+1;
		}
	}
	if( handle == 0 )
	{
		p = (Channel**)realloc((void*)channels, ++channelLength);
		if( p == NULL )
		{
			MonAPI::Message::reply(msg, 0);
			return -1;
		}
		channels = p;
		channels[channelLength-1] = new Channel;
	}
	MonAPI::Message::reply(msg, handle);
	return 0;
}

int ServerCommand::BindChannelObject(MessageInfo *msg)
{
	char *device;
	int handle;
	int driver_index;
	struct driver_desc *driver;

	device = msg->str;
	handle = msg->arg2;

	std::map<char*, int>::iterator it = parent->drivers_hash->find(device);
	driver_index = (*it).second;
	std::vector<struct driver_desc*>::iterator it2 = parent->drivers->begin();
	for( int i = 0 ; i < driver_index ; i++ ) it2++;
	driver = (*it2);
	parent->channels[handle-1]->init(driver);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

