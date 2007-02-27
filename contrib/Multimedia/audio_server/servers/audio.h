#ifndef _SERVER_AUDIO_H_
#define _SERVER_AUDIO_H_

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

#endif

