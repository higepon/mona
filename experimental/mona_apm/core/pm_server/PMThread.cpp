#include "PMThread.h"
#include <sys/types.h>
#include <monapi.h>

PMThread::PMThread(int (*f)())
{
	this->fp = f;
	this->tid = THREAD_UNKNOWN;
	this->st = INITIAL;
}

PMThread::~PMThread()
{
	if( this->st == RUNNING || this-> st == INITIAL )
	{
		mthread_join(tid);
	}
}

void PMThread::start()
{
	tid = mthread_create((dword)this->fp);
	this->run();
}

int PMThread::run()
{
	this->st = RUNNING;
	if( fp != NULL )
		return this->fp();
	return -1;
}

void PMThread::join()
{
	mthread_join(tid);
	this->st = TERMINATED;
}
