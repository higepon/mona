/*!
    \file  BitMap.h
    \brief class BitMap

    Copyright (c) 2003 Higepon
    WITHOUT ANY WARRANTY

    \author  Higepon
    \version $Revision$
    \date   create:2003/03/30 update:$Date$
*/
#ifndef _MONA_BITMAP_
#define _MONA_BITMAP_

#include <sys/types.h>

/*!
    BitMap claass
*/
class BitMap {

public:
    BitMap(int number) : bitsNumber_(number)
    {
        pointerNum_ = (bitsNumber_ + POINTER_SIZE_BITS - 1) / POINTER_SIZE_BITS;
        map_ = new uintptr_t[pointerNum_];
        clearAll();
    }

    ~BitMap()
    {
        delete[] map_;
    }

    void mark(int index)
    {
        ASSERT((index / POINTER_SIZE_BITS) < pointerNum_);
        map_[index / POINTER_SIZE_BITS] |= 1 << (index % POINTER_SIZE_BITS);
        return;
    }

    void clear(int index)
    {
        ASSERT((index / POINTER_SIZE_BITS) < pointerNum_);
        map_[index / POINTER_SIZE_BITS] &= ~(1 << (index % POINTER_SIZE_BITS));
        return;
    }

    int find()
    {
        for (int i = 0; i < bitsNumber_; i++) {
            if (!marked(i)) {
                mark(i);
                return i;
            }
        }
        return -1;
    }

    int find(int num)
    {
        for (int i = 0; i < bitsNumber_; i++) {
            if (!marked(i)) {
                int size = 0;
                for (int j  = i; j < bitsNumber_; j++) {
                    if (marked(j)) {
                        break;
                    }
                    size++;
                    if (size == num) {
                        for (int k = i; k <= j; k++) {
                            mark(k);
                        }
                        return i;
                    }
                }
            }
        }
        return -1;
    }

    int countClear()
    {
        int count = 0;
        for (int i = 0; i < bitsNumber_; i++) {
            if (!marked(i)) count++;
        }

        return count;
    }

    bool marked(int index)
    {
        ASSERT((index / POINTER_SIZE_BITS) < pointerNum_);
        return(map_[index / POINTER_SIZE_BITS] & (1 << (index % POINTER_SIZE_BITS)));
    }

    int getBitsNumber() const
    {
        return bitsNumber_;
    }

private:
    enum
    {
        POINTER_SIZE_BITS = sizeof(uintptr_t) * 8
    };

    void clearAll()
    {
        memset(map_, 0, sizeof(uintptr_t) * pointerNum_);
    }

    uintptr_t* map_;
    int bitsNumber_;
    int pointerNum_;
};

#endif
