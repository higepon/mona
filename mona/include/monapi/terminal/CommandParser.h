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
#include <monalibc.h>
#endif
#include <vector>
#include <string>
namespace MonAPI {
namespace terminal {

class CommandParser
{
public:
    CommandParser(Writer* writer);
    virtual ~CommandParser();

    int parse(const uint8_t* buffer, uint32_t size);
    int parseInternal();
    int parseEscape();
    int parseString();
    int parseEcma48CSI();
    int parseEcma48CSIArgs(std::vector<uint32_t>& args);
    int getChar();
    bool isDigit(char c);

    int unGetChar();
    void prepareParse(const uint8_t* buffer, uint32_t size);

protected:
    Writer* writer_;
    const uint8_t* buffer_;
    uint32_t size_;
    uint32_t position_;
};

}; // namespace terminal
}; // namespace MonAPI

#endif // _MONAPI_TERMINAL_COMMAND_PARSER_
