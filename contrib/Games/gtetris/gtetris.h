#ifndef GTETRIS__
#define GTETRIS__
#define BLOCKNUM	7
#include <baygui.h>
#include "block.h"
#include "tetromino.h"
#include "field.h"

//using namespace MonAPI;

class GTetris : public Frame
{
private:
	Image* background;
	Tetromino* tetromino;
	Field* field;
	int next[2];
	double speed;
	MonAPI::Random random;
	int removeFlg;
	int LRPush;
	int DownPush;
	int ZXPush;
	int noPlaying;
	int land;

	int line;
	int score;
	int level;

	static int scores[4];
public:
	GTetris();
	~GTetris();
	virtual void paint(Graphics* g);
	virtual void processEvent(Event* e);
	virtual void checkLine();
	virtual void drawScores(Graphics* g);
	virtual void levelUp();
	virtual void newGame();
};

#endif
