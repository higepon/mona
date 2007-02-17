#pragma once
#include <sys/types.h>
#include <monalibc/stdint.h>
#include "drivers/audiodriver.h"
#include "servers/audio.h"
#include <stlport/map>

class Audio
{
public:
	friend class ServerCommand;
	Audio();
	~Audio();
	bool init(char *devices[], int devnum);
	int run();
private:
	int counter;
	std::map<char*, struct driver_desc*> *drivers;

	int messageLoop();
	bool findDevices(char *devices[], int devnum);
	struct driver_desc *findDriver(const char* name);

	inline int makeID(){ return counter++; }
protected:
};

class ServerCommand
{
public:
	static int caller(int, Audio*, MessageInfo*);
	static int GetServerVersion(Audio*, MessageInfo*);
};

