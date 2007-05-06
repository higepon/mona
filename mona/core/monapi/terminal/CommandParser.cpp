#include "monapi/terminal/CommandParser.h"

using namespace MonAPI::terminal;
using namespace std;

#define SYNTAX_ERROR() printf("syntax error at %s:%d\n", __FILE__, __LINE__)

CommandParser::CommandParser(Writer* writer) : writer_(writer)
{

}

CommandParser::~CommandParser()
{

}

void CommandParser::prepareParse(const uint8_t* buffer, uint32_t size)
{
    buffer_ = buffer;
    size_ = size;
    position_ = 0;
}

int CommandParser::parse(const uint8_t* buffer, uint32_t size)
{
    prepareParse(buffer, size);
    return parseInternal();
}

// ECMA-48 CSI Sequence
// after ESC [, ';' separated decimal number comes, and command character comes last
//   ESC [ Ps A Cursor Up Ps Times (default = 1) (CUU)
//   ESC [ Ps B Cursor Down Ps Times (default = 1) (CUD)
//   ESC [ Ps C Cursor Forward Ps Times (default = 1) (CUF)
//   ESC [ Ps D Cursor Backward Ps Times (default = 1) (CUB)
//   ESC [ Ps ; Ps H    Cursor Position [row;column] (default = [1,1]) (CUP)
//   ESC [ Ps J Erase in Display
//         Ps = 0 -> Clear Below (default)
//         Ps = 1 -> Clear Above
//         Ps = 2 -> Clear All
int CommandParser::parseInternal()
{
    int c = getChar();
    if (c == -1) return 0;
    if (c == 0x1b)
    {
        return parseEscape();
    }
    else
    {
        unGetChar();
        return parseString();;
    }
}

int CommandParser::parseEscape()
{
   int c = getChar();
   if (c == -1) return 0;
   if (c == '[')
   {
       return parseEcma48CSI();
   }
   else
   {
       SYNTAX_ERROR();
       return -1;
   }
}

int CommandParser::parseString()
{
    string ret = "";
    for (;;)
    {
        int c = getChar();
        if (c == 0x1b)
        {
            unGetChar();
            writer_->write((uint8_t*)(ret.c_str()), ret.size());
            return parseInternal();
        }
        else if (c == -1 || c == '\0')
        {
            writer_->write((uint8_t*)(ret.c_str()), ret.size());
            return parseInternal();
        }
        ret += c;
    }
    return 0;
}

// after ESC [, ';' separated decimal number comes, and command character comes last
int CommandParser::parseEcma48CSI()
{
    vector<uint32_t> args;
    int ret = parseEcma48CSIArgs(args);
    if (ret == -1) return -1;
    int c = getChar();
    if (c == -1)
    {
        SYNTAX_ERROR();
        return -1;
    }

    switch(c)
    {
    // up
    case 'A':
        if (args.size() != 1)
        {
            SYNTAX_ERROR();
            return -1;
        }
        writer_->moveCursorUp(args[0]);
        break;
    // down
    case 'B':
        if (args.size() != 1)
        {
            SYNTAX_ERROR();
            return -1;
        }
        writer_->moveCursorDown(args[0]);
        break;
    // right
    case 'C':
        if (args.size() != 1)
        {
            SYNTAX_ERROR();
            return -1;
        }
        writer_->moveCursorRight(args[0]);
        break;
    // left
    case 'D':
        if (args.size() != 1)
        {
            SYNTAX_ERROR();
            return -1;
        }
        writer_->moveCursorLeft(args[0]);
        break;
    // move to (x, y)
    case 'H':
        if (args.size() != 2)
        {
            SYNTAX_ERROR();
            return -1;
        }
        writer_->moveCursor(args[0], args[1]);
        break;
    // clear all screen
    case 'J':
        if (args.size() != 1 || args[0] != 2)
        {
            SYNTAX_ERROR();
            return -1;
        }
        writer_->clearScreen();
        break;
    default:
        SYNTAX_ERROR();
        return -1;
    }
    return parseInternal();
}

// after ESC [, ';' separated decimal number comes, and command character comes last
int CommandParser::parseEcma48CSIArgs(vector<uint32_t>& args)
{
    vector<uint32_t> numbers;
    for (;;)
    {
        int c = getChar();
        if (c == -1)
        {
            SYNTAX_ERROR();
            return -1;
        }
        else if (isDigit(c))
        {
            numbers.push_back(c - '0');
        }
        else if (c == ';')
        {
            uint32_t number = 0;
            for (uint32_t i = 0; i < numbers.size(); i++)
            {
                uint32_t power = 1;
                for (uint32_t j = i; j < numbers.size() - 1; j++)
                {
                    power *= 10;
                }
                number += numbers[i] * power;
            }
            args.push_back(number);
            return parseEcma48CSIArgs(args);
        }
        else
        {
            if (numbers.size() > 0)
            {
                uint32_t number = 0;
                for (uint32_t i = 0; i < numbers.size(); i++)
                {
                    uint32_t power = 1;
                    for (uint32_t j = i; j < numbers.size() - 1; j++)
                    {
                        power *= 10;
                    }
                    number += numbers[i] * power;
                }
                args.push_back(number);
            }
            unGetChar();
            return 0;
        }
    }
}

int CommandParser::getChar()
{
    if (position_ >= size_) return -1;
    return buffer_[position_++];
}

int CommandParser::unGetChar()
{
    position_--;
    return 0;
}

bool CommandParser::isDigit(char c)
{
    return '0' <= c && c <= '9';
}
