#include "monapi/terminal/CommandParser.h"

using namespace MonAPI::terminal;

CommandParser::CommandParser(Writer* writer)
{

}

CommandParser::~CommandParser()
{

}

int CommandParser::parse(const uint8_t* buffer, uint32_t size)
{
    return MONA_SUCCESS;
}
