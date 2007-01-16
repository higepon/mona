#include "Overlap.h"
#include "screen.h"

inline void swap(int& a, int& b)
{
	int c = a;
	a = b;
	b = c;
}

using namespace MonAPI;

extern guiserver_bitmap* screen_buffer;

Overlap::Overlap(int x, int y, int width, int height) : x(x), y(y), width(width), height(height)
{
	this->Draw();
}

Overlap::~Overlap()
{
	this->Draw();
}

void Overlap::Move(int x, int y, int width, int height)
{
	if (x == this->x && y == this->y && width == this->width && height == this->height) return;
	
	this->Draw(0, 0, -1, -1);
	
	swap(this->x, x);
	swap(this->y, y);
	swap(this->width , width );
	swap(this->height, height);
	this->Draw(0, 0, -1, -1);
	
	DrawScreen(x, y, 1, height);
	DrawScreen(x + width - 1, y, 1, height);
	DrawScreen(x + 1, y, width - 2, 1);
	DrawScreen(x + 1, y + height - 1, width - 2, 1);
	
	DrawScreen(this->x, this->y, 1, this->height);
	DrawScreen(this->x + this->width - 1, this->y, 1, this->height);
	DrawScreen(this->x + 1, this->y, this->width - 2, 1);
	DrawScreen(this->x + 1, this->y + this->height - 1, this->width - 2, 1);
}

void Overlap::Draw()
{
	this->Draw(0, 0, -1, -1);

	DrawScreen(this->x, this->y, 1, this->height);
	DrawScreen(this->x + this->width - 1, this->y, 1, this->height);
	DrawScreen(this->x + 1, this->y, this->width - 2, 1);
	DrawScreen(this->x + 1, this->y + this->height - 1, this->width - 2, 1);
}

void Overlap::Draw(int rx, int ry, int rw, int rh)
{
	Screen* scr = GetDefaultScreen();
	int sw = scr->getWidth(), sh = scr->getHeight(), sw4 = sw * 4;
	if (rw < 0) rw = sw;
	if (rh < 0) rh = sh;
	
	int rx2 = rx + rw - 1, ry2 = ry + rh - 1;
	int x1 = this->x, y1 = this->y;
	int x2 = this->x + this->width - 1, y2 = this->y + this->height - 1;
	
	int _x1 = x1, _y1 = y1, _x2 = x2, _y2 = y2;
	if (_x1 < rx ) _x1 = rx;
	if (_y1 < ry ) _y1 = ry;
	if (_x2 > rx2) _x2 = rx2;
	if (_y2 > ry2) _y2 = ry2;
	if (_x1 == x1) _x1++;
	if (_x2 == x2) _x2--;
	
	// left line
	if (rx <= x1 && x1 <= rx2)
	{
		uint8_t* p = (uint8_t*)&screen_buffer->Data[x1 + _y1 * sw];
		for (int y = _y1; y <= _y2; y++, p += sw4)
		{
			p[0] = ~p[0];
			p[1] = ~p[1];
			p[2] = ~p[2];
		}
	}
	
	// right line
	if (rx <= x2 && x2 <= rx2)
	{
		uint8_t* p = (uint8_t*)&screen_buffer->Data[x2 + _y1 * sw];
		for (int y = _y1; y <= _y2; y++, p += sw4)
		{
			p[0] = ~p[0];
			p[1] = ~p[1];
			p[2] = ~p[2];
		}
	}
	
	// top line
	if (ry <= y1 && y1 <= ry2)
	{
		uint8_t* p = (uint8_t*)&screen_buffer->Data[_x1 + y1 * sw];
		for (int x = _x1; x <= _x2; x++, p += 4)
		{
			p[0] = ~p[0];
			p[1] = ~p[1];
			p[2] = ~p[2];
		}
	}
	
	// bottom line
	if (ry <= y2 && y2 <= ry2)
	{
		uint8_t* p = (uint8_t*)&screen_buffer->Data[_x1 + y2 * sw];
		for (int x = _x1; x <= _x2; x++, p += 4)
		{
			p[0] = ~p[0];
			p[1] = ~p[1];
			p[2] = ~p[2];
		}
	}
}
