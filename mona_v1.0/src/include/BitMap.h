/*!
    \file  BitMap.h
    \brief class BitMap

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/30 update:$Date$
*/
#ifndef _MONA_BITMAP_
#define _MONA_BITMAP_

#include<types.h>

/*!
    BitMap claass
*/
class BitMap {

 public:
    BitMap(dword number);
    ~BitMap();

 public:
    void mark(dword index);
    void clear(dword index);
    dword find();
    dword countClear();
    bool marked(dword index);
    dword getBitsNumber() const;

 private:
    dword* map_;
    dword bitsNumber_;
    dword dwordNumber_;
    static const dword DWORD_BITS;

};

#endif
