#include <stdio.h>
#include <monapi/Message.h>
#include "servers/audio.h"
#include "audio_server.h"

int main(int argc, char *argv[])
{
	AudioServer server;
	MessageInfo msg;
	server = audio_server_new();
	printf("Audio server was started.\n");
	while(1)
	{
		if( MonAPI::Message::receive(&msg) ) continue;
		switch(msg.header)
		{
			case MSG_AUDIO_NEW_CHANNEL:
				audio_new_channel(server, &msg);
				break;
			case MSG_AUDIO_DELETE_CHANNEL:
				audio_delete_channel(server, &msg);
				break;
			case MSG_AUDIO_START:
				audio_start(server, &msg);
				break;
			case MSG_AUDIO_STOP:
				audio_stop(server, &msg);
				break;
			case MSG_AUDIO_SET_FORMAT:
				audio_set_format(server, &msg);
				break;
			default: break;
		}
	}
	return 0;
}
