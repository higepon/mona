/*!
    \file  BitMap.cpp
    \brief class BitMap

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/30 update:$Date$
*/

#include<BitMap.h>

const int BitMap::DWORD_BITS = sizeof(dword) * 8;

/*!
    \brief initilize BitMap

    \param number number of bits
    \author HigePon
    \date   create:2003/03/30 update:
*/
BitMap::BitMap(int number) {

    bitsNumber_ = number;
    dwordNumber_ = (bitsNumber_ / DWORD_BITS)
                 + ((bitsNumber_ % DWORD_BITS) ? 1 : 0);

    map_ = new dword[dwordNumber_];

    for (int i = 0; i < bitsNumber_; i++) clear(i);
    return;
}

/*!
    \brief destroy

    \author HigePon
    \date   create:2003/03/30 update:
*/
BitMap::~BitMap() {

    delete(map_);
    return;
}

/*!
    \brief mark bitmap

    \param index index to mark
    \author HigePon
    \date   create:2003/03/30 update:
*/
void BitMap::mark(int index) {

    map_[index / DWORD_BITS] |= 1 << (index % DWORD_BITS);
    return;
}

/*!
    \brief clear bitmap

    \param index index to clear
    \author HigePon
    \date   create:2003/03/30 update:
*/
void BitMap::clear(int index) {

    map_[index / DWORD_BITS] &= ~(1 << (index % DWORD_BITS));
    return;
}

/*!
    \brief find empty and marked

    \return index marked. -1 when not found
    \author HigePon
    \date   create:2003/03/30 update:
*/
int BitMap::find() {

    for (int i = 0; i < bitsNumber_; i++) {

        if (!marked(i)) {
            mark(i);
            return i;
        }
    }
    return -1;
}

/*!
    \brief count clear bits

    \return clear count
    \author HigePon
    \date   create:2003/03/30 update:
*/
int BitMap::countClear() {

    int count = 0;

    for (int i = 0; i < bitsNumber_; i++) {

        if (!marked(i)) count++;
    }

    return count;
}

/*!
    \brief test if marked

    \param index index to test
    \return true if marked
    \author HigePon
    \date   create:2003/03/30 update:
*/
bool BitMap::marked(int index) {

    return(map_[index / DWORD_BITS] & (1 << (index % DWORD_BITS)));
}
