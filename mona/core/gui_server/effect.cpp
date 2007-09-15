// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <monapi/syscall.h>
#include "effect.h"
#include "Overlap.h"

extern int we_creation, we_destruction, we_step, we_wait;

void CreationEffect(guiserver_window* w)
{
	if (!w->Visible) return;

	w->Visible = false;
	switch (we_creation)
	{
		case 1:  // simple expansion
		{
			int cx = w->X + w->Width / 2, cy = w->Y + w->Height / 2;
			int dx = cx - w->X, dy = cy - w->Y;
			Overlap* prev = NULL;
			for (int i = 1; i <= we_step; i++)
			{
				int ow = dx * i / we_step, oh = dy * i / we_step;
				Overlap* ov = new Overlap(cx - ow, cy - oh, ow * 2, oh * 2);
				sleep(we_wait);
				if (prev != NULL) delete prev;
				prev = ov;
			}
			if (prev != NULL) delete prev;
			break;
		}
	}
	w->Visible = true;
	w->__internal2 = true;
}

void DestructionEffect(guiserver_window* w)
{
	if (w->Parent != 0) return;
	
	switch (we_destruction)
	{
		case 1:  // simple reduction
		{
			int cx = w->X + w->Width / 2, cy = w->Y + w->Height / 2;
			int dx = cx - w->X, dy = cy - w->Y;
			Overlap* prev = NULL;
			for (int i = we_step; i > 0; i--)
			{
				int ow = dx * i / we_step, oh = dy * i / we_step;
				Overlap* ov = new Overlap(cx - ow, cy - oh, ow * 2, oh * 2);
				sleep(we_wait);
				if (prev != NULL) delete prev;
				prev = ov;
			}
			if (prev != NULL) delete prev;
			break;
		}
		case 2:  // explosion
		{
			int cx = w->X + w->Width / 2, cy = w->Y + w->Height / 2;
			int dx = cx - w->X, dy = cy - w->Y;
			Overlap* prev[] = { NULL, NULL, NULL, NULL };
			for (int i = we_step; i > 0; i--)
			{
				int ow = dx * i / we_step, oh = dy * i / we_step;
				int sx = (we_step - i) * 8, sy = (we_step - i) * 8;
				Overlap* ov[] =
					{
						new Overlap(w->X - sx, w->Y - sy, ow, oh),
						new Overlap(w->X + w->Width + sx - ow, w->Y - sy, ow, oh),
						new Overlap(w->X - sx, w->Y + w->Height + sy - oh, ow, oh),
						new Overlap(w->X + w->Width + sx - ow, w->Y + w->Height + sy - oh, ow, oh)
					};
				sleep(we_wait);
				for (int i = 0; i < 4; i++)
				{
					if (prev[i] != NULL) delete prev[i];
					prev[i] = ov[i];
				}
			}
			for (int i = 0; i < 4; i++)
			{
				if (prev[i] != NULL) delete prev[i];
			}
			break;
		}
	}
}

void ExpansionEffect(int start_x, int start_y, int x, int y, int width, int height)
{
	int dx = x - start_x, dy = y - start_y;
	Overlap* prev = NULL;
	for (int i = 1; i <= we_step; i++)
	{
		int ox = start_x + dx * i / we_step, oy = start_y + dy * i / we_step;
		int ow = width * i / we_step, oh = height * i / we_step;
		Overlap* ov = new Overlap(ox, oy, ow, oh);
		sleep(we_wait);
		if (prev != NULL) delete prev;
		prev = ov;
	}
	if (prev != NULL) delete prev;
}

void ReductionEffect(int start_x, int start_y, int x, int y, int width, int height)
{
	int dx = x - start_x, dy = y - start_y;
	Overlap* prev = NULL;
	for (int i = we_step; i > 0; i--)
	{
		int ox = start_x + dx * i / we_step, oy = start_y + dy * i / we_step;
		int ow = width * i / we_step, oh = height * i / we_step;
		Overlap* ov = new Overlap(ox, oy, ow, oh);
		sleep(we_wait);
		if (prev != NULL) delete prev;
		prev = ov;
	}
	if (prev != NULL) delete prev;
}
