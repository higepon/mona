#ifndef __MONAPI_CSTRING_H__
#define __MONAPI_CSTRING_H__

namespace MonAPI
{
    struct CString
    {
    protected:
        char* buffer;
        int length;

    public:
        CString();
        CString(const char* text, int length = -1);
        CString(const CString& text);
        virtual ~CString();

        int getLength() const { return this->length; }
        char operator [](int index) const;
        operator const char*() const { return this->buffer; }

        bool operator ==(const char* text) const;
        bool operator ==(const CString& text) const;
        inline bool operator !=(const char* text) const { return *this != text; }
        inline bool operator !=(const CString& text) const { return *this != text; }
        CString& operator =(const char* text);
        CString& operator =(const CString& text);
        void operator +=(const char* text);
        void operator +=(const CString& text);
        void operator +=(char ch);
        CString operator +(const char* text) const;
        CString operator +(const CString& text) const;

        bool startsWith(const CString& value) const;
        bool endsWith(const CString& value) const;

        CString substring(int start, int length) const;

        CString toLower() const;
        CString toUpper() const;
    };
}

extern MonAPI::CString operator +(const char* text1, const MonAPI::CString& text2);

#endif  // __MONAPI_CSTRING_H__
