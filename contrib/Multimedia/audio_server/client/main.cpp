#include <servers/audio.h>
#include <monapi/Stream.h>
#include <stdio.h>
#include <audio.h>
#include <assert.h>
#include <math.h>

int frender(FILE *fp, void *buffer ,size_t size)
{
    fread(buffer, size, 1, fp);
    return feof(fp);
}

int render(void* buffer, size_t size)
{
	short *p = (short*)buffer;
	static float phase = 0.0;
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
	FILE *fp;
	struct audio_data_format format = {0, 2, 16, 44100}; 

	audio = new Audio(&format, AUDIO_OUTPUT);
	printf("Audio object was created.\n");
	fp = fopen("/APPS/TEST.RAW", "r");

	audio->start();
	for( ; counter < 44100*10 ; counter += sizeof(buf) )
//	while(1)
	{
frender(fp, buf, sizeof(buf));
		render(buf, sizeof(buf));
//		audio->write(buf, sizeof(buf));
	}
	audio->stop();

	delete audio;
	fclose(fp);
	printf("AUdio object was deleted.\n");
	return 0;
}
