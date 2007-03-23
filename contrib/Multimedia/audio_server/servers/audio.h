#ifndef _SERVER_AUDIO_H_
#define _SERVER_AUDIO_H_

#include <stdint.h>

enum
{
	MSG_AUDIO_SERVER_COMMAND	= 0x50728d6,
	MSG_AUDIO_SERVER_MESSAGE	= 0x506c91a,
	MSG_AUDIO_RETURN_NUMBER		= 0x507124e,
};

enum AudioServerCommand
{
	GetCommandInfo = 0,
	GetServerVersion,
	GetServerStatus,
	AllocateChannel,
	PrepareChannel,
	StartChannel,
	StopChannel,
	ReleaseChannel,
	DevicesInfo,
	ChannelInfo,
	SetBuffer,
	NopCommand,
};

enum AudioServerMesssage
{
	Non = 0,
	BufferIsEmpty,
	MessageContinues,
	NopMessage,
};

enum AudioServerReturnNumber
{
	Unknown = 0,
	CannotAllocateChannel,
	NopReturn,
};

struct audio_server_channel_info
{
	int32_t channel;
	int32_t samplerate;
	int32_t bitspersample;
	int32_t channels;
};

struct audio_server_buffer_info
{
	int32_t channel;
	void *pointer;		/* A pointer of the buffer. */
	size_t size;		/* A size of the buffer, */
	int32_t isDMA;		/* This is used as a boolean. 1=TRUE */
};

#endif

