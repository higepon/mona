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


class PagingUtil {

  public:
    static void setup();
    static void makePTE(PTE* pte);
    static void makePTE(PTE* pte, dword physaddr, byte readWrite, byte user);
    static void setCR3(dword physaddr);
    static void startPaging();
    static void stopPaging();
};

#endif
