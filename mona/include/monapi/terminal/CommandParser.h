#ifndef _MONAPI_TERMINAL_COMMAND_PARSER_
#define _MONAPI_TERMINAL_COMMAND_PARSER_

#include "sys/types.h"
#include "monapi/Assert.h"
#include "monapi/terminal/Writer.h"

#ifdef TERMINAL_TEST
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#else
#include <monapi.h>
#endif

namespace MonAPI {
namespace terminal {

class CommandParser
{
public:
    CommandParser(Writer* writer);
    virtual ~CommandParser();

    int parse(const uint8_t* buffer, uint32_t size);
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_COMMAND_PARSER_
