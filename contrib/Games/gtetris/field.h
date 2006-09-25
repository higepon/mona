#ifndef FIELD__
#define FIELD__
#include <baygui.h>

//using namespace MonAPI;

class Field
{
private:
	int field[26][16];
	int drawFlg[22][10];
	int dropLine[22];
public:
	Field()
	{
		int field[26][16] = {
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,0,0,0,0,0,0,0,0,0,0,9,9,9},
								{9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9},
								{9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9},
								{9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9}
							};
		for (int i=0; i<26; i++)
		{
			for (int j=0; j<16; j++)
			{
				this->field[i][j] = field[i][j];
			}
		}
		for (int i=0; i<22; i++)
		{
			for (int j=0; j<10; j++)
			{
				this->drawFlg[i][j] = 0;
			}
			this->dropLine[i] = 0;
		}
	}

	void reset();
	void drawField(Graphics* g);
	void renewField();
	void drawAllField(Graphics* g);
	void set(int x, int y, int type);
	void erase(int x, int y);
	int getField(int x, int y)
	{
		return field[y+1][x];
	}
	int getFlg(int x, int y)
	{
		if (x < 0 || x > 9 || y < 0 || y > 21)
		{
			return 0;
		}
		return drawFlg[y][x];
	}
	int checkLine(Graphics* g);
	void eraseLine(int line);
	void eraseLines(Graphics* g, int* lines);
	void drop();
};

#endif