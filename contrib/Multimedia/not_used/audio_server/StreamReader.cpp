#include "Audio.h"
#include "debug.h"

StreamReader::StreamReader(struct driver_desc* d, ch_t c, MonAPI::Stream *s, Audio* p) : driver(d), channel(c), stream(s), parent(p), stopping(false)
{
}

StreamReader::~StreamReader()
{
	if( stream != NULL ) delete stream;
}

void StreamReader::Interrupted()
{
	int state = driver->get_int_state();
	uint32_t readsize;
	uint8_t buf[0x10000];
	dputs("Interrupted");
	//if( (state & (1<<1)) && channel == 1 )
	{
		stream->waitForRead();
		readsize = stream->read(parent->getOffBuffer(), 512);
	//	memcpy(parent->getOffBuffer(), buf, readsize);
		parent->turnBuffer();
	//	printf("readsize = %d\n", readsize);
//		driver->stop_channel(channel);
		driver->set_buffer(channel, (uint8_t*)parent->getUsingBuffer(), readsize);
		driver->start_channel(channel, 1);

	}
}
