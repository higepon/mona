#ifndef __MESSAGEUTIL_H__
#define __MESSAGEUTIL_H__

typedef unsigned char byte;
typedef unsigned long dword;

class MessageUtil
{
public:
    MessageUtil();
    virtual ~MessageUtil();

    byte charTo5bit(char c);
    dword stringToDword(const char* str, bool reply = false);
};

#endif // __MESSAGEUTIL_H__
