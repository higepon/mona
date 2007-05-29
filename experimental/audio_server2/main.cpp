#include "es1370.h"
#include <monalibc/math.h>
#include <monalibc/stdio.h>

error_t callback(void* ref, void* buffer, size_t size)
{
	short *p = (short*)buffer;
	static float phase = 0.0;
	float samplingRate = 44100;
	float sinewaveFrequency = 440;
	float freq = sinewaveFrequency * 2 * M_PI / samplingRate;
//	puts(__func__);
//	printf("p = %x\n", p);
	for(unsigned int i = 0 ; i < size/4u ; i++ )
	{
		short wave = (short)(200.0*sin(phase));
		*p++ = wave;
		*p++ = wave;
		phase += freq;
	}
	return OK;
}

int main()
{
	handle_t dev;
	struct audio_data_format format;

	format.sample_rate = 44100;
	format.bits = 16;
	format.channels = 2;

	printf("callback: %x\n", &callback);

	dev = es1370_new(&format);
	if( dev == NULL )
	{
		puts("Couldn't open the device.");
		return 1;
	}
	es1370_set_callback(dev, &callback, dev);
	es1370_start(dev);
	while(1);

	es1370_delete(dev);

	return 0;
}
