// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <monapi.h>
#include <gui/System/Drawing/Graphics.h>
#include <gui/System/Drawing/Font.h>
#include <gui/System/Math.h>

using namespace System;

namespace System { namespace Drawing
{
	Graphics::Graphics(_P<Bitmap> image)
	{
		this->image = image;
	}
	
	void Graphics::SetPixel(int x, int y, Color c)
	{
		int w = this->clientRectangle.Width, h = this->clientRectangle.Height;
		if ((w != 0 || h != 0) && (x >= w || y >= h)) return;
		this->image->SetPixel(x + this->clientRectangle.X, y + this->clientRectangle.Y, c);
	}
	
	_P<Graphics> Graphics::FromImage(_P<Bitmap> image)
	{
		return new Graphics(image);
	}
	
	Graphics::~Graphics()
	{
		this->Dispose();
	}
	
	void Graphics::Dispose()
	{
		this->image = NULL;
	}
	
	void Graphics::DrawLine(Color c, int x1, int y1, int x2, int y2)
	{
		int dx = Math::Abs(x1 - x2), dy = Math::Abs(y1 - y2);
		if (dx == 0 && dy == 0)
		{
			this->SetPixel(x1, y1, c);
		}
		else if (dx >= dy)
		{
			if (x1 > x2)
			{
				int tx = x1; x1 = x2; x2 = tx;
				int ty = y1; y1 = y2; y2 = ty;
			}
			for (int x = x1; x <= x2; x++)
			{
				this->SetPixel(x, ((2 * y1 + 2 * (y2 - y1) * (x - x1) / (x2 - x1)) + 1) / 2, c);
			}
		}
		else
		{
			if (y1 > y2)
			{
				int tx = x1; x1 = x2; x2 = tx;
				int ty = y1; y1 = y2; y2 = ty;
			}
			for (int y = y1; y <= y2; y++)
			{
				this->SetPixel((2 * x1 + 2 * (x2 - x1) * (y - y1) / (y2 - y1) + 1) / 2, y, c);
			}
		}
	}
	
	void Graphics::DrawRectangle(Color c, int x, int y, int width, int height)
	{
		if (width  < 0) { x += width ; width  = -width ; }
		if (height < 0) { y += height; height = -height; }
		int xw = x + width - 1, yh = y + height - 1;
		for (int xx = x; xx <= xw; xx++)
		{
			this->SetPixel(xx, y , c);
			this->SetPixel(xx, yh, c);
		}
		for (int yy = y + 1; yy <= yh - 1; yy++)
		{
			this->SetPixel(x , yy, c);
			this->SetPixel(xw, yy, c);
		}
	}
	
	void Graphics::FillRectangle(Color c, int x, int y, int width, int height)
	{
		if (width  < 0) { x += width ; width  = -width ; }
		if (height < 0) { y += height; height = -height; }
		int xw = x + width, yh = y + height;
		for (int yy = y; yy < yh; yy++)
		{
			for (int xx = x; xx < xw; xx++)
			{
				this->SetPixel(xx, yy, c);
			}
		}
	}
	
	/// buggy
	void Graphics::DrawEllipse(Color c, int x, int y, int width, int height)
	{
		if (width  < 0) { x += width ; width  = -width ; }
		if (height < 0) { y += height; height = -height; }
		int xw = x + width, yh = y + height;
		int r1 = (width - 1) * (height - 1) * (width - 1) * (height - 1);
		int r2 = width * height * width * height;
		for (int yy = y; yy < yh; yy++)
		{
			for (int xx = x; xx < xw; xx++)
			{
				int _x = ((xx - x) * 2 + 1 - width) * height, _y = ((yy - y) * 2 + 1 - height) * width;
				int d = _x * _x + _y * _y;
				if (r1 < d && d <= r2) this->SetPixel(xx, yy, c);
			}
		}
	}
	
	void Graphics::FillEllipse(Color c, int x, int y, int width, int height)
	{
		if (width  < 0) { x += width ; width  = -width ; }
		if (height < 0) { y += height; height = -height; }
		int xw = x + width, yh = y + height;
		int r2 = width * height * width * height;
		for (int yy = y; yy < yh; yy++)
		{
			for (int xx = x; xx < xw; xx++)
			{
				int _x = ((xx - x) * 2 + 1 - width) * height, _y = ((yy - y) * 2 + 1 - height) * width;
				int d = _x * _x + _y * _y;
				if (d <= r2) this->SetPixel(xx, yy, c);
			}
		}
	}
	
	void Graphics::DrawString(String text, _P<Font> font, Color c, int x, int y)
	{
		font->Draw(this, text, c, x, y);
	}
	
	Size Graphics::MeasureString(String text, _P<Font> font)
	{
		return font->GetSize(text);
	}
}}
