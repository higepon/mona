#ifndef _PMTHREAD_H_
#define _PMTHREAD_H_

#include <sys/types.h>
#include <monapi.h>

enum State
{
	INITIAL, RUNNING, TERMINATED,
};

class PMThread
{
public:
	PMThread(int (*f)());
	virtual ~PMThread();
	void start();
	int run();
	void join();
protected:
	int (*fp)();
	dword tid;
	enum State st;
};

#endif
