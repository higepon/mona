/*!
  \file  Segments.cpp
  \brief class Segment memory management

  Copyright (c) 2003 Higepon
  All rights reserved.
  License=MIT/X Licnese

  \author  HigePon
  \version $Revision$
  \date   create:2003/10/19 update:$Date$
*/

#include <global.h>
#include <Segments.h>

/* Segment Faults */
const byte Segment::FAULT_STACK_OVERFLOW;
const byte Segment::FAULT_OUT_OF_RANGE;
const byte Segment::FAULT_UNKNOWN;

/*----------------------------------------------------------------------
    StackSegment
----------------------------------------------------------------------*/

/*!
    \brief initilize stack segment

    \param  start LinearAddress of start
    \param  size  segment size
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::StackSegment(LinearAddress start, dword size) {

    start_        = start;
    size_         = size + PageManager::ARCH_PAGE_SIZE;
    isAutoExtend_ = false;
}

/*!
    \brief initilize stack segment. Auto extention mode

    \param  start        LinearAddress of start
    \param  initilieSize segment size
    \param  maxSize      segment max size
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::StackSegment(LinearAddress start, dword initileSize, dword maxSize) {

    start_        = start;
    size_         = initileSize + PageManager::ARCH_PAGE_SIZE;
    maxSize_      = maxSize;
    isAutoExtend_ = true;

    g_page_manager->setAttribute((PageEntry*)g_current_process->cr3, start, true, false, true);
}

/*!
    \brief destruct segment

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
StackSegment::~StackSegment() {

}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool StackSegment::faultHandler(LinearAddress address, dword error) {

    if (error == PageManager::FAULT_NOT_WRITABLE && !tryExtend(address)) {

        return false;

    } else if (error == PageManager::FAULT_NOT_EXIST && !allocatePage(address)) {

        return false;
    } else {

        return false;
    }

    return true;
}

/*!
    \brief try to extend stack, when auto extention mode

    \param  address fault point
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool StackSegment::tryExtend(LinearAddress address) {

    if (!isAutoExtend_) {

        /* not auto extension mode */
        errorNumber_ = FAULT_STACK_OVERFLOW;
        return false;
    }

    if (address > start_ + PageManager::ARCH_PAGE_SIZE || address < start_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    if (size_ + PageManager::ARCH_PAGE_SIZE > maxSize_) {

        errorNumber_ = FAULT_STACK_OVERFLOW;
        return false;
    }

    /* page allocation */
    g_page_manager->setAttribute((PageEntry*)g_current_process->cr3, address, true, true, true);

    /* read only */
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3
                                         , address - PageManager::ARCH_PAGE_SIZE, true, false, g_current_process->dpl);

    /* extention done */
    size_ += PageManager::ARCH_PAGE_SIZE;

    return true;
}

/*!
    \brief allocate physical page

    \param  address LinearAddress
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool StackSegment::allocatePage(LinearAddress address) {

    if (address < start_ || address > start_ + size_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page allocation */
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3, address, true, true, g_current_process->dpl);

    return true;
}

/*----------------------------------------------------------------------
    HeapSegment
----------------------------------------------------------------------*/

/*!
    \brief initilize heap segment

    \param  start LinearAddress of start
    \param  size  segment size
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
HeapSegment::HeapSegment(LinearAddress start, dword size) {

    start_ = start;
    size_  = size;
}

/*!
    \brief destruct segment

    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
HeapSegment::~HeapSegment() {

}

/*!
    \brief fault handler

    \param  address LinearAddress of fault point
    \param  error   fault type
    \author HigePon
    \date   create:2003/10/15 update:2003/10/19
*/
bool HeapSegment::faultHandler(LinearAddress address, dword error) {

    if (error != PageManager::FAULT_NOT_EXIST) {

        errorNumber_ = FAULT_UNKNOWN;
        return false;
    }

    if (address < start_ || address > start_ + size_) {

        errorNumber_ = FAULT_OUT_OF_RANGE;
        return false;
    }

    /* page allocation */
    g_page_manager->allocatePhysicalPage((PageEntry*)g_current_process->cr3, address, true, true, g_current_process->dpl);

    return true;
}
