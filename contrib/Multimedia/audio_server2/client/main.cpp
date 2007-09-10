#include <servers/audio.h>
#include <monapi/Stream.h>
#include <stdio.h>
#include <audio.h>
#include <assert.h>
#include <math.h>

int render(void* buffer, size_t size)
{
	short *p = (short*)buffer;
	static float phase = 0.0;
	static int counter = 0;
	float samplingRate = 44100;
	float sinewaveFrequency = 440;
	float freq = sinewaveFrequency * 2 * M_PI / samplingRate;
	for(unsigned int i = 0 ; i < size/4u ; i++ )
	{
		short wave = (short)(3000.0*sin(phase));
		*p++ = wave;
		*p++ = wave;
		phase += freq;
	}
	return 0;
}

int main()
{
	Audio *audio;
	char buf[4096];
	int counter = 0;
	struct audio_data_format format = {0, 2, 16, 44100}; 

	audio = new Audio(&format, AUDIO_OUTPUT);
	printf("Audio object was created.\n");

	audio->start();
	for( ; counter < 44100*2 ; counter += sizeof(buf) )
	{
		render(buf, sizeof(buf));
		audio->write(buf, sizeof(buf));
	}

	delete audio;
	printf("AUdio object was deleted.\n");
	return 0;
}
