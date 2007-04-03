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

int (ServerCommand::*memberTable[])(MessageInfo*) = {
	&ServerCommand::Nop,
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
	if( number >= NumOfMembers ) return -1;
	if( memberTable[number] == NULL ) return -1;
	return (this->*memberTable[number])(msg);
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

