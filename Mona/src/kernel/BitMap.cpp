/*!
    \file  BitMap.cpp
    \brief class BitMap

    Copyright (c) 2002,2003 Higepon and the individuals listed on the ChangeLog entries.
    All rights reserved.
    License=MIT/X Licnese

    \author  HigePon
    \version $Revision$
    \date   create:2003/03/30 update:$Date$
*/

#include<BitMap.h>
#include<string.h>

const int BitMap::DWORD_BITS;
const int BitMap::NOT_FOUND;
/*!
    \brief initilize BitMap

    \param number number of bits
    \author HigePon
    \date   create:2003/03/30 update:
*/
BitMap::BitMap(int number) {

    bitsNumber_ = number;
    dwordNumber_ = (bitsNumber_ + DWORD_BITS - 1) / DWORD_BITS;

    map_ = new int[dwordNumber_];
    memset(map_, 0, sizeof(int) * dwordNumber_);

    for (int i = 0; i < bitsNumber_; i++) clear(i);
    return;
}

/*!
    \brief destroy

    \author HigePon
    \date   create:2003/03/30 update:
*/
BitMap::~BitMap() {

    delete[] map_;
    return;
}

/*!
    \brief get number of bits

    \author HigePon
    \date   create:2003/08/22 update:
*/
int BitMap::getBitsNumber() const {

    return bitsNumber_;
}

/*!
    \brief mark bitmap
inc
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

    \return index marked. BitMap::NOT_FOUND when not found
    \author HigePon
    \date   create:2003/03/30 update:2003/10/26
*/
int BitMap::find() {

    for (int i = 0; i < bitsNumber_; i++) {

        if (!marked(i)) {
            mark(i);
            return i;
        }
    }
    return NOT_FOUND;
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
