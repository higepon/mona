// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

// MonaGUI発祥の地。
// すべてはここから始まった。

#include <monapi.h>
using namespace MonAPI;

const char* kan[] = {
	"@...@..@....",
	".@@@@@@@@@@.",
	"....@..@....",
	"...@@@@@@@..",
	"@..@..@..@..",
	".@.@@@@@@@..",
	"......@.....",
	"..@@@@@@@@..",
	".@....@.....",
	".@.@@@@@@@..",
	"@....@.@@...",
	"@.@@@....@@.",
	NULL
};

void MonaDrawString(Screen* scr, int x, int y, dword fore, dword back, const char** image)
{
	for (; *image != NULL; image++, y++)
	{
		int xx = x;
		for (const char* line = *image; *line != '\0'; line++, xx++)
		{
			scr->putPixel16(xx, y, *line == '@' ? fore : back);
		}
	}
}

int MonaMain(List<char*>* pekoe)
{
	Screen scr;
	for (int i = 0; i < 100; i += 12)
	{
		MonaDrawString(&scr, i, i, 0xff0000, 0xffffff, kan);
	}
    return 0;
}
