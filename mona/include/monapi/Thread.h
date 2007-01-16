#ifndef _MONAPI_THREAD_
#define _MONAPI_THREAD_

#include <sys/types.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Thread
----------------------------------------------------------------------*/
class Thread
{
public:
    Thread(void (*run)(void*), void* arg, void (*notify)(void*));
    virtual ~Thread();

    void start();
    void stop();

protected:
    void (*run_)(void*);
    void (*notify_)(void*);
    void* arg_;
    uint32_t threadId_;
};

/*----------------------------------------------------------------------
    How to use
----------------------------------------------------------------------*/
// static void hoge(void* arg)
// {
//     const char* m = (const char*)arg;
//     for (int i = 0; i < 3 ; i++)
//     {
//         sleep(3000);
//         syscall_print(m);
//     }
// }

// static void hogeDone(void* arg)
// {
//     syscall_print("hoge done\n");
// }


// int main(int argc, char* argv[])
// {
//     MonAPI::Thread* thread = new MonAPI::Thread(*hoge, (void*)"message from hige\n", *hogeDone);
//     thread->start();
//     sleep(4000);
//     thread->stop();
//     printf("stopped\n");
// }



};
#endif
