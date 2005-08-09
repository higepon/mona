// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_POINT_H__
#define __SYSTEM_DRAWING_POINT_H__

namespace System { namespace Drawing
{
	struct Point
	{
		int X, Y;
		
		Point() : X(0), Y(0) {}
		Point(int x, int y) : X(x), Y(y) {}
		
		inline Point operator +(const Point& p) { Point ret = *this; return ret += p; }
		inline Point operator -(const Point& p) { Point ret = *this; return ret -= p; }
		
		inline Point& operator +=(const Point& p)
		{
			this->X += p.X;
			this->Y += p.Y;
			return *this;
		}
		
		inline Point& operator -=(const Point& p)
		{
			this->X -= p.X;
			this->Y -= p.Y;
			return *this;
		}
		
		inline bool operator ==(const Point& p)
		{
			return this->X == p.X && this->Y == p.Y;
		}
		
		inline bool operator !=(const Point& p)
		{
			return this->X != p.X || this->Y != p.Y;
		}
		
		static Point get_Empty() { return Point(); }
	};
}}

#endif  // __SYSTEM_DRAWING_POINT_H__
