#ifndef __UTIL_STRING_H__
#define __UTIL_STRING_H__

#ifdef USE_BOEHM_GC
#include "gc_cpp.h"
#include "gc_allocator.h"
extern bool g_gc_initialized;
#else
#include "../gc3/gc.h"
#endif
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "Assert.h"

namespace util {

/*----------------------------------------------------------------------
    String
----------------------------------------------------------------------*/
#ifdef USE_BOEHM_GC
class String : public gc_cleanup
#else
class String
#endif
{
public:
    enum
    {
        BUFFER_SIZE = 256,
    };

    String(const char* text) : extendSize_(8)
    {
        set(text);
    }

    String(const String& text) : extendSize_(8)
    {
        set(text.data());
    }

    String() : extendSize_(8)
    {
        set("");
    }
    ~String()
    {
    }

    uint32_t size() const { return length_; }
    char operator [](uint32_t index) const
    {
        ASSERT_UTIL(index < length_);
        return data_[index];
    }

    char at(uint32_t index) const
    {
        ASSERT_UTIL(index < length_);
        return data_[index];
    }

    void set(uint32_t index, char c)
    {
        ASSERT_UTIL(index < length_);
        data_[index] = c;
    }

    const char* data() const { return data_; }

    void operator +=(const String& s)
    {
        uint32_t length  = length_;
        uint32_t slength = s.size();
        length_ += slength;
        if (bufferSize_ >= length_)
        {
            for (uint32_t i = 0; i < slength; i++) data_[i + length] = s[i];
            data_[length_] = '\0';
        }
        else
        {
            char* tmp = data_;
            bufferSize_ = length_ + 1 + extendSize_;
            extendSize_ = (uint32_t)(extendSize_ * 1.5);
#ifdef USE_BOEHM_GC
            data_ = new(GC) char[bufferSize_];
#else
#ifdef USE_MONA_GC
            data_ = new(false) char[bufferSize_];
#else
            data_ = new char[bufferSize_];
#endif
#endif
            for (uint32_t i = 0; i < length; i++) data_[i] = tmp[i];
            for (uint32_t i = 0; i < slength; i++) data_[i + length] = s[i];
            data_[length + slength] = '\0';
        }
    }

    String operator +(const String& s)
    {
        String ret = *this;
        ret += s;
        return ret;
    }

    void operator +=(char ch)
    {
        uint32_t length  = length_;
        length_ += 1;
        if (bufferSize_ >= length_)
        {
            data_[length] = ch;
            data_[length + 1] = '\0';
        }
        else
        {
            char* tmp = data_;
            bufferSize_ = length_ + 1 + extendSize_;
            extendSize_ = (uint32_t)(extendSize_ * 1.5);
#ifdef USE_BOEHM_GC
            data_ = new(GC) char[bufferSize_];
#else
#ifdef USE_MONA_GC
            data_ = new(false) char[bufferSize_];
#else
            data_ = new char[bufferSize_];
#endif
#endif
            for (uint32_t i = 0; i < length; i++) data_[i] = tmp[i];
            data_[length] = ch;
            data_[length + 1] = '\0';
        }
    }


    bool operator ==(const String& s) const
    {
        uint32_t length = size();
        if (length != s.size()) return false;

        for (uint32_t i = 0; i < length; i++)
        {
            if (at(i) != s[i]) return false;
        }
        return true;
    }

    bool operator !=(const String& text) { return !((*this) == text); }

    bool include(String& text)
    {
        return strstr(data(), text.data()) != NULL;
    }
    bool startWith(const char* text)
    {
        return strstr(data(), text) == data();
    }

    bool include(const char* text)
    {
        return strstr(data(), text) != NULL;
    }

    int indexOf(const char* text)
    {
        char* found = strstr(data(), text);
        if (NULL == found) return - 1;
        return found - data();
    }

    int indexOf(const String& text)
    {
        return indexOf(text.data());
    }

    int replaceOnce(const String& a, const String& b)
    {
        int index = indexOf(a);
        if (-1 == index) return -1;
        int asize = a.size();
        int bsize = b.size();
        int diffLength = bsize - asize;
        char* prevData = data_;
        uint32_t prevLength = size();
        if (diffLength > 0)
        {
            length_ = prevLength + diffLength;
            bufferSize_ = length_ + 1 + extendSize_;
            extendSize_ = (uint32_t)(extendSize_ * 1.5);
#ifdef USE_BOEHM_GC
            data_ = new(GC) char[bufferSize_];
#elifdef USE_MONA_GC
            data_ = new(false) char[bufferSize_];
#else
            data_ = new char[bufferSize_];
#endif
            memcpy(data_, prevData, index);
            memcpy(&data_[index], b.data(), bsize);
            memcpy(&data_[index + bsize], &prevData[index + asize], prevLength - index - asize);
        }
        else
        {
            length_ = prevLength + diffLength;
            memcpy(&data_[index], b.data(), bsize);
            memmove(&data_[index + bsize], &data_[index + asize], prevLength - index - asize);
            data_[length_] = '\0';
        }
        return index;
    }

    int replace(const String& a, const String& b)
    {
        while (replaceOnce(a, b) != -1);
        return -1;
    }

private:
    void set(const char* text)
    {
        length_ = strlen(text);
        bufferSize_ = length_ + 1 + extendSize_;
#ifdef USE_BOEHM_GC
        data_ = new(GC) char[bufferSize_];
#elifdef USE_MONA_GC
        data_ = new(false) char[bufferSize_];
#else
        data_ = new char[bufferSize_];
#endif
        strcpy(data_, text);
    }

protected:
    char* data_;
    uint32_t length_;
    uint32_t bufferSize_;
    uint32_t extendSize_;

};

inline String operator +(const char* text1, const String& text2)
{
    String ret = text1;
    ret += text2;
    return ret;
}

};
#endif // __UTIL_STRING_H__
