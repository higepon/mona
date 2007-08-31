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

AudioServer::AudioServer() : channel_(NULL)
{
	driver_ = audio_driver_factory("es1370");
	device_ = driver_->driver_new();
	dprintf("device_ = %x\n", device_);
	dprintf("this = %x\n", this);
	driver_->driver_set_format(device_, &default_format);
	blocksize_ = driver_->driver_get_block_size(device_);
	mixer_ = new MonAPI::Thread(mixer_th, this, mixer_th);
	mixer_->start();
}

AudioServer::~AudioServer()
{
	driver_->driver_delete(device_);
}

int AudioServer::createChannel(int direction)
{
	dputs(__func__);
	if( channel_ != NULL ) return -1;
	channel_ = new Channel(direction);
	return 0;
}

int AudioServer::destroyChannel(int channel)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	delete channel_;
	channel_ = NULL;
	return 0;
}

int AudioServer::setFormat(int channel, struct audio_data_format *format)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->setFormat(format);
}

int AudioServer::getFormat(int channel, struct audio_data_format *format)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->getFormat(format);
}

int AudioServer::setStream(int channel, uint32_t handle)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->setStream(handle);
}

int AudioServer::setVolume(int channel, int volume)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->setVolume(volume);
}

int AudioServer::getVolume(int channel)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	return channel_->getVolume();
}

int AudioServer::setBlockSize(int channel, int blocksize)
{
	dputs(__func__);
	return -1;
}

int AudioServer::getBlockSize(int channel)
{
	dputs(__func__);
	return -1;
}

int AudioServer::start(int channel)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	driver_->driver_start(device_);
	return 0;
}

int AudioServer::stop(int channel)
{
	dputs(__func__);
	if( channel != 0 ) return -1;
	if( channel_ == NULL ) return -1;
	driver_->driver_stop(device_);
	return 0;
}

void mixer_th(void *arg)
{
	dputs(__func__);
	AudioServer *server = (AudioServer*)arg;
	char *buf;
	buf = (char*)malloc(server->blocksize_);
	while(1)
	{
		if( server->channel_ == NULL ) continue;
		server->channel_->getStream()->read(buf, server->blocksize_);
		server->driver_->driver_write_block(server->device_, buf);
	}
}

/*
	struct audio_driver *driver_ = NULL;
	handle_t device_;
	Channel *channel_;
*/

#if 0 
AudioServer audio_server_new()
{
	struct audio_server *ret;
    
	syscall_get_io();
	ret = (struct audio_server*)calloc(1, sizeof(struct audio_server));
	if( ret == NULL ) return NULL;
	ret->driver = audio_driver_factory("es1370");
	if( ret->driver == NULL )
	{
		free(ret);
		return NULL;
	}
	ret->stream = new MonAPI::Stream;
	if( ret->stream == NULL )
	{
		free(ret->driver);
		free(ret);
		return NULL;
	}
	ret->device = ret->driver->driver_new();
	return (AudioServer)ret;
}

void audio_server_delete(AudioServer o)
{
	struct audio_server *serv;
	serv = (struct audio_server*)o;
	if( serv->device != NULL )
	{
		serv->driver->driver_delete(serv->device);
		serv->device = NULL;
	}
	if( serv->stream != NULL ) delete serv->stream;
}

/* Function: MSG_AUDIO_NEW_CHANNEL
 * To create a new channel.
 * Params:
 * 	type: A type of the channel. It's described by the enum `ChannelType`.
 * Returns: number of the channel, handle of the stream.
 */
int audio_new_channel(AudioServer o, MessageInfo *msg)
{
	assert(o!=NULL);
	o->tid = msg->from;
	if( o->channels != 0 )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	o->channels = 1;
    //_logprintf("o->stream->handle() = %x\n", o->stream->handle());
	MonAPI::Message::reply(msg, 1, o->stream->handle());

	return 0;
}

/* Function: MSG_AUDIO_DELETE_CHANNEL
 * To delete a channel.
 * Params:
 * 	channel: number of a channel.
 * Retruns: None
 */
int audio_delete_channel(AudioServer o, MessageInfo *msg)
{
	assert(o!=NULL);
	if( o->channels == 0 )
	{
		MonAPI::Message::reply(msg, 0);
		return -1;
	}
	o->channels = 0;
	return 0;
}

/* Function: MSG_AUDIO_START
 * To start playing or captureing.
 * Params:
 *	channel: number of a channel.
 * Returns: None
 */
int audio_start(AudioServer o, MessageInfo *msg)
{
	assert(o!=NULL);
	if( o->device == NULL || o->driver == NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
    //_logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	o->driver->driver_set_render_callback(o->device, &audio_render_callback, o);
	o->driver->driver_set_stopped_callback(o->device, &audio_stopped_callback, o);
    //_logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	MonAPI::Message::reply(msg, 0);
	o->driver->driver_start(o->device);
	return 0;
}

/* Function: MSG_AUDIO_STOP
 * To stop playing or captureing.
 * Params:
 * 	channel: number of a channel.
 * Returns: None
 */
int audio_stop(AudioServer o, MessageInfo *msg)
{
	if( o->device == NULL || o->driver == NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	o->driver->driver_stop(o->device);
	MonAPI::Message::reply(msg, 0);
	return 0;
}

/* Function: MSG_AUDIO_SET_FORMAT
 * To set a audio data format.
n * Params:
 * 	channels: number of audio channels
 * 	bits: bits per sample
 * 	rate: samples per second
 * 	byteorder: Byte ordering in sample. But now, It's a little endian only.
 * 	           Therefore you don't have to set this param.
 * Returns: Error Status
 */
int audio_set_format(AudioServer o, MessageInfo *msg)
{
	if( o->device != NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	o->outputFormat.channels = msg->arg1;
	o->outputFormat.bits = msg->arg2;
	o->outputFormat.sample_rate = msg->arg3;
	o->driver->driver_set_format(o, &o->outputFormat);
	if( o->device == NULL )
	{
		MonAPI::Message::reply(msg, (uint32_t)-1);
		return -1;
	}
	MonAPI::Message::reply(msg, 0);
	return 0;
}

/* Function: MSG_AUDIO_SERVER_VERSION
 * To get a version number of this server.
 * Prams: None
 * Retruns: version number
 */

/* Function: MSG_AUDIO_KILL
 * To kill this server.
 * Params: None
 * Returns: None
 */

/* Function: MSG_AUDIO_RESET
 * To reest this server.
 * Params: None
 * Returns: None
 */


int audio_render_callback(void *ref, void *buf, size_t size, size_t *wrote)
{
	AudioServer serv = (AudioServer)ref;
///*
	MonAPI::Stream *stream = serv->stream;
//	stream->waitForRead();
//	stream->lockForRead();
	*wrote = stream->read((uint8_t*)buf, (uint32_t)size);
//	stream->unlockForRead();
	_printf("*wrote = %d\n", *wrote);
	if( *wrote < 1 ) return NG;
	return OK;
//*/
/*
	monapi_cmemoryinfo *cmi;
//	puts("Send a message.");
	MessageInfo msg;
	MonAPI::Message::sendReceive(&msg, serv->tid, 0xFFFFFFFF, size);
//	puts("Received a message from a client.");
//	printf("msg->arg2 (handle) = %d\n", msg.arg2);
//	printf("msg->arg3 (size)   = %d\n", msg.arg3);
	cmi = monapi_cmemoryinfo_new();
	cmi->Size = msg.arg3;
	cmi->Handle = msg.arg2;
	monapi_cmemoryinfo_map(cmi);
	memcpy(buf, cmi->Data, msg.arg3);
	*wrote = msg.arg3;
	return OK;
*/
}

int audio_stopped_callback(void *ref)
{
	logprintf("%s\n", __func__);
	return OK;
}
#endif
