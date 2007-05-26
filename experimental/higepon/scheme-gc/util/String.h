#ifndef __UTIL_STRING_H__
#define __UTIL_STRING_H__

#ifdef USE_BOEHM_GC
#include "gc_cpp.h"
#include "gc_allocator.h"
extern bool g_gc_initialized;
#elif defined(USE_MONA_GC)
#include "../gc3/gc.h"
#endif
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "Vector.h"
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

    String(const char* text) : data_(NULL), extendSize_(8)
    {
        set(text);
    }

    String(const char* text, uint32_t length) : data_(NULL), extendSize_(8)
    {
        set(text, length);
    }

    String(const String& text) : data_(NULL), extendSize_(8)
    {
        set(text.data());
    }

    String() : data_(NULL), extendSize_(8)
    {
        set("");
    }
    ~String()
    {
#ifdef USE_BOEHM_GC
#elif defined(USE_MONA_GC)
#else
        if (NULL != data_) delete [] data_;
#endif

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

    void insert(uint32_t index, char c)
    {
        ASSERT_UTIL(index <= length_);
        ASSERT_UTIL(index >= 0);

        if (bufferSize_ < length_ + 1 + 1)
        {
            char* tmp = data_;
            bufferSize_ = length_ + 1 + 1 + extendSize_;
            extendSize_ = (uint32_t)(extendSize_ * 1.5);
#ifdef USE_BOEHM_GC
            data_ = new(GC) char[bufferSize_];
#elif defined(USE_MONA_GC)
            data_ = new(false) char[bufferSize_];
#else
            data_ = new char[bufferSize_];
#endif
            for (uint32_t i = 0; i <= length_; i++) data_[i] = tmp[i];
#ifdef USE_BOEHM_GC
#elif defined(USE_MONA_GC)
#else
           delete[] tmp;
#endif
        }
        memmove(&data_[index + 1], &data_[index], length_ - index + 1);
        data_[index] = c;
        length_++;
    }

    void removeAt(uint32_t index)
    {
        ASSERT_UTIL(index < length_);
        if (index == length_ - 1)
        {
            data_[index] = '\0';
        }
        else
        {
            memmove(&data_[index], &data_[index + 1], length_ - index);
        }
        length_--;
    }

    const char* data() const { return data_; }

    void operator +=(const String& s)
    {
        uint32_t length  = length_;
        uint32_t slength = s.size();
        length_ += slength;
        if (bufferSize_ >= length_ + 1)
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
#elif defined(USE_MONA_GC)
            data_ = new(false) char[bufferSize_];
#else
            data_ = new char[bufferSize_];
#endif
            for (uint32_t i = 0; i < length; i++) data_[i] = tmp[i];
            for (uint32_t i = 0; i < slength; i++) data_[i + length] = s[i];
            data_[length + slength] = '\0';
#ifdef USE_BOEHM_GC
#elif defined(USE_MONA_GC)
#else
           delete[] tmp;
#endif
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
        if (bufferSize_ >= length_ + 1)
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
#elif defined(USE_MONA_GC)
            data_ = new(false) char[bufferSize_];
#else
            data_ = new char[bufferSize_];
#endif
            for (uint32_t i = 0; i < length; i++) data_[i] = tmp[i];
            data_[length] = ch;
            data_[length + 1] = '\0';
#ifdef USE_BOEHM_GC
#elif defined(USE_MONA_GC)
#else
            delete[] tmp;
#endif
        }
    }

    String& operator =(const char* text)
    {
        set(text);
        return *this;
    }

    String& operator =(const String& text)
    {
        set(text.data());
        return *this;
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

    int indexOf(char c)
    {
        return indexOf(c, 0);
    }

    int indexOf(char c, int start)
    {
        int len = size();
        const char* p = data();
        for (int i = start; i < len; i++)
        {
            if (p[i] == c) return i;
        }
        return -1;
    }


    bool isEmpty() const
    {
        return size() == 0;
    }

    int countChar(char c) const
    {
        int len = size();
        int count = 0;
        const char* p = data();
        for (int i = 0; i < len; i++)
        {
            if (p[i] == c) count++;
        }
        return count;
    }

    int indexOf(const String& text)
    {
        return indexOf(text.data());
    }

    char last()
    {
        return data_[length_ - 1];
    }

    char first()
    {
        return data_[0];
    }

    void chop()
    {
        if (length_ == 0) return;
        data_[length_ - 1] = '\0';
        length_--;
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
#elif defined(USE_MONA_GC)
            data_ = new(false) char[bufferSize_];
#else
            data_ = new char[bufferSize_];
#endif
            if (index != 0) memcpy(data_, prevData, index);
            memcpy(&data_[index], b.data(), bsize);
            memcpy(&data_[index + bsize], &prevData[index + asize], prevLength - index - asize + 1);
#ifdef USE_BOEHM_GC
#elif defined(USE_MONA_GC)
#else
            delete [] prevData;
#endif
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

    void replace(const String& a, const String& b)
    {
        while (replaceOnce(a, b) != -1);
        return;
    }


    String substring(int start, int length) const
    {
        if (start < 0 || (int)size() <= start || length < 1) return String("");
        int len = size() - start;
        if (length > len) length = len;

        return String(&data_[start], length);
    }

    Vector<String>* split(char ch)
    {
        Vector<String>* ret = new Vector<String>;
        int p = 0;
        int pp = 0;
        for (;;)
        {
            pp = p;
            p = indexOf(ch, p);
            if (p < 0)
            {
                ret->add(substring(pp, size() - pp));
                break;
            }
            ret->add(substring(pp, p - pp));
            p++;
        }
        return ret;
    }


private:
    void set(const char* text)
    {
        length_ = strlen(text);
        bufferSize_ = length_ + 1 + extendSize_;
#ifdef USE_BOEHM_GC
        data_ = new(GC) char[bufferSize_];
#elif defined(USE_MONA_GC)
        data_ = new(false) char[bufferSize_];
#else
        if (data_ != NULL) delete[] data_;
        data_ = new char[bufferSize_];
#endif
        strcpy(data_, text);
    }

    void set(const char* text, uint32_t length)
    {
        length_ = length;
        bufferSize_ = length_ + 1 + extendSize_;
#ifdef USE_BOEHM_GC
        data_ = new(GC) char[bufferSize_];
#elif defined(USE_MONA_GC)
        data_ = new(false) char[bufferSize_];
#else
        if (data_ != NULL) delete[] data_;
        data_ = new char[bufferSize_];
#endif
        strncpy(data_, text, length_);
        data_[length_] = '\0';
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
