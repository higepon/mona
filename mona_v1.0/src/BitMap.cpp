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

BitMap::BitMap(int number) {

    bitsNumber_ = number;
    dwordNumber_ = (bitsNumber_ / DWORD_BITS)
                 + ((bitsNumber_ % DWORD_BITS) ? 1 : 0);

    map_ = new dword[dwordNumber_];

    for (int i = 0; i < bitsNumber_; i++) clear(i);

    return;
}

BitMap::~BitMap() {

    delete(map_);
    return;
}

void BitMap::mark(int index) {

    map_[index / DWORD_BITS] |= 1 << (index % DWORD_BITS);
    return;
}

void BitMap::clear(int index) {

    map_[index / DWORD_BITS] &= ~(1 << (index % DWORD_BITS));
    return;
}

int BitMap::find() {

    for (int i = 0; i < bitsNumber_; i++) {

        if (!marked(i)) {
            mark(i);
            return i;
        }
    }
    return -1;
}

int BitMap::countClear() {

    int count = 0;

    for (int i = 0; i < bitsNumber_; i++) {

        if (!marked(i)) count++;
    }

    return count;
}

bool BitMap::marked(int index) {

    return(map_[index / DWORD_BITS] & (1 << (index % DWORD_BITS)));
}
