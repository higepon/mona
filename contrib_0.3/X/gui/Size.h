// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#ifndef __SYSTEM_DRAWING_SIZE_H__
#define __SYSTEM_DRAWING_SIZE_H__

namespace System { namespace Drawing
{
	struct Size
	{
		int Width, Height;
		
		Size() : Width(0), Height(0) {}
		Size(int width, int height) : Width(width), Height(height) {}
		
		static Size get_Empty() { return Size(); }
	};
}}

#endif  // __SYSTEM_DRAWING_SIZE_H__
