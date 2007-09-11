#pragma once

#include <stdint.h>
#include "servers/audio.h"
#include "audio_driver.h"
#include "circular_buffer.h"
#include <monapi/Stream.h>
#include <monapi/Thread.h>
#include <stlport/list>

class Mixer
{
public:
	Mixer();
	~Mixer();
private:
};

class Channel
{
public:
	Channel(int direction);
	~Channel();
	int setFormat(struct audio_data_format *format);
	int getFormat(struct audio_data_format *format);
	int setStream(uint32_t handle);
	MonAPI::Stream *getStream();
	int setVolume(int volume);
	int getVolume();
private:
	int direction_;
	struct audio_channel_description desc_;
	MonAPI::Stream* stream_;
};

void mixer_th(void *arg);

class AudioServer
{
public:
	AudioServer();
	~AudioServer();

	int createChannel(int direction);
	int destroyChannel(int channel);
	int setFormat(int channel, struct audio_data_format *format);
	int getFormat(int channel, struct audio_data_format *format);
	int setStream(int channel, uint32_t handle);
	int setBlockSize(int channel, int blocksize);
	int getBlockSize(int channel);
	int start(int channel);
	int stop(int channel);
	int setVolume(int channel, int volume);
	int getVolume(int channel);
	int wait(int channel);
//private:
	struct audio_driver *driver_;
	handle_t device_;
	Channel *channel_;
	size_t blocksize_;
	MonAPI::Thread* mixer_;
	bool playing;
};

#if 0
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
#endif


#if 0
AudioServer audio_server_new();
int audio_render_callback(void *ref, void *buf, size_t size, size_t*);
int audio_stopped_callback(void *ref);
int audio_new_channel(AudioServer o, MessageInfo *msg);
int audio_delete_channel(AudioServer o, MessageInfo *msg);
int audio_start(AudioServer o, MessageInfo *msg);
int audio_stop(AudioServer o, MessageInfo *msg);
int audio_set_format(AudioServer o, MessageInfo *msg);
#endif
