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
            ASSERT(this->buffer)
            strncpy(this->buffer, text, length);
            this->buffer[length] = '\0';
            this->length = length;
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

    char CString::operator [](int index) const
    {
        ASSERT(!(index < 0 || this->length <= index))
        return this->buffer[index];
    }

    bool CString::operator ==(const char* text) const
    {
        if (this->buffer == NULL || text == NULL) return this->buffer == text;

        return strcmp(this->buffer, text) == 0;
    }

    bool CString::operator ==(const CString& text) const
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
            this->length = strlen(text);
            this->buffer = new char[this->length + 1];
            ASSERT(this->buffer)
            strncpy(this->buffer, text, this->length);
            this->buffer[this->length] = '\0';
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
            ASSERT(this->buffer)
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
            ASSERT(buf)
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
            ASSERT(buf)
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
        ASSERT(buf)
        strncpy(buf, this->buffer, this->length);
        buf[this->length++] = ch;
        buf[this->length] = '\0';
        if (this->buffer == NULL) delete [] this->buffer;
        this->buffer = buf;
    }

    CString CString::operator +(const char* text) const
    {
        CString ret = *this;
        ret += text;
        return ret;
    }

    CString CString::operator +(const CString& text) const
    {
        CString ret = *this;
        ret += text;
        return ret;
    }

    bool CString::startsWith(const CString& value) const
    {
        int len = value.length;
        if (len > this->length) return false;
        for (int i = 0; i < len; i++)
        {
            if (this->buffer[i] != value.buffer[i]) return false;
        }
        return true;
    }

    bool CString::endsWith(const CString& value) const
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

    int CString::indexOf(char ch, int from /*= 0*/) const
    {
        if (this->buffer == NULL || this->length == 0) return -1;

        if (from < 0) from = 0;
        for (int i = from; i < this->length; i++)
        {
            if (this->buffer[i] == ch) return i;
        }
        return -1;
    }

    int CString::indexOf(const CString& value, int from /*= 0*/) const
    {
        if (this->buffer == NULL) return value.buffer == NULL;
        if (this->length == 0) return -1;

        if (from < 0) from = 0;
        int last = this->length - value.length;
        if (value.buffer == NULL || value.length == 0) return from < this->length ? from : -1;
        for (int i = from; i <= last; i++)
        {
            bool ok = true;
            for (int j = 0; j < value.length; j++)
            {
                if (this->buffer[i + j] != value.buffer[j])
                {
                    ok = false;
                    break;
                }
            }
            if (ok) return i;
        }
        return -1;
    }

    int CString::lastIndexOf(char ch, int from /*= -1*/) const
    {
        if (this->buffer == NULL || this->length == 0) return -1;

        if (from == -1) from = this->length;
        if (from > this->length) from = this->length;
        for (int i = from; i > 0; i--)
        {
            if (this->buffer[i - 1] == ch) return i - 1;
        }
        return -1;
    }

    int CString::lastIndexOf(const CString& value, int from /*= -1*/) const
    {
        if (this->buffer == NULL) return value.buffer == NULL;
        if (this->length == 0) return -1;

        if (from == -1) from = this->length;
        if (from > this->length) from = this->length;
        if (value.buffer == NULL || value.length == 0) return from - 1;
        for (int i = from; i >= value.length; i--)
        {
            bool ok = true;
            for (int j = 0; j < value.length; j++)
            {
                if (this->buffer[i - j - 1] != value.buffer[value.length - j - 1])
                {
                    ok = false;
                    break;
                }
            }
            if (ok) return i - value.length;
        }
        return -1;
    }

    CString CString::substring(int start, int length) const
    {
        if (start < 0 || this->length <= start || length < 1) return NULL;
        int len = this->length - start;
        if (length > len) length = len;

        return CString(&this->buffer[start], length);
    }

    _A<CString> CString::split(char ch) const
    {
        int len = 0, p = 0, pp = 0;
        for (;;)
        {
            pp = p;
            p = this->indexOf(ch, p);
            if (p < 0)
            {
                len++;
                break;
            }
            len++;
            p++;
        }

        _A<CString> ret(len);
        p = 0;
        for (int i = 0;; i++)
        {
            pp = p;
            p = this->indexOf(ch, p);
            if (p < 0)
            {
                ret[i] = this->substring(pp, this->length - pp);
                break;
            }
            ret[i] = this->substring(pp, p - pp);
            p++;
        }
        return ret;
    }

    _A<CString> CString::split(const CString& value) const
    {
        int len = 0, p = 0, pp = 0;
        for (;;)
        {
            pp = p;
            p = this->indexOf(value, p);
            if (p < 0)
            {
                len++;
                break;
            }
            len++;
            p += value.length;
        }

        _A<CString> ret(len);
        p = 0;
        for (int i = 0;; i++)
        {
            pp = p;
            p = this->indexOf(value, p);
            if (p < 0)
            {
                ret[i] = this->substring(pp, this->length - pp);
                break;
            }
            ret[i] = this->substring(pp, p - pp);
            p += value.length;
        }
        return ret;
    }

    CString CString::toLower() const
    {
        CString ret = *this;
        for (int i = 0; i < ret.length; i++)
        {
            char ch = ret.buffer[i];
            if ('A' <= ch && ch <= 'Z') ret.buffer[i] = ch + ('a' - 'A');
        }
        return ret;
    }

    CString CString::toUpper() const
    {
        CString ret = *this;
        for (int i = 0; i < ret.length; i++)
        {
            char ch = ret.buffer[i];
            if ('a' <= ch && ch <= 'z') ret.buffer[i] = ch - ('a' - 'A');
        }
        return ret;
    }
}

MonAPI::CString operator +(const char* text1, const MonAPI::CString& text2)
{
    MonAPI::CString ret = text1;
    ret += text2;
    return ret;
}
