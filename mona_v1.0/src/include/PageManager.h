/*!
    \file  PageManager.h
    \brief some functions for page management

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/08/19 update:$Date$
*/
#ifndef _MONA_PAGE_MANAGER_
#define _MONA_PAGE_MANAGER_

#include <Queue.h>

typdef dword PageEntry;
typdef dword LenearAddress;

class PageDirectory : public Queue {

    PageEntry* pageEntry_;
};

class PageManager {


  private:
    PageDirectory* allocatePageDirectory();
    void makePageEntry(PageEntry* entry, LenearAddress address, bype present, byte rw, byte user);


};



#endif
