#include <monapi/string.h>
#include <monapi/CString.h>
#include <monapi/syscall.h>

#define ASSERT(cond) if (!cond) { printf("%s:%d: null pointer exception!\n", __FILE__, __LINE__); exit(1); }

namespace MonAPI
{
    CString::CString()
    {
        this->buffer = NULL;
        this->length = 0;
    }

    CString::CString(const char* text, int length /*= -1*/)
    {
        if (text == NULL)
        {
            this->buffer = NULL;
            this->length = 0;
        }
        else
        {
            if (length == -1) length = strlen(text);
            this->buffer = new char[length + 1];
            ASSERT(this->buffer);
            strncpy(this->buffer, text, length);
            this->buffer[length] = '\0';
        }
    }

    CString::CString(const CString& text)
    {
        this->buffer = NULL;
        this->length = 0;
        *this = text;
    }

    CString::~CString()
    {
        if (this->buffer != NULL) delete [] this->buffer;
    }

    bool CString::operator ==(const char* text)
    {
        if (this->buffer == NULL || text == NULL) return this->buffer == text;

        return strcmp(this->buffer, text) == 0;
    }

    bool CString::operator ==(const CString& text)
    {
        if (this->buffer == NULL || text.buffer == NULL) return this->buffer == text.buffer;
        if (this->length != text.length) return false;

        return strcmp(this->buffer, text.buffer) == 0;
    }

    CString& CString::operator =(const char* text)
    {
        if (this->buffer == NULL) delete [] this->buffer;
        if (text == NULL)
        {
            this->buffer = NULL;
            this->length = 0;
        }
        else
        {
            int len = strlen(text);
            this->buffer = new char[len + 1];
            ASSERT(this->buffer);
            strncpy(this->buffer, text, len);
            this->buffer[len] = '\0';
        }
        return *this;
    }

    CString& CString::operator =(const CString& text)
    {
        if (this->buffer == NULL) delete [] this->buffer;
        if (text.buffer == NULL)
        {
            this->buffer = NULL;
            this->length = 0;
        }
        else
        {
            this->length = text.length;
            this->buffer = new char[this->length + 1];
            ASSERT(this->buffer);
            strncpy(this->buffer, text.buffer, this->length);
            this->buffer[this->length] = '\0';
        }
        return *this;
    }

    void CString::operator +=(const char* text)
    {
        int len1 = this->length, len2 = text != NULL ? strlen(text) : 0;
        this->length = len1 + len2;
        char* buf;
        if (this->length == 0)
        {
            buf = NULL;
        }
        else
        {
            buf = new char[this->length + 1];
            ASSERT(buf);
            if (this->buffer != NULL) strncpy(buf, this->buffer, len1);
            if (text != NULL) strncpy(&buf[len1], text, len2);
            buf[this->length] = '\0';
        }
        if (this->buffer == NULL) delete [] this->buffer;
        this->buffer = buf;
    }

    void CString::operator +=(const CString& text)
    {
        int len1 = this->length, len2 = text.length;
        this->length = len1 + len2;
        char* buf;
        if (this->length == 0)
        {
            buf = NULL;
        }
        else
        {
            buf = new char[this->length + 1];
            ASSERT(buf);
            if (this->buffer != NULL) strncpy(buf, this->buffer, len1);
            if (text .buffer != NULL) strncpy(&buf[len1], text.buffer, len2);
            buf[this->length] = '\0';
        }
        if (this->buffer == NULL) delete [] this->buffer;
        this->buffer = buf;
    }

    void CString::operator +=(char ch)
    {
        char* buf = new char[this->length + 2];
        ASSERT(buf);
        strncpy(buf, this->buffer, this->length);
        buf[this->length++] = ch;
        buf[this->length] = '\0';
        if (this->buffer == NULL) delete [] this->buffer;
        this->buffer = buf;
    }

    CString CString::operator +(const char* text)
    {
        CString ret = *this;
        ret += text;
        return ret;
    }

    CString CString::operator +(const CString& text)
    {
        CString ret = *this;
        ret += text;
        return ret;
    }

    bool CString::startsWith(const CString& value)
    {
        int len = value.length;
        if (len > this->length) return false;
        for (int i = 0; i < len; i++)
        {
            if (this->buffer[i] != value.buffer[i]) return false;
        }
        return true;
    }

    bool CString::endsWith(const CString& value)
    {
        int len = value.length;
        int pos = this->length - len;
        if (pos < 0) return false;
        for (int i = 0; i < len; i++)
        {
            if (this->buffer[pos + i] != value.buffer[i]) return false;
        }
        return true;
    }

    CString CString::substring(int start, int length)
    {
        if (start < 0 || this->length <= start) return NULL;
        int len = this->length - start;
        if (length > len) length = len;

        return CString(&this->buffer[start], length);
    }

}

MonAPI::CString operator +(const char* text1, const MonAPI::CString& text2)
{
    MonAPI::CString ret = text1;
    ret += text2;
    return ret;
}
