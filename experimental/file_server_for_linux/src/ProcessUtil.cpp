#include "ProcessUtil.h"

using namespace mona_helper;
using namespace std;

ProcessUtil::ProcessUtil()
{
}

ProcessUtil::~ProcessUtil()
{
}

void ProcessUtil::getProcessInfo(vector<Process>& ps)
{
    char buf[4096];
    FILE* pfs = popen("ps -e", "r");
    if (pfs == NULL) return;
    Process p;
    while (fgets(buf, 4096, pfs) != NULL) 
    {
        stringstream ss(buf);
        ss >> p.pid;
        ss >> p.tty;
        ss >> p.time;
        ss >> p.cmd;
        ps.push_back(p);
    }
    pclose(pfs);
}

typedef vector<ProcessUtil::Process> Ps;

dword ProcessUtil::getPid(string name)
{
    Ps ps;
    ProcessUtil::getProcessInfo(ps);

    for (Ps::iterator it = ps.begin(); it != ps.end(); it++)
    {
        if ((*it).cmd == name)
        {
            return atoi((*it).pid.c_str());
        }
    }
    return 0xffffffff;
}




