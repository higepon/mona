#pragma once

#include "audio_driver.h"
#include <monapi/Stream.h>

struct audio_server
{
	MonAPI::Stream *stream;
	struct audio_driver *driver;
	struct audio_data_format outputFormat;
	handle_t device;
	int channels;
	uint32_t tid;
};

typedef struct audio_server* AudioServer;


AudioServer audio_server_new();
int audio_render_callback(void *ref, void *buf, size_t size, size_t*);
int audio_stopped_callback(void *ref);
int audio_new_channel(AudioServer o, MessageInfo *msg);
int audio_delete_channel(AudioServer o, MessageInfo *msg);
int audio_start(AudioServer o, MessageInfo *msg);
int audio_stop(AudioServer o, MessageInfo *msg);
int audio_set_format(AudioServer o, MessageInfo *msg);
