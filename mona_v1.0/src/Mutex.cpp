/*!
    \file  Mutex.cpp
    \brief class kMutex

    Copyright (c) 2004 Higepon
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2004/01/12 update:$Date$
*/

#include <Mutex.h>
#include <io.h>
#include <syscalls.h>

/*----------------------------------------------------------------------
    KMutex
----------------------------------------------------------------------*/
int KMutex::idCount = 0;

KMutex::KMutex(Process* process) : process_(process) {
    id_ = allocateId();
    waitList_ = new HList<Thread*>();
}

KMutex::~KMutex() {

    for (int i = 0; i < waitList_->size(); i++) {
        process_->activateMutex(waitList_->get(i));
    }
    delete waitList_;
}

int KMutex::init() {

    BinaryTree<KMutex*>* tree = process_->getKMutexTree();
    tree->add(getId(), this);
    return NORMAL;
}

int KMutex::lock(Thread* thread) {

    enter_kernel_lock_mode();

    /* lock OK */
    if (!isLocked()) {
        owner_ = thread;

    /* lock NG, so wait */
    } else {
        process_->waitMutex(thread);
        waitList_->add(thread);
    }
    exit_kernel_lock_mode();
    return NORMAL;
}

int KMutex::tryLock(Thread* thread) {

    int result;

    /* not locked */
    if (!isLocked()) {
        return NORMAL;
    }

    enter_kernel_lock_mode();

    /* lock OK */
    if (!isLocked()) {
        owner_ = thread;
        result = NORMAL;
    } else {
        result = -1;
    }

    exit_kernel_lock_mode();
    return result;
}

int KMutex::unlock() {

    /* not locked */
    if (!isLocked()) {
        return NORMAL;
    }

    enter_kernel_lock_mode();

    if (waitList_ ->size() == 0) {
        process_->activateMutex(owner_);
        owner_ = NULL;

    } else {
        owner_ = waitList_->remove(0);
    }

    exit_kernel_lock_mode();
    return NORMAL;
}
