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

class PageDirectory : public Queue {

    PageEntry* pageEntry_;
};

class PageManager {


  private:
    PageDirectory* allocatePageDirectory();


};



#endif
