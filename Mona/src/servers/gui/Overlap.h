// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifndef __GUISERVER_OVERLAP_H__
#define __GUISERVER_OVERLAP_H__

#include <sys/types.h>
#include <gui/messages.h>

class Overlap
{
private:
	int x, y, width, height;

public:
	Overlap(int x, int y, int width, int height);
	~Overlap();
	
	void Move(int x, int y, int width, int height);
	void Draw();
	void Draw(int sx, int sy, int sw, int sh);
};

#endif  // __GUISERVER_OVERLAP_H__
