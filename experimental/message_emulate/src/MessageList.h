#ifndef __MESSAGELIST_H__
#define __MESSAGELIST_H__

#include <string>
#include <vector>

typedef struct Message
{
    std::string text;
};

class MessageList
{
public:
    MessageList();
    virtual ~MessageList();

public:
    void push(Message message);
    Message pop();
    Message peek(int index);
    bool exist() const;
    int length() const;
    bool remove(int index);

protected:
    std::vector<Message> list_;
};

#endif // __MESSAGELIST_H__
