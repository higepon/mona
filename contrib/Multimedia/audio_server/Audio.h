#pragma once
#include <monalibc/stdint.h>
#include "drivers/audiodriver.h"

class Audio
{
public:
	Audio();
	~Audio();
	bool init(char *devices[]);
private:
	bool findDevices();
	struct driver_desc *findDriver(const char* name);
protected:
};
