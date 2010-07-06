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
    BitMap(int number)
    {
        bitsNumber_ = number;
        uint32_tNumber_ = (bitsNumber_ + DWORD_BITS - 1) / DWORD_BITS;
        map_ = new uintptr_t[uint32_tNumber_];
        memset(map_, 0, sizeof(uint32_tNumber_) * uint32_tNumber_);

        for (int i = 0; i < bitsNumber_; i++) clear(i);
        return;
    }

    ~BitMap()
    {
        delete[] map_;
    }

 public:
    void mark(int index)
    {
        ASSERT((index / DWORD_BITS) < uint32_tNumber_);
        map_[index / DWORD_BITS] |= 1 << (index % DWORD_BITS);
        return;
    }

    void clear(int index)
    {
        ASSERT((index / DWORD_BITS) < uint32_tNumber_);
        map_[index / DWORD_BITS] &= ~(1 << (index % DWORD_BITS));
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
        return NOT_FOUND;
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
        return NOT_FOUND;
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
        ASSERT((index / DWORD_BITS) < uint32_tNumber_);
        return(map_[index / DWORD_BITS] & (1 << (index % DWORD_BITS)));
    }
    int getBitsNumber() const
    {
        return bitsNumber_;
    }


  public:
    enum
    {
        NOT_FOUND  = -1,
        DWORD_BITS = sizeof(uint32_t) * 8
    };

  private:
    uintptr_t* map_;
    int bitsNumber_;
    int uint32_tNumber_;
};

#endif
