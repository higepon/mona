/*
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
 */

#ifndef FIRSTFIT_ALLOCATOR_
#define FIRSTFIT_ALLOCATOR_

class FirstFitAllocator {
private:
    struct Header {
        struct Header* next;
        uintptr_t sizeByte;
        uintptr_t magic;
        uintptr_t pad;
    };
    uintptr_t start_;
    uintptr_t end_;
    Header* freeList_;

    Header* getBody(Header* header)
    {
        return header + 1;
    }

    Header* getHeaderFromPointer(void* p)
    {
        return (Header*)p - 1;
    }

    uintptr_t align16(uintptr_t s)
    {
        return (s + 16 - 1) & 0xFFFFFFF0;
    }

public:
    enum {
        HEADER_SIZE = sizeof(Header),
        MAGIC = 0xDEADCAFE
    };

    FirstFitAllocator() {}

    FirstFitAllocator(uintptr_t start, uintptr_t end) :
        start_(start),
        end_(end),
        freeList_((Header*)start)
    {
        freeList_->sizeByte = end - start - HEADER_SIZE;
        freeList_->next = NULL;
        freeList_->magic = MAGIC;
    }

    // for testability
    uintptr_t getSize(void* p) const
    {
        return ((Header*)p - 1)->sizeByte;
    }

    // for testability
    uintptr_t getFreeListSize() const
    {
        uintptr_t ret = 0;
        for (Header* q = freeList_; q != NULL; q = q->next) {
            ret++;
        }
        return ret;
    }

    void* getStart() const
    {
        return (void*)start_;
    }

    void compact()
    {
        for (Header* p = freeList_; p != NULL;) {
            uintptr_t neighbor = (uintptr_t)p + HEADER_SIZE + p->sizeByte;
            if (neighbor == (uintptr_t)p->next) {
                Header* p_next_next = p->next->next;
                p->sizeByte += HEADER_SIZE + p->next->sizeByte;
                p->next = p_next_next;
                // Retry, since new p can be comacted with next neighbor.
                continue;
            } else {
                p = p->next;
            }
        }
    }

    bool free(void* p)
    {
        bool ret = free_no_compact(p);
        if (ret) {
            compact();
        }
        return ret;
    }

    bool free_no_compact(void* p)
    {
        if (NULL == p) {
            allocator_warn("try to free NULL");
            return false;
        }
        Header* h = getHeaderFromPointer(p);
        if (h->magic != MAGIC) {
            allocator_warn("magic is overwritten");
            return false;
        }
        if (freeList_ == NULL) {
            freeList_ = h;
            return true;
        // memory block should be inserted to the left
        } else if (h < freeList_) {
            h->next = freeList_;
            freeList_ = h;
            return true;
        } else {
            for (Header* q = freeList_; q != NULL; q = q->next) {
                if (q < h && h < q->next) {
                    Header* qnext = q->next;
                    q->next = h;
                    h->next = qnext;
                    return true;
                } else if (q->next == NULL && h > q) {
                    q->next = h;
                    return true;
                }
            }
        }
        allocator_warn("doulbe free-ed?");
        return false;
    }

    uintptr_t getFreeSize() const
    {
        uintptr_t ret = 0;
        for (Header* p = freeList_; p != NULL; p = p->next) {
            ret += p->sizeByte + HEADER_SIZE;
        }
        return ret;
    }

    void* allocate(uintptr_t size)
    {
        if (0 == size) {
            return NULL;
        }
        size = align16(size);
        uintptr_t wholeSizeToAlloc = size + HEADER_SIZE;
        for (Header *p = freeList_, *prev = NULL; p != NULL; prev = p, p = p->next) {
            if (size == p->sizeByte || wholeSizeToAlloc == p->sizeByte) {
                if (NULL == prev) {
                    freeList_ = p->next;
                } else {
                    prev->next = p->next;
                }
                return getBody(p);
            } else {
                if (wholeSizeToAlloc <= p->sizeByte) {
                    uintptr_t orgSizeByte = p->sizeByte;
                    p->sizeByte = size;
                    Header* rest = (Header*)((uintptr_t)p + wholeSizeToAlloc);
                    rest->next = p->next;
                    rest->sizeByte = orgSizeByte - size - HEADER_SIZE;
                    rest->magic = MAGIC;

                    if (NULL == prev) {
                        freeList_ = rest;
                    } else {
                        prev->next = rest;
                    }
                    return getBody(p);
                }
            }
        }
        return NULL;
    }

};

#endif // FIRSTFIT_ALLOCATOR_
