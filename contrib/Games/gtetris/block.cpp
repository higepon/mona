//#define DEBUG__

#include "gtetris.h"
#include "block.h"

Image* Block::imgs[7];

Block::Block(int x, int y, int type)
{
#ifdef DEBUG__
	printf("Block::<init>\n");
#endif
	this->x = x;
	this->y = y;
	this->type = type;
}

Block::~Block()
{
#ifdef DEBUG__
	printf("Block::<delete>\n");
#endif
}

void Block::draw(Graphics* g)
{
	g->drawImage(imgs[type], 266 + x * 16 , 10 + y * 16);
}

void Block::draw(Graphics* g, int x, int y, int type)
{
	g->drawImage(imgs[type], 266 + x * 16 , 10 + y * 16);
}

void Block::erase(Graphics* g, int x, int y)
{
	g->setColor(Color::black);
	g->fillRect(266 + x * 16 , 10 + y * 16, 16, 16);
}
