#include "Audio.h"
#include <monapi/message.h>
#include "debug.h"

IntNotifer::IntNotifer(struct driver_desc* d, ch_t c, int32_t t) : driver(d), channel(c), tid(t)
{
}

IntNotifer::~IntNotifer()
{
}

void IntNotifer::Interrupted()
{
	int state = driver->get_int_state();
	dputs("Interrupted");
//	if( (state & (1<<2)) && channel == 1 )
	{
		dputs("1");
		MonAPI::Message::send(tid, MSG_AUDIO_SERVER_MESSAGE, BufferIsEmpty);
	}
}
