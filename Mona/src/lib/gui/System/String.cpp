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
		this->Set(Encoding::ToWCharArray(text, length));
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
}
