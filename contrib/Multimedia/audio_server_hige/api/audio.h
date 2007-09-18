#ifndef _API_AUDIO_H_
#define _API_AUDIO_H_

#include <stdint.h>
#include "../servers/audio.h"
#include <monapi/Stream.h>

class Audio
{
public:
	Audio(struct audio_data_format* format, int direction);
	~Audio();
	int start();
	int stop();
	int setVolume(int volume);
	int getVolume();
	int wait();
	size_t write(void *buffer, size_t size);
	size_t read(void *buffer, size_t size);
private:
	int volume_;
	struct audio_data_format format_;
	uint32_t server_;
	int direction_;
	int channel_;
	MonAPI::Stream *stream_;
};

#endif
