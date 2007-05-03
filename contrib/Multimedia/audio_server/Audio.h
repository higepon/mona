#pragma once
#include <monapi/Mutex.h>
#include <sys/types.h>
#include <sys/HList.h>
#include <monalibc/stdint.h>
#include <monapi/Stream.h>
#include "drivers/audiodriver.h"
#include "servers/audio.h"
#include <stlport/vector>
#include <stlport/map>
#include <stlport/list>
#include <monapi/Thread.h>

#define SYNCHRONIZED(x) { mutex->lock(); x; mutex->unlock(); }

class Channel;
class IntNotifer;

class Audio
{
public:
	friend class ServerCommand;
	Audio();
	~Audio();
	bool init(char *devices[], int devnum);
	bool init_drivers();
	int run();
	uint32_t tid();
	std::list<IntNotifer*> *notifers;
private:
	int counter;
	std::vector<struct driver_desc*> *drivers;
	std::map<char*, int> *drivers_hash;
	ServerCommand *commander;
	HList<void*> *reservoir;
	Channel **channels;
	size_t channelLength;
	MonAPI::Thread *command_thread;
	uint32_t tids[4];
	uint32_t tid_;

	int messageLoop();
	bool findDevices(char *devices[], int devnum);
	struct driver_desc *findDriver(const char* name);
	void *dmabuf;

	inline int makeID(){ return counter++; }
protected:
};

extern MonAPI::Mutex *mutex;

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
	static void command_thread_main_loop(void* arg);
};

class IntNotifer
{
private:
	struct driver_desc *driver;
	ch_t channel;
	int32_t tid;
public:
	IntNotifer(struct driver_desc*, ch_t, int32_t);
	~IntNotifer();

	void Interrupted();
};

