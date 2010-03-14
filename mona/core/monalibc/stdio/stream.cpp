#include <monapi/Stream.h>
#include <monapi/System.h>
#include <monapi/messages.h>
#include "stdio_p.h"

extern "C" uint32_t writeStream(void* s, const void* buf, uint32_t size)
{
    MonAPI::Stream *stream;
    stream = (MonAPI::Stream*)s;
    return stream->write((uint8_t*)buf, size);
}

extern "C" uint32_t readStream(void* s, void* buf, uint32_t size)
{
    MonAPI::Stream *stream;
    stream = (MonAPI::Stream*)s;
    stream->waitForRead();
    return stream->read((uint8_t*)buf, size);
}

// これだとおかしくなる by higepon
// extern "C" void stream_opener()
// {
//  stdin->_stream = MonAPI::System::getStdinStream();
//  stdout->_stream = MonAPI::System::getStdoutStream();
//  stderr->_stream = MonAPI::System::getStdoutStream();
// }

extern MonAPI::Stream* inStream;
extern MonAPI::Stream* outStream;

extern "C" void stream_opener()
{
    stdin->_stream = inStream;
    stdout->_stream = outStream;
    stderr->_stream = outStream;
}

