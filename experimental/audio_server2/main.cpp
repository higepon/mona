#include "es1370.h"
#include <monalibc/stdio.h>

error_t callback(void* ref, void* buffer, size_t size)
{
	short *p = (short*)buffer;
	puts(__func__);
	printf("p = %x\n", p);
	for(int i = 0 ; i < size/2 ; i++ )
	{
		if( i % 200 < 10 ) p[i] = 200;
		else p[i] = -200;
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
