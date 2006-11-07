#ifndef __PROCESS_SERVER_H__
#define __PROCESS_SERVER_H__

#define SVR "Process Server"

class ProcessServer {

public:
    ProcessServer();
    ~ProcessServer();
    int init();
    int run();

private:
    int executeProcess();
    int executeFile();

    HList<dword>receivers;
}

#endif  // __PROCESS_SERVER_H__
