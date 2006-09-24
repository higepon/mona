#ifndef TETROMINO__
#define TETROMINO__
#include <baygui.h>
#include "field.h"
#include "block.h"

//using namespace MonAPI;

class Tetromino
{
private:
	int x;
	int y;
	int turn;
	int type;
	double interval;
	int landing;
	int landed ;
	int speed;
	static int pattern[7][4];
	static int patternL[7][4];
	static int patternR[7][4];
	static int patternD[7][4];
	void trueDraw(Field* f);
public:
	Tetromino(int x, int y, int turn, int type);
	~Tetromino();
	int setNew(Field* f, int x, int y, int turn, int type);
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

	int addInterval(Field* f, double d, int* score);

	void turnRight(Field* f);
	void turnLeft(Field* f);

	void moveRight(Field* f);
	void moveLeft(Field* f);
	int moveDown(Field* f);

	void draw(Field* f);
	void erase(Field* f);

	int checkGameOver(Field* f);

	static void drawNext(Graphics* g, int type, int turn);
	static void drawNext2(Graphics* g, int type, int turn);
};

#endif
