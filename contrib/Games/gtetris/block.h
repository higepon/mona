#ifndef BLOCK__
#define BLOCK__
#include <baygui.h>

//using namespace MonAPI;

class Block
{
private:
	int x;
	int y;
	int type;
public:
	static Image* imgs[7];
	Block(int x, int y, int type);
	~Block();
	void setPosition(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	int getX()
	{
		return x;
	}
	int getY()
	{
		return y;
	}
	int getType()
	{
		return type;
	}
	void draw(Graphics* g);

	static void draw(Graphics* g, int x, int y, int type);
	static void erase(Graphics* g, int x, int y);
};

#endif
