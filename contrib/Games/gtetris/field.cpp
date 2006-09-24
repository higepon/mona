//#define DEBUG__

#include "gtetris.h"
#include "field.h"

void Field::renewField()
{
#ifdef DEBUG__
	printf("Field::drawField(Graphics*)\n");
#endif
	for (int i=0; i<22; i++)
	{
		for (int j=0; j<10; j++)
		{
			if (drawFlg[i][j] > 0)
			{
				if (drawFlg[i][j] <= 7)
				{
					field[i+1][j+3] = drawFlg[i][j];
				}
			}
			else if (drawFlg[i][j] < 0)
			{
				field[i+1][j+3] = 0;
			}
			drawFlg[i][j] = 0;
		}
	}
}

void Field::drawField(Graphics* g)
{
#ifdef DEBUG__
	printf("Field::drawField(Graphics*)\n");
#endif
	for (int i=0; i<22; i++)
	{
		for (int j=0; j<10; j++)
		{
			if (drawFlg[i][j] > 0)
			{
				Block::draw(g, j, i, (drawFlg[i][j]-1) % 7);
				if (drawFlg[i][j] <= 7)
				{
					field[i+1][j+3] = drawFlg[i][j];
				}
			}
			else if (drawFlg[i][j] < 0)
			{
				Block::erase(g, j, i);
				field[i+1][j+3] = 0;
			}
			drawFlg[i][j] = 0;
		}
	}
}

void Field::drawAllField(Graphics* g)
{
	for (int i=0; i<22; i++)
	{
		for (int j=0; j<10; j++)
		{
			if (field[i+1][j+3] > 0)
			{
				Block::draw(g, j, i, field[i+1][j+3]-1);
			}
			else if (drawFlg[i][j] > 0)
			{
				Block::draw(g, j, i, drawFlg[i][j]-1);
				field[i+1][j+3] = drawFlg[i][j];
			}
			else if (field[i+1][j+3] == 0 || drawFlg[i][j] < 0)
			{
				Block::erase(g, j, i);
				field[i+1][j+3] = 0;
			}
			drawFlg[i][j] = 0;
		}
	}
}

void Field::set(int x, int y, int type)
{
//	if(y < 0){
//		return ;
//	}
#ifdef DEBUG__
	printf("Field::set(%d, %d, %d); field[y+1][x+3]: %d\n", x,y,type, field[y+1][x+3]);
#endif
	if (field[y+1][x+3] != type || drawFlg[y][x] != type)
	{
		drawFlg[y][x] = type;
	}
}

void Field::erase(int x, int y)
{
//	if(y < 0){
//		return ;
//	}
#ifdef DEBUG__
	printf("Field::erase(%d, %d); field[y+1][x+3]: %d\n", x,y, field[y+1][x+3]);
#endif
//	if(field[y+1][x+3] > 0){
	drawFlg[y][x] = -1;
//	}
}

void Field::reset()
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

// è¡Ç¶ÇΩÉâÉCÉìêîÇï‘Ç∑
int Field::checkLine(Graphics* g)
{
	int line = 0;
	int eraselines[4] = {-1, -1, -1, -1};
	for (int i=0; i<22; i++)
	{
		int flg = 1;
		for (int j=0; j<10; j++)
		{
			if (field[i+1][j+3] == 0)
			{
				flg = 0;
				break;
			}
		}
		if (flg == 1)
		{
			eraselines[line] = i;
//
			line++;
		}
	}

//	printf("eraseline: %d\n", line);
	if (line == 0)
	{
		return 0;
	}
	else
	{
		eraseLines(g, eraselines);
		return line;
	}
}

void Field::eraseLine(int line)
{
	if (line < 0)
	{
		return;
	}
	for (int j=0; j<10; j++)
	{
		drawFlg[line][j] = -1;
	}
	for (int i=0; i<line; i++)
	{
		dropLine[i]++;
	}
}

void Field::eraseLines(Graphics* g, int* lines)
{
	for (int i=0; i<4; i++)
	{
		eraseLine(lines[i]);
	}
	drawField(g);
}

void Field::drop()
{
	for (int i=21; i>=0; i--)
	{
		if (dropLine[i] != 0)
		{
			for (int j=0; j<10; j++)
			{
				if (field[i+1][j+3] != 0)
				{
//					printf("erase(%d, %d); set(%d, %d, %d);\n", j, i, j, i+dropLine[i], field[i+1][j+3]);
					erase(j, i);
					set(j, i+dropLine[i], field[i+1][j + 3]);
				}
			}
			dropLine[i] = 0;
		}
	}
}

