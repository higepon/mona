#include "audio_server.h"
#include <stdio.h>
#include <math.h>
#include "debug.h"

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

int frender(FILE *fp, void *buffer, size_t size)
{
	fread(buffer, size, 1, fp);
	return feof(fp);
}

int main()
{
	AudioServer server;
	int channel;
	MonAPI::Stream stream;
	struct audio_data_format format = {0, 2, 16, 44100};
	char buf[4096*2];
	FILE *fp = fopen("/APPS/TEST.RAW", "r");

	channel = server.createChannel(0);
	server.setStream(channel, stream.handle());
	dputs("Set stream");
	server.setFormat(channel, &format);
	dputs("Set format");
	render(buf, sizeof(buf));
	stream.write(buf, sizeof(buf));
	render(buf, sizeof(buf));
	stream.write(buf, sizeof(buf));
	server.start(channel);
	dputs("Start");
	while(1)
	//for( int i = 0 ; i < 44100*2*2*3 ; i+= sizeof(buf) )
	{
		//render(buf, sizeof(buf));
		frender(fp, buf, sizeof(buf));
		stream.waitForWrite();
		stream.write(buf, sizeof(buf));
	}
	server.wait(channel);
	server.stop(channel);
	fclose(fp);
	return 0;
}
