#ifndef __UIP_LOOP_H__
#define __UIP_LOOP_H__

#include <limits.h>
#include <string>

typedef struct WGetInfo
{
    std::string host;
    std::string path;
    std::string content;
    uint32_t port;
    uint32_t from;
    bool exec;
};

#endif /* __UIP_LOOP_H__ */
