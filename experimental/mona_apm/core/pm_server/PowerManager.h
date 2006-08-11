#ifndef _POWERMANAGER_H_
#define _POWERMANAGER_H_

#include "APM.h"

#define SVR "PowerManager"

class PowerManager
{
public:
	PowerManager();
	void run();
private:
	int feature_;
	APM *apm_;
};

#endif
