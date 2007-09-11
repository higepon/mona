#include "audio_server.h"
#include "audio_driver.h"
#include "servers/audio.h"
#include "monapi.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <monapi/Stream.h>
#include <monapi/Message.h>
#include <monapi/cmemoryinfo.h>
#include <assert.h>

struct audio_data_format default_format =
{
	0,
	2,
	16,
	44100
};
void stop_cb(void *arg);

AudioServer::AudioServer() : channel_(NULL)
{
	driver_ = audio_driver_factory("es1370");
	device_ = driver_->driver_new();
	dprintf("device_ = %x\n", device_);
	dprintf("this = %x\n", this);
	driver_->driver_set_format(device_, &default_format);
	blocksize_ = driver_->driver_get_block_size(device_);
	mixer_ = new MonAPI::Thread(mixer_th, this, stop_cb);
	mixer_->start();
}

AudioServer::~AudioServer()
{
	driver_->driver_delete(device_);
}

int AudioServer::createChannel(int direction)
{
	if( channel_ != NULL ) return -1;
	channel_ = new Channel(direction);
	return 0;
}

int AudioServer::destroyChannel(int channel)
{
	if( channel != 0 ) return -1;
	delete channel_;
	channel_ = NULL;
	return 0;
}

int AudioServer::setFormat(int channel, struct audio_data_format *format)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->setFormat(format);
}

int AudioServer::getFormat(int channel, struct audio_data_format *format)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->getFormat(format);
}

int AudioServer::setStream(int channel, uint32_t handle)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->setStream(handle);
}

int AudioServer::setVolume(int channel, int volume)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->setVolume(volume);
}

int AudioServer::getVolume(int channel)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->getVolume();
}

int AudioServer::setBlockSize(int channel, int blocksize)
{
	return -1;
}

int AudioServer::getBlockSize(int channel)
{
	return -1;
}

int AudioServer::start(int channel)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	driver_->driver_start(device_);
	return 0;
}

int AudioServer::stop(int channel)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	driver_->driver_stop(device_);
	return 0;
}

int AudioServer::wait(int channel)
{
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	while(!playing);
	return 0;
}

void stop_cb(void *arg)
{
	dputs("MIXER IS STOPPED!");
}

size_t stream_block_reader(MonAPI::Stream *stream, void *mem, size_t size)
{
	size_t readsize = 0;
	size_t remain = 0;
	char *p;
	if( mem == NULL ) return -1;
	if( stream == NULL )
	{
		memset(mem, 0, size);
		return 0;
	}
	p = mem;
	remain = size;
	do
	{
		readsize = stream->read(p, remain);
		remain = size - readsize;
		p += readsize;
	}
	while(remain);
	return size;
}

void mixer_th(void *arg)
{
	dputs(__func__);
	dprintf("Mixer: Hi, I am %d\n", syscall_get_tid());
	AudioServer *server = (AudioServer*)arg;
	bool is_stopped = false;
	char *buf;
	int result;
	buf = (char*)malloc(server->blocksize_);
	while(1)
	{
		if( server->channel_ == NULL )
		{
			memset(buf, 0, server->blocksize_);
		}
		else
		{
			result = stream_block_reader(server->channel_->getStream(), buf, server->blocksize_);
			if( result == 0 )
				server->playing = false;
			else
				server->playing = true;
		}
		server->driver_->driver_write_block(server->device_, buf);
	}
}

