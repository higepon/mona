// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_MATH_H__
#define __SYSTEM_MATH_H__

namespace System
{
	class Math
	{
	public:
		inline static int Max(int a, int b) { return a > b ? a : b; }
		inline static int Min(int a, int b) { return a < b ? a : b; }
		inline static int Abs(int a) { return a >= 0 ? a : -a; }
		inline static int Sign(int a) { return a > 0 ? 1 : a < 0 ? -1 : 0; }
	};
}

#endif  // __SYSTEM_MATH_H__
