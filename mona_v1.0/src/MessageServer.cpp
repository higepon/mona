/*!
    \file  MessageServer.cpp
    \brief class MessageServer

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/07/29 update:$Date$
*/

#include<MessageServer.h>


MessageServer::MessageServer() {

    QueueManager::init(&queue_);
}

MessageServer::~MessageServer() {

}

bool MessageServer::hasMessage() {

    return !(QueueManager::isEmpty(&queue_));
}

Message* MessageServer::getMesasge() {

    return (Message*)QueueManager::getNext(&queue_);
}

void MessageServer::send(Message* message) {

    QueueManager::addToPrevious(&queue_, message);
}


void MessageServer::Main() {

    while (true) {

        while (hasMessage()) {



        }

        // sleep
    }
}
