// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <gui/System/Drawing/Font.h>
#include <gui/System/Drawing/Graphics.h>

#ifndef NULL
#define NULL 0
#endif

namespace System { namespace Drawing
{
	/*internal*/ class FontChar
	{
	public:
		wchar Code;
		unsigned char Width, Height;
		unsigned char Data[];
		
		inline int get_Size()
		{
			return (((int)this->Width) * ((int)this->Height) + 7) / 8;
		}
	};
	
	Font::Font(unsigned char* fontData) : table(65536)
	{
		for (int i = 0; i < 65536; i++) table[i] = NULL;
		minimum.Width = minimum.Height = -1;
		this->fontData = fontData;
		int len = *(int*)fontData;
		FontChar* fc;
		for (int i = 0, idx = 4; i < len; i++)
		{
			fc = (FontChar*)&fontData[idx];
			if (minimum.Width  == -1 || minimum.Width  > fc->Width ) minimum.Width  = fc->Width;
			if (minimum.Height == -1 || minimum.Height > fc->Height) minimum.Height = fc->Height;
			if (maximum.Width  < fc->Width ) maximum.Width  = fc->Width;
			if (maximum.Height < fc->Height) maximum.Height = fc->Height;
			if (fc->Code < 65536) table[fc->Code] = fc;
			idx += 6 + fc->get_Size();
		}
	}
	
	Font::~Font()
	{
	}
	
	void Font::Draw(_P<Graphics> g, String text, Color c, int x, int y)
	{
		int xx = x, yy = y;
		FOREACH(wchar, ch, text)
		{
			if (ch == '\n')
			{
				xx = x;
				yy += this->get_Height();
			}
			else
			{
				FontChar* fc = this->GetFontChar(ch);
				if (fc == NULL) fc = this->GetFontChar('?');
				if (fc != NULL)
				{
					this->Draw(g, fc, c, xx, yy);
					xx += fc->Width;
				}
			}
		}
	}
	
	void Font::Draw(_P<Graphics> g, wchar ch, Color c, int x, int y)
	{
		FontChar* fc = this->GetFontChar(ch);
		if (fc == NULL) fc = this->GetFontChar('?');
		this->Draw(g, fc, c, x, y);
	}
	
	Size Font::GetSize(String text)
	{
		Size ret;
		int x = 0, y = 0, by = this->get_Height();
		FOREACH(wchar, ch, text)
		{
			if (ch == '\n')
			{
				x = 0;
				y += this->get_Height();
				by += this->get_Height();
			}
			else
			{
				FontChar* fc = this->GetFontChar(ch);
				if (fc == NULL) fc = this->GetFontChar('?');
				if (fc != NULL) x += fc->Width;
			}
			if (ret.Width  < x ) ret.Width  = x;
			if (ret.Height < by) ret.Height = by;
		}
		return ret;
	}
	
	Size Font::GetSize(wchar ch)
	{
		Size ret;
		FontChar* fc = this->GetFontChar(ch);
		if (fc == NULL) fc = this->GetFontChar('?');
		if (fc != NULL)
		{
			ret.Width  = fc->Width;
			ret.Height = fc->Height;
		}
		return ret;
	}
	
	FontChar* Font::GetFontChar(wchar ch)
	{
		return (ch < 65536 && table[ch] != NULL) ? table[ch] : table['?'];
	}
	
	void Font::Draw(_P<Graphics> g, FontChar* fc, Color c, int x, int y)
	{
		if (fc == NULL) return;
		
		unsigned char* p = fc->Data;
		int bit = 1;
		for (int dy = 0; dy < fc->Height; dy++)
		{
			for (int dx = 0; dx < fc->Width; dx++)
			{
				if ((*p & bit) != 0) g->SetPixel(x + dx, y + dy, c);
				bit <<= 1;
				if (bit == 256)
				{
					p++;
					bit = 1;
				}
			}
		}
	}
}}
