#ifndef __PROCESSUTIL_H__
#define __PROCESSUTIL_H__

#include <string>
#include <vector>
#include <sstream>
#include "types.h"

namespace mona_helper {

class ProcessUtil
{
private:
    ProcessUtil();
    virtual ~ProcessUtil();

public:
    typedef struct Process
    {
        std::string pid, tty, time, cmd;
    };

    static void getProcessInfo(std::vector<Process>& ps);
    static dword getPid(std::string name);
};

}; // namespace mona_helper

#endif // __PROCESSUTIL_H__
