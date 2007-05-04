#include "TestWriter.h"

using namespace MonAPI::terminal;

TestWriter::TestWriter() : x_(0), y_(0), isCursorDisplayed_(false)
{

}

TestWriter::~TestWriter()
{

}

int TestWriter::drawCursor()
{
}

int TestWriter::eraseCursor()
{

}

int TestWriter::clearScreen()
{

}

int TestWriter::moveCursor(uint32_t x, uint32_t y)
{

}

int TestWriter::moveCursorUp(uint32_t n)
{
}

int TestWriter::moveCursorDown(uint32_t n)
{
}

int TestWriter::moveCursorLeft(uint32_t n)
{
}

int TestWriter::moveCursorRight(uint32_t n)
{
}

int TestWriter::moveCursorTo(uint32_t n, char direction)
{
}

