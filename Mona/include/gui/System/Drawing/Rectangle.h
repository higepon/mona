// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_RECTANGLE_H__
#define __SYSTEM_DRAWING_RECTANGLE_H__

#include <gui/System/Drawing/Point.h>
#include <gui/System/Drawing/Size.h>

namespace System { namespace Drawing
{
	struct Rectangle
	{
		int X, Y, Width, Height;
		
		Rectangle() : X(0), Y(0), Width(0), Height(0) {}
		Rectangle(int x, int y, int width, int height) : X(x), Y(y), Width(width), Height(height) {}
		Rectangle(const Point& p, const Size& sz) : X(p.X), Y(p.Y), Width(sz.Width), Height(sz.Height) {}
		
		void Intersect(const Rectangle& r);
		
		inline bool IntersectsWith(Rectangle r)
		{
			r.Intersect(*this);
			return r.Width != 0 && r.Height != 0;
		}
		
		inline int get_Left() const { return this->X; }
		inline int get_Top() const { return this->Y; }
		inline int get_Right() const { return this->X + this->Width; }
		inline int get_Bottom() const { return this->Y + this->Height; }
		inline Point get_Location() const { return Point(this->X, this->Y); }
		inline Size get_Size() const { return Size(this->Width, this->Height); }
		
		inline bool get_IsEmpty() const
		{
			return this->X == 0 && this->Y == 0 && this->Width == 0 && this->Height == 0;
		}
		
		inline void Offset(int dx, int dy) { this->X += dx; this->Y += dy; }
		inline void Offset(const Point& p) { this->X += p.X; this->Y += p.Y; }
		
		inline bool Contains(int x, int y)
		{
			return this->X <= x && x < this->X + this->Width && this->Y <= y && y < this->Y + this->Height;
		}
		inline bool Contains(Point pt) { return this->Contains(pt.X, pt.Y); }
	};
}}

#endif  // __SYSTEM_DRAWING_RECTANGLE_H__
