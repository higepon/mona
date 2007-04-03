#pragma once
#include <sys/types.h>
#include <monalibc/stdint.h>
#include <monapi/Stream.h>
#include "drivers/audiodriver.h"
#include "servers/audio.h"
#include <stlport/vector>
#include <stlport/map>

class Audio
{
public:
	friend class ServerCommand;
	Audio();
	~Audio();
	bool init(char *devices[], int devnum);
	bool init_drivers();
	int run();
private:
	int counter;
	std::vector<struct driver_desc*> *drivers;
	std::map<char*, int> *drivers_hash;
	ServerCommand *commander;
//	Stream *stream;

	int messageLoop();
	bool findDevices(char *devices[], int devnum);
	struct driver_desc *findDriver(const char* name);
	void *dmabuf;

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
	ServerCommand(Audio *_parent);
	virtual ~ServerCommand();
	int caller(int, MessageInfo*);
	int Nop(MessageInfo*);
	int GetServerVersion(MessageInfo*);
	int AllocateChannel(MessageInfo*);
	int PrepareChannel(MessageInfo*);
	int ReleaseChannel(MessageInfo*);
	int SetBuffer(MessageInfo*);
	int StartChannel(MessageInfo*);
	int StopChannel(MessageInfo*);
};

