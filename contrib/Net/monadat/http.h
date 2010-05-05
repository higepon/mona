/*
 * http.h - 
 *
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: http.h 261 2008-07-25 06:16:44Z higepon $
 */

#ifndef MONADAT_HTTP_
#define MONADAT_HTTP_

class Buffer
{
public:
    Buffer(int capcity) : pointer_(0), capcity_(capcity), size_(0)
    {
        start_ = new uint8_t[capcity];
    }

    ~Buffer()
    {
        delete[] start_;
    }

    void append(const void* src, int sizeToAppend)
    {
        if (size_ + sizeToAppend > capcity_) {
            expand(sizeToAppend);
        }
        memcpy(&start_[size_], src, sizeToAppend);
        size_ += sizeToAppend;
    }

    int forwardPointer(int offset)
    {
        if (pointer_ + offset < size_) {
            pointer_ += offset;
            return 0;
        } else {
            return -1;
        }
    }

    int size() const
    {
        return size_ - pointer_;
    }

    const uint8_t* get()
    {
        return start_ + pointer_;
    }

private:

    void expand(int requiredExtraSize)
    {
        int newCapacity = capcity_ + requiredExtraSize * 2;
        uint8_t* newStart = new uint8_t[newCapacity];
        memcpy(newStart, start_, size_);
        start_ = newStart;
        capcity_ = newCapacity;
    }
    uint8_t* start_;
    int pointer_;
    int capcity_;
    int size_;
};

Buffer* http_get(const char* host, const char* path);

#endif // MONADAT_HTTP_
