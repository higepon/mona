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
    BitMap(int number);
    ~BitMap();

 public:
    void mark(int index);
    void clear(int index);
    int find();
    int countClear();
    bool marked(int index);

 private:
    dword* map_;
    int bitsNumber_;
    int dwordNumber_;

};

#endif
