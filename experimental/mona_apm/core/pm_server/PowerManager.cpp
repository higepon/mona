#include <monapi.h>
#include <monapi/messages.h>
#include <monalibc.h>
#include "PowerManager.h"
#include "APM.h"

using namespace MonAPI;

PowerManager::PowerManager()
{
	feature_ = syscall_shutdown(SHUTDOWN_FEATURE, SHUTDOWN_FEATURE_APM);
	if( feature_ == SHUTDOWN_FEATURE_APM )
	{
		apm_ = new APM;
	}
	else
	{
		printf("%s: PowerManagement is not supported.\n", SVR);
	}
}

void PowerManager::run()
{
	if( feature_ == SHUTDOWN_FEATURE_APM )
	{
		apm_->init();
		apm_->MessageLoop();
	}
}
