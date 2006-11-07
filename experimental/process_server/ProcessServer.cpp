#include "ProcessServer.h"

ProcessServer::ProcessServer() {

    return;
}

ProcessServer::~ProcessServer() {

    return;
}

int ProcessServer::init() {


}

int ProcessServer::run() {

    int status = 1;
    MessageInfo *msg = new MessageInfo();

    while(status != 0) {
        if(Message::receive(msg) != 0) continue;

        Switch(msg->header) {
        case MSG_PROCESS_EXECUTE_FILE:
            {
                dword tid = 0;
                int result = executeFile();
                Message::reply(msg, result, tid);
                break;
            }
        case MSG_REGISTER_TO_SERVER:
            registerReceiver(msg->arg1);
            Message::reply(msg);
            break;
        case MSG_UNREGISTER_FROM_SERVER:
            unregisterReceiver(msg->arg1);
            Message::reply(msg);
            break;
        case MSG_PROCESS_GET_PROCESS_INFO:
            {
                ProcessInfo pi = getProcessInfo(msg->from);
                Message::reply(msg, pi.parent, 0, pi.path);
                break;
            }
        case MSG_PROCESS_CREATED:
            addProcessInfo(msg->arg1, msg->arg2, msg->str);
            break;
        case MSG_PROCESS_TERMINATED:
            removeProcessInfo(msg->arg1);
            break;
        case MSG_PROCESS_GET_COMMON_PARAMS:
            Message::reply(msg, commonParams->Handle);
            break;
        case MSG_PROCESS_STOP_SERVER:
            Message::reply(msg);
            status = 0;
        default:
            break;
        }
    }
    return status;
}

void ProcessServer::registerReceiver(dword tid) {

    int size = receivers.size();

    for (int i = 0; i < size; i++) {
        if (receivers[i] == tid) return;
    }
    receivers.add(tid);
}

void ProcessServer::unregisterReceiver(dword tid) {

    int size = receivers.size();

    for (int i = 0; i < size; i++) {
        if (receivers[i] != tid) continue;

        receivers.removeAt(i);
        return;
    }
}


