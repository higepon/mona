/*!
    \file  MessageServer.cpp
    \brief class MessageServer

    Copyright (c) 2003 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/07/29 update:$Date$
*/

#include<MessageServer.h>
#include<global.h>

MessageServer::MessageServer() {

    QueueManager::init(&queue_);
}

MessageServer::~MessageServer() {

}

bool MessageServer::hasMessage() {

    return !(QueueManager::isEmpty(&queue_));
}

Message* MessageServer::getMessasge() {

    return (Message*)QueueManager::getNext(&queue_);
}

void MessageServer::send(Message* message) {

    QueueManager::addToPrevious(&queue_, message);
}


void MessageServer::Main() {

    while (true) {

        while (hasMessage()) {


            g_console->printf("message come\n");
            getMessasge();
        }

        // sleep
    }
}

void servermanager() {

    g_message_server = new MessageServer();

    g_message_server->Main();
}
