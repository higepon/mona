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


BitMap::BitMap(int number) {

    bitsNumber_ = number;
    dwordNumber_ = (bitsNumber_ / (sizeof(dword) * 8))
                 + ((bitsNumber_ % (sizeof(dword) * 8)) ? 1 : 0);

    map_ = new dword[dwordNumber_];

    return;
}

BitMap::~BitMap() {

    delete(map_);
    return;
}

void BitMap::mark(int index) {

}

void BitMap::clear(int index) {


}

int BitMap::find() {

    return 0;
}

int BitMap::countClear() {

    return 0;
}

bool BitMap::marked(int index) {

    return true;
}
