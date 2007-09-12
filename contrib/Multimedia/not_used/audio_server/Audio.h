#pragma once
#include <monapi/Mutex.h>
#include <sys/types.h>
#include <sys/HList.h>
#include <monalibc/stdint.h>
#include <monapi/Stream.h>
#include "drivers/audiodriver.h"
#include "servers/audio.h"
#include <stlport/list>
#include <monapi/Thread.h>

class Channel;
class Notifer;
class IntNotifer;

class Audio
{
public:
	friend class ServerCommand;
	Audio();
	~Audio();
	bool init(char *devices[], int devnum);
	bool init_driver();
	int run();
	uint32_t tid();
	std::list<Notifer*> *notifers;
	inline void* getUsingBuffer(){ return usingbuffer ? dmabuf1 : dmabuf2; }
	inline void* getOffBuffer(){ return !usingbuffer ? dmabuf1 : dmabuf2; }
	inline void turnBuffer(){ usingbuffer = !usingbuffer; }
private:
	int counter;
	struct driver_desc *driver;
	ServerCommand *commander;
	Channel **channels;
	size_t channelLength;
	uint32_t tids[4];
	uint32_t tid_;

	int messageLoop();
	bool findDevices(char *devices[], int devnum);
	struct driver_desc *findDriver(const char* name);
	void *dmabuf1;
	void *dmabuf2;
	bool usingbuffer;

	inline int makeID(){ return counter++; }
protected:
};

class Channel
{
public:
	Channel();
	virtual ~Channel();
	bool init(struct driver_desc *driver);
	void prepare(int samplerate, int bits, int isStereo);
	void set_buffer(void *p, int size);
	void start();
	void stop();
private:
	struct driver_desc *driver;
	ch_t channel;
protected:
};


class ServerCommand
{
private:
	Audio *parent;
public:
	struct driver_desc *driver;
	ServerCommand(Audio *_parent);
	virtual ~ServerCommand();
	int caller(int, MessageInfo*);
	int GetThreadID(MessageInfo*);
	int Nop(MessageInfo*);
	int GetServerVersion(MessageInfo*);
	int AllocateChannel(MessageInfo*);
	int PrepareChannel(MessageInfo*);
	int ReleaseChannel(MessageInfo*);
	int SetBuffer(MessageInfo*);
	int StartChannel(MessageInfo*);
	int StopChannel(MessageInfo*);
	int CreateDataStream(MessageInfo*);
	int CreateChannelObject(MessageInfo*);
	int BindChannelObject(MessageInfo*);
	int RegisterTID(MessageInfo*);
	int CreateStream(MessageInfo*);
	static void command_thread_main_loop(void* arg);
};

class Notifer
{
public:
	virtual void Interrupted() = 0;
};

class IntNotifer : public Notifer
{
private:
	struct driver_desc *driver;
	ch_t channel;
	int32_t tid;
public:
	IntNotifer(struct driver_desc*, ch_t, int32_t);
	virtual ~IntNotifer();

	virtual void Interrupted();
};

class StreamReader : public Notifer
{
private:
	struct driver_desc *driver;
	ch_t channel;
	MonAPI::Stream *stream;
	Audio *parent;
	bool stopping;
public:
	StreamReader(struct driver_desc*, ch_t, MonAPI::Stream*, Audio*);
	virtual ~StreamReader();
	virtual void Interrupted();
};

