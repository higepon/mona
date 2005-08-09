// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include "Rectangle.h"
#include "Math.h"

using namespace System;

namespace System { namespace Drawing
{
	void Rectangle::Intersect(const Rectangle& r)
	{
		int x1 = this->X, x2 = this->get_Right();
		int y1 = this->Y, y2 = this->get_Bottom();
		int x3 = r.X, x4 = r.get_Right(), y3 = r.Y, y4 = r.get_Bottom();
		
		if (x3 <= x1 && x1 <= x4)
		{
			this->Width = Math::Min(this->Width, x4 - x1);
		}
		else if (x3 <= x2 && x2 <= x4)
		{
			int dx = x3 - x1;
			this->X += dx;
			this->Width = Math::Min(r.Width, this->Width - dx);
		}
		else if (x1 < x3 && x4 < x2)
		{
			int dx1 = x3 - x1;
			int dx2 = x2 - x4;
			this->X += dx1;
			this->Width -= dx1 + dx2;
		}
		else
		{
			this->X = this->Width = 0;
		}
		
		if (y3 <= y1 && y1 <= y4)
		{
			this->Height = Math::Min(this->Height, y4 - y1);
		}
		else if (y3 <= y2 && y2 <= y4)
		{
			int dy = y3 - y1;
			this->Y += dy;
			this->Height = Math::Min(r.Height, this->Height - dy);
		}
		else if (y1 < y3 && y4 < y2)
		{
			int dy1 = y3 - y1;
			int dy2 = y2 - y4;
			this->Y += dy1;
			this->Height -= dy1 + dy2;
		}
		else
		{
			this->Y = this->Height = 0;
		}
	}
}}
