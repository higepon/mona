/*!
    \file  PagingUtil.h
    \brief some functions for Paging set up

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/06/10 update:$Date$
*/
#ifndef _MONA_PAGINGUTIL_
#define _MONA_PAGINGUTIL_

typedef dword linear_addr;  /* 32bit */
typedef dword vurtual_addr; /* 32bit */
typedef dword phys_addr;    /* 32bit */


/*!
    \struct PTE
 */
typedef struct  {
    unsigned present:1;          /*!< present bit            */
    unsigned readWrite:1;        /*!< read/write bit         */
    unsigned user:1;             /*!< user superviser bit    */
    unsigned pageWriteThrough:1; /*!< page write through bit */
    unsigned pageCacheDisable:1; /*!< page cache disable bit */
    unsigned accesse:1;          /*!< access bit             */
    unsigned dirty:1;            /*!< dirty bit              */
    unsigned intelReserved:2;    /*!< reserved by intel      */
    unsigned monaAvailable:3;    /*!< mona may use this      */
    unsigned pageBaseAddress:20; /*!< base address of page   */
} PTE;

extern "C" PTE* g_page_dir;

class PagingUtil {

  public:
    static void setup();
    static void makePTE(PTE* pte);
    static void makePTE(PTE* pte, dword physaddr, byte readWrite, byte user);
    static void setCR3(dword physaddr);
    static void startPaging();
    static void stopPaging();
    inline static word getDirEntry(linear_addr p);
    inline static word getPageEntry(linear_addr p);
    inline static PTE* linearToPTE(linear_addr p);
};

inline word PagingUtil::getDirEntry(linear_addr p) {

    return ((p >> 22) & 0x3ff);
}

inline word PagingUtil::getPageEntry(linear_addr p) {

    return ((p >> 12) & 0x3ff);
}

/*  inline PTE* PagingUtil::linearToPTE(linear_addr p) { */

/*      return (PTE*)(g_page_dir[getDirEntry(p)]); */
/*  } */


#endif
