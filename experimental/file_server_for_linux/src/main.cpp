#include "ProcessUtil.h"
#include <iostream>
using namespace std;
using namespace mona_helper;

typedef vector<ProcessUtil::Process> Ps;

#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msqid;

void messageInit()
{
    pid_t pid = getpid();
    if (-1 == (msqid = msgget((key_t)pid, 0666 | IPC_CREAT)))
    {
        fprintf(stderr, "can not msgget\n");
        exit(1);
    }
}

void messageDestroy()
{
    if(-1 == msgctl(msqid, IPC_RMID, NULL))
    {
        fprintf(stderr, "msg remove failure");
        exit(1);
    }
}

struct MessageBuf
{
    long int type;
    char data[BUFSIZ];
};


void messageSend(MessageBuf* message)
{
    message->type = 0x1234;
    if (-1 == msgsnd(msqid, message, BUFSIZ, 0))
    {
        fprintf(stderr, "msgsnd failure");
        exit(1);
    }
}


void messageReceive(MessageBuf* message)
{
    if (-1 == msgrcv(msqid, message, BUFSIZ, 0x1234, 0))
    {
        fprintf(stderr, "msgrcv failure");
        exit(1);
    }
}


int main()
{
    Ps ps;
    ProcessUtil::getProcessInfo(ps);

    for (Ps::iterator it = ps.begin(); it != ps.end(); it++)
    {
        cout << (*it).pid << " " << (*it).cmd << endl;
    }
    return 0;
}
