// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/String.h>
#include <gui/System/Pointer.h>

using namespace System;
using namespace System::Text;

namespace System
{
	String::String()
	{
	}
	
	String::String(const char* text, int length /*= -1*/)
	{
		if (text == NULL)
		{
			this->Set(_A<wchar>(0));
		}
		else
		{
			this->Set(Encoding::ToWCharArray(text, length));
		}
	}
	
	String::String(_A<wchar> text)
	{
		this->Set(text);
	}
	
	String::~String()
	{
	}
	
	bool String::operator ==(String text)
	{
		int len = this->get_Length();
		if (len != text.get_Length()) return false;
		
		for (int i = 0; i < len; i++)
		{
			if ((*this)[i] != text[i]) return false;
		}
		return true;
	}
	
	String& String::operator =(const String& text)
	{
		this->Set(text);
		return *this;
	}
	
	String& String::operator =(const char* text)
	{
		this->Set(Encoding::ToWCharArray(text, -1));
		return *this;
	}
	
	void String::operator +=(const String& text)
	{
		int len1 = this->get_Length(), len2 = text.get_Length();
		_A<wchar> s(len1 + len2);
		int p = 0;
		for (int i = 0; i < len1; i++) s[p++] = (*this)[i];
		for (int i = 0; i < len2; i++) s[p++] = text[i];
		this->Set(s);
	}
	
	void String::operator +=(wchar ch)
	{
		int len = this->get_Length();
		_A<wchar> s(len + 1);
		int p = 0;
		for (int i = 0; i < len; i++) s[p++] = (*this)[i];
		s[p] = ch;
		this->Set(s);
	}
	
	String String::operator +(const String& text)
	{
		String ret = *this;
		ret += text;
		return ret;
	}
	
	bool String::StartsWith(const String& value)
	{
		int len = value.get_Length();
		if (len > this->get_Length()) return false;
		for (int i = 0; i < len; i++)
		{
			if ((*this)[i] != value[i]) return false;
		}
		return true;
	}
	
	bool String::EndsWith(const String& value)
	{
		int len = value.get_Length();
		int pos = this->get_Length() - len;
		if (pos < 0) return false;
		for (int i = 0; i < len; i++)
		{
			if ((*this)[pos + i] != value[i]) return false;
		}
		return true;
	}
	
	String String::Substring(int start, int length)
	{
		int len = this->get_Length() - start;
		if (length > len) length = len;
		
		_A<wchar> s(length);
		int p = 0;
		for (int i = 0; i < length; i++) s[p++] = (*this)[start + i];
		return s;
	}
	
}

String operator +(const char* text1, const String& text2)
{
	String ret = text1;
	ret += text2;
	return ret;
}
