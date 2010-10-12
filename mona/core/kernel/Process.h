/*!
    \file  Process.h
    \brief class Process

    class Process

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/06/27 update:$Date$
*/
#ifndef _MONA_PROCESS_
#define _MONA_PROCESS_

#include <sys/types.h>
#include <sys/List.h>
#include <sys/BinaryTree.h>
#include "PageManager.h"
#include "Segments.h"
#include "kernel.h"
#include "Mutex.h"
#include "UserSemaphore.h"
#include "KObject.h"
#include "Thread.h"
#include "MemoryAllocator.h"
#include "Pair.h"
#define DPL_KERNEL  0
#define DPL_USER    3

class Thread;
class Process;
class MemoryAllocator;
extern "C" VirtualConsole* g_console;

template <class T> class HList2 : public List<T> {

  public:
    HList2();
    HList2(int size);
    HList2(int size, int increase);
    virtual ~HList2();

  public:
    void add(T element);
    T get(int index) const;
    T operator[](int index);
    bool removeAt(int index, T* found = NULL);
    bool remove(T element);
    int size() const;
    virtual bool isEmpty() const;
    bool hasElement(T element) const;

  private:
    T* data_;         /*! internal array     */
    int size_;        /*! size of liset      */
    int numElements_; /*! number of elements */
    int increase_;    /*! increase           */

    /* initilize */
    void init(int size, int increase);

};

/*!
    \brief constructor

    constructor default size is 5

    \author Higepon
    \date   create:2002/10/22 update:
*/
template <class T> HList2<T>::HList2() {
    init(5, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of list

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> HList2<T>::HList2(int size) {

    init(size, 5);
    return;
}

/*!
    \brief constructor

    constructor

    \param size size of initial size of list
    \param increase when resize this value used

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> HList2<T>::HList2(int size, int increase) {

    init(size, increase);
    return;
}

/*!
    \brief destructor

    destructor

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> HList2<T>::~HList2() {

    /* release memory */
    delete[] data_;
    return;
}

/*!
    \brief isEmpty

    return is Empty or not

    \return true/false empty/has elements

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> bool HList2<T>::isEmpty() const {

    return numElements_ == 0;
}

/*!
    \brief add element

    add element at the end of array

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> void HList2<T>::add(T element) {

    /* if array is full */
    if (size_ == numElements_) {

        /* resize array */
        size_ += increase_;
        T* temp = new T[size_];

        /* optimize ? */
        int numElements = numElements_;

        /* copy original to new array */
        for (int i = 0; i < numElements; i++) {
            temp[i] = data_[i];
        }

        delete[] data_;
        data_ = temp;
    }

    /* add element */
    data_[numElements_] = element;
    numElements_++;
    return;
}

/*!
    \brief get

    get element at index

    \param index index of element to get

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> T HList2<T>::get(int index) const {

    /* check range */
    ASSERT(0 <= index  && index < numElements_);
    return data_[index];
}

/*!
    \brief operator[]

    get element at index

    \param index index of element to get

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> T HList2<T>::operator[](int index) {

    return (this->get(index));
}

/*!
    \brief size

    return size of list

    \return size of list

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> int HList2<T>::size() const {
    g_log->printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    return numElements_;
}

/*!
    \brief remove element

    remove element at index

    \param index that removed

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> bool HList2<T>::removeAt(int index, T* found) {

    /* check range */
    if (index < 0 || index >=numElements_) {

        /* do nothing */
        return false;
    }

    /* save element to remove */
    T toRemove = data_[index];

    /* fix hole */
    int numElements = numElements_;
    for (int i = index; i < numElements - 1; i++) {
        data_[i] = data_[i + 1];
    }
    numElements_--;
    if (found != NULL) {
        *found = toRemove;
    }
    return true;
}

/*!
    \brief remove element

    remove element

    \param element element to remove

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> bool HList2<T>::remove(T element) {

    /* optimize */
    g_log->printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    int size = this->size();
    g_log->printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    for (int i = 0; i < size; i++) {
    g_log->printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
        /* element to remove found */
        if (data_[i] == element) {
    g_log->printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            removeAt(i);
    g_log->printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
            return true;
        }
    }

    return false;
}

/*!
    \brief initilize

    set size of list & increase

    \author Higepon
    \date   create:2003/12/07 update:
*/
template <class T> void HList2<T>::init(int size, int increase) {

    /* number of elements */
    numElements_ = 0;

    /* set size and increase */
    size_     = size     > 0 ? size : 5;
    increase_ = increase > 0 ? increase : 5;

    /* create internal array */
    data_ = new T[size_];
    return;
}

/*!
    \brief check list has the element

    \author Higepon
    \date   create:2003/12/21 update:
*/
template <class T> bool HList2<T>::hasElement(T element) const {

    /* optimize? */
    int size = this->size();

    /* find element */
    for (int i = 0; i < size; i++) {

        if (data_[i] == element) {
            return true;
        }
    }

    /* not found */
    return false;
}


/*----------------------------------------------------------------------
    Idle function
----------------------------------------------------------------------*/
void monaIdle();

/*----------------------------------------------------------------------
    Arch dependent functions
----------------------------------------------------------------------*/
extern "C" void arch_switch_thread_to_user1();
extern "C" void arch_switch_thread_to_user2();
extern "C" void arch_switch_thread_to_v861();
extern "C" void arch_switch_thread_to_v862();
extern "C" void arch_switch_thread1();
extern "C" void arch_switch_thread2();
extern "C" void arch_idle();

/*----------------------------------------------------------------------
    ArchThreadInfo
----------------------------------------------------------------------*/
typedef struct ArchThreadInfo
{
    uint32_t  eip;       // 0
    uint32_t  cs;        // 4
    uint32_t  eflags;    // 8
    uint32_t  eax;       // 12
    uint32_t  ecx;       // 16
    uint32_t  edx;       // 20
    uint32_t  ebx;       // 24
    uint32_t  esp;       // 28
    uint32_t  ebp;       // 32
    uint32_t  esi;       // 36
    uint32_t  edi;       // 40
    uint32_t  ds;        // 44
    uint32_t  es;        // 48
    uint32_t  fs;        // 52
    uint32_t  gs;        // 56
    uint32_t  ss;        // 60
    uint32_t  dpl;       // 64
    uint32_t  esp0;      // 68
    uint32_t  ss0;       // 72
    uint32_t  cr3;       // 76
    uint32_t  fpu[27];   // 80
};

/*----------------------------------------------------------------------
    ThreadInfo
----------------------------------------------------------------------*/
typedef struct ThreadInfo
{
    ArchThreadInfo* archinfo;
    Thread* thread;
    Process* process;
};

/*----------------------------------------------------------------------
    ProcessOperation
----------------------------------------------------------------------*/
class ProcessOperation
{
  public:
    static void initialize(PageManager* manager);
    static Process* create(int type, const char* name);
    static LinearAddress allocateKernelStack();
    static void freeKernelStack(LinearAddress address);

  private:
    enum
    {
        KERNEL_STACK_START     = 0x100000,
        KERNEL_STACK_END       = 0x200000,
        KERNEL_STACK_SIZE      = KERNEL_STACK_END - KERNEL_STACK_START,
        KERNEL_STACK_UNIT_SIZE = 0x1000,
    };

  public:
    static PageManager* pageManager;
    static const int USER_PROCESS   = 1;
    static const int KERNEL_PROCESS = 2;
};

/*----------------------------------------------------------------------
    ThreadOperation
----------------------------------------------------------------------*/
class ThreadOperation
{
  public:
    static Thread* create(Process* process, uint32_t programCounter);
    static int switchThread(bool isProcessChanged, int i);
    static intptr_t kill(Process* process, Thread* thread);
    static intptr_t kill(uint32_t tid);

  private:
    static void sendKilledMessage();
    static void archCreateUserThread(Thread* thread, uint32_t programCounter, PageEntry* directory, LinearAddress stack);
    static void archCreateThread(Thread* thread, uint32_t programCounter, PageEntry* directory, LinearAddress stack);
    static uint32_t id;
};


/*----------------------------------------------------------------------
    Process
----------------------------------------------------------------------*/
class Process
{
private:
    Process() {}
  public:
    Process(const char* name, PageEntry* directory);
    virtual ~Process();

  public:
    bool hasSharedOverlap(uintptr_t start, uintptr_t end);

    uint32_t getStackBottom(Thread* thread);

    inline int getThreadIndex(Thread* thread)
    {
        for (int i = 0; i < threadList_->size(); i++)
        {
            if (threadList_->get(i) == thread) {
                return i;
            }
        }
        return -1;
    }

    inline virtual const char* getName() const
    {
        return name_;
    }

    inline virtual uint32_t getPid()
    {
        return pid_;
    }

    inline virtual bool isUserMode() const
    {
        return isUserMode_;
    }

    inline virtual PageEntry* getPageDirectory() const
    {
        return pageDirectory_;
    }

    inline virtual List<class SharedMemorySegment*>* getSharedList() const
    {
        return shared_;
    }

    inline virtual class Segment* getHeapSegment()
    {
        return &heap_;
    }

    inline virtual List<MessageInfo*>* getMessageList() const
    {
        return messageList_;
    }

    inline virtual List<char*>* getArguments()
    {
        return arguments_;
    }

    inline uint32_t allocateStack() const
    {
        return STACK_START - (STACK_SIZE + STACK_SIZE) * (threadNum - 1);
    }

    inline List<Thread*>* getThreadList() const
    {
        return threadList_;
    }

    inline void* AllocateLinearAddress(uint32_t size) {
        if (this->lallocator == NULL)
        {
            this->lallocator = new MemoryAllocator(0xd0000000, 256 * 1024 * 1024);
        }
        return this->lallocator->Allocate(size);
    }

    inline void addKObject(intptr_t id, KObject* obj)
    {
        kobjects_.add(Pair<intptr_t, KObject*>(id, obj));
    }

    inline bool removeKObject(intptr_t id, KObject* obj)
    {
        return kobjects_.remove(Pair<intptr_t, KObject*>(id, obj));
    }

    SharedMemorySegment* findSharedSegment(uint32_t id) const;

    inline HList2< Pair<intptr_t, KObject*> >* getKObjects()
    {
        return &kobjects_;
    }

    static const LinearAddress STACK_START = 0xF0000000;
    static const uint32_t STACK_SIZE          = 0x400000;

  public:
    int threadNum;

  protected:
    static uint32_t pid;
    MemoryAllocator* lallocator;
    List<Thread*>* threadList_;
    List<char*>* arguments_;
    class Segment heap_;
//    class SharedMemorySegment* dllsegment_;
    List<SharedMemorySegment*>* shared_;
    List<MessageInfo*>* messageList_;
    HList2< Pair<intptr_t, KObject*> > kobjects_;
    bool isUserMode_;
    PageEntry* pageDirectory_;
    char name_[16];
    uint32_t pid_;
};

/*----------------------------------------------------------------------
    UserProcess
----------------------------------------------------------------------*/
class UserProcess : public Process
{
private:
    UserProcess();
public:
    UserProcess(const char* name, PageEntry* directory);
    virtual ~UserProcess();
};

/*----------------------------------------------------------------------
    KernelProcess
----------------------------------------------------------------------*/
class KernelProcess : public Process
{
private:
    KernelProcess();
public:
    KernelProcess(const char* name, PageEntry* directory);
    virtual ~KernelProcess();
};

#endif
