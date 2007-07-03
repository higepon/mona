#include "audio_driver.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <monapi/syscall.h>
#include <monapi/messages.h>

jmp_buf jb;

error_t stopped(void *ref)
{
	longjmp(jb, 1);
	return OK;
}

error_t render(void* ref, void* buffer, size_t size, size_t *wrote)
{
	short *p = (short*)buffer;
	static float phase = 0.0;
	static int counter = 0;
	float samplingRate = 44100;
	float sinewaveFrequency = 440;
	float freq = sinewaveFrequency * 2 * M_PI / samplingRate;
//	puts(__func__);
//	printf("p = %x\n", p);
	if( counter >= 44100*2*3) return NG;
	for(unsigned int i = 0 ; i < size/4u ; i++ )
	{
		short wave = (short)(1000.0*sin(phase));
		*p++ = wave;
		*p++ = wave;
		phase += freq;
	}
	counter += size;
	*wrote = size;
	return OK;
}

error_t frender(void *ref, void *buffer ,size_t size, size_t *wrote)
{
	FILE *fp = (FILE*)ref;
	*wrote = fread(buffer, size, 1, fp);
	return feof(fp) != 0 ? NG : OK;
}

error_t cmrender(void *ref, void *buffer ,size_t size, size_t *wrote)
{
	monapi_cmemoryinfo *cmi = (monapi_cmemoryinfo*)ref;
	static unsigned int pos = 0;
	size_t write = size;
	pos += size;
	if( cmi->Size - pos < size ) write = cmi->Size - pos;
	memcpy(buffer, cmi->Data+pos, write);
	*wrote = write;
	if( cmi->Size - pos < size ) return NG;
	return OK;
}

int main()
{
	handle_t dev;
	struct audio_data_format format;
	struct audio_driver *driver;
	monapi_cmemoryinfo *cmi;
//	cmi = monapi_file_read_all("/APPS/TEST.RAW");
	FILE *fp;
	fp = fopen("/APPS/TEST.RAW", "r");
	if( fp == NULL ) return 1;
	setbuf(fp, NULL);

	format.sample_rate = 44100;
	format.bits = 16;
	format.channels = 2;

	printf("callback: %x\n", &render);

	driver = audio_driver_factory("es1370");
	puts("got a driver");
	if( driver == NULL ) return 1;

	dev = driver->driver_new(&format);
	if( dev == NULL )
	{
		puts("Couldn't open the device.");
		return 1;
	}
//	driver->driver_set_render_callback(dev, &render, dev);
	driver->driver_set_render_callback(dev, &frender, fp);
//	driver->driver_set_render_callback(dev, &cmrender, cmi);
	driver->driver_set_stopped_callback(dev, &stopped, dev);
	if( setjmp(jb) != 0 ) goto End;
	driver->driver_start(dev);
	while(1) syscall_mthread_yield_message();
End:
	puts("Stopped");

	fclose(fp);
	driver->driver_stop(dev);
	driver->driver_delete(dev);

	return 0;
}
