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
