#include "MessageList.h"

using namespace std;

MessageList::MessageList()
{
}

MessageList::~MessageList()
{
}

void MessageList::push(Message message)
{
    list_.push_back(message);
}

Message MessageList::pop()
{
    Message result = list_[0];
    list_.erase(list_.begin());
    return result;
}

bool MessageList::exist() const
{
    return !list_.empty();
}

int MessageList::length() const
{
    return list_.size();
}

Message MessageList::peek(int index)
{
    Message result;
    if (index >= list_.size())
    {
        return result;
    }
    result = list_[index];
    return result;
}

bool MessageList::remove(int index)
{
    Message result;
    if (index >= list_.size())
    {
        return false;
    }
    vector<Message>::iterator i = list_.begin();
    i += index;
    list_.erase(i);
    return true;
}
