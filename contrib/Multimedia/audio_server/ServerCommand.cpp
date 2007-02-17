#include <Audio.h>
#include <monapi/Message.h>
#include <stdint.h>
#include "debug.h"

/* A version number is as the BCD. */
static uint32_t major_version = 0x00000000;
static uint32_t minor_version = 0x00000001;

#define NumOfMembers ((int)(sizeof(memberTable)/sizeof(memberTable[0])))

int (*memberTable[])(Audio*, MessageInfo*) = {
	NULL,
	ServerCommand::GetServerVersion,
};

int ServerCommand::caller(int number, Audio* audio, MessageInfo *msg)
{
	if( number >= NumOfMembers ) return -1;
	if( memberTable[number] == NULL ) return -1;
	return (*memberTable[number])(audio, msg);
}

int ServerCommand::GetServerVersion(Audio* audio, MessageInfo *msg)
{
	/* arg2 = major_version, arg3 = minor_version */
	MonAPI::Message::reply(msg, major_version, minor_version);
	return 0;
}
