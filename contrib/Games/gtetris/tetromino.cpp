//#define DEBUG__

#include <baygui.h>
#include "gtetris.h"
#include "tetromino.h"

int Tetromino::pattern[7][4] = {
								   {0x0E40, 0x4C40, 0x04E0, 0x4640}, //T
								   {0x0740, 0x6220, 0x0170, 0x2230}, //L
								   {0x0E20, 0x44C0, 0x08E0, 0x6440}, //‹tL
								   {0x0C60, 0x2640, 0x0C60, 0x2640}, //2
								   {0x0360, 0x4620, 0x0360, 0x4620}, //S
								   {0x0F00, 0x2222, 0x0F00, 0x2222}, //|
								   {0x0660, 0x0660, 0x0660, 0x0660}  //o
							   };
int Tetromino::patternL[7][4] = {
									{0x0840, 0x4840, 0x0480, 0x4440},
									{0x0440, 0x4220, 0x0140, 0x2220},
									{0x0820, 0x4480, 0x0880, 0x4440},
									{0x0840, 0x2440, 0x0840, 0x2440},
									{0x0240, 0x4420, 0x0240, 0x4420},
									{0x0800, 0x2222, 0x0800, 0x2222},
									{0x0440, 0x0440, 0x0440, 0x0440}
								};
int Tetromino::patternR[7][4] = {
									{0x0240, 0x4440, 0x0420, 0x4240},
									{0x0140, 0x2220, 0x0110, 0x2210},
									{0x0220, 0x4440, 0x0820, 0x2440},
									{0x0420, 0x2240, 0x0420, 0x2240},
									{0x0120, 0x4220, 0x0120, 0x4220},
									{0x0100, 0x2222, 0x0100, 0x2222},
									{0x0220, 0x0220, 0x0220, 0x0220}
								};
int Tetromino::patternD[7][4] = {
									{0x0A40, 0x0840, 0x00E0, 0x0240},
									{0x0340, 0x4020, 0x0070, 0x0030},
									{0x0C20, 0x00C0, 0x00E0, 0x2040},
									{0x0860, 0x0240, 0x0860, 0x0240},
									{0x0160, 0x0420, 0x0160, 0x0420},
									{0x0F00, 0x0002, 0x0F00, 0x0002},
									{0x0060, 0x0060, 0x0060, 0x0060}
								};

Tetromino::Tetromino(int x, int y, int turn, int type)
{
#ifdef DEBUG__
	printf("Tetromino::<init>\n");
#endif
	// x = 0 ` 9
	// y = 0 ` 21
	// type = 0 ` 6
	this->x = x%10 + 3; // 3 ` 12
	this->y = y%22; // ‚»‚Ì‚Ü‚Ü
	this->turn = turn % 4;
	this->type = type%7 + 1; // 1 ` 7
	this->interval = 0;
	this->landing = 0;
	this->landed = 0;
}

Tetromino::~Tetromino()
{
#ifdef DEBUG__
	printf("Tetromino::<delete>\n");
#endif
}

int Tetromino::setNew(Field* f, int x, int y, int turn, int type)
{
	this->x = x%10 + 3; // 3 ` 12
	this->y = y%22; // ‚»‚Ì‚Ü‚Ü
	this->turn = (turn+4) % 4;
	this->type = type%7 + 1; // 1 ` 7
	this->interval = 0;
	this->landing = 0;
	this->landed = 0;
	return checkGameOver(f);
}

int Tetromino::checkGameOver(Field* f)
{
	int pat = pattern[type%7][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				if (f->getField(x + j, y + i) != 0)
				{
					return 1;
				}
			}

			mask = mask >> 1;
		}
	}
	return 0;
}

/*
 * Š®‘S’…’n‚µ‚½‚Æ‚«A1‚ª•Ô‚é(next‚É•Ï‚í‚é)
 */
int Tetromino::addInterval(Field* f, double d, int* score)
{
	this->interval += d;

	if (this->interval > 15)
	{
		// interval‚ª15‚ğ’´‚¦‚½‚ç1ƒ}ƒX‰º‚°‚é
		int land = moveDown(f);

		if (land != 0)
		{
			// ’…’n‚µ‚Ä‚¢‚½‚Æ‚«

			if (landing < 0)
			{
				// ‰’…’l‚È‚ç 0‚É
//				this->interval = 0;
				landing = 0;
			}
			else
			{
				// ’…’nÏ‚İ‚È‚çAlanding‚É1‚ğ‘«‚·
				landing = landing + 1;
			}
			if (landing >= 25)
			{
				//landing ‚ª 25‚ğ‰z‚¦‚½‚çŠ®‘S’…’n
				landed = 1;
				trueDraw(f);
				f->renewField();
				return 1;
			}
		}
		else
		{
			// ’…’n‚µ‚Ä‚¢‚È‚¢‚Æ‚«‚ÍAinterval‚ğ16‚ÅŠ„‚Á‚½—]‚è‚É
			this->interval = this->interval - 16;
			// landing‚ğ‰Šú‰»
			landing = -1;
			// “¾“_‚ğ1‘‚â‚·
			(*score) += 1;
			int temp = 0;
			return addInterval(f, 0, &temp);
		}
	}
	return 0;
}

void Tetromino::trueDraw(Field* f)
{
	int pat = pattern[type%7][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				f->set(x + j - 3, y + i, type);
			}

			mask = mask >> 1;
		}
	}
}

void Tetromino::draw(Field* f)
{
	int pat = pattern[type%7][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				f->set(x + j - 3, y + i, type + 7);
			}

			mask = mask >> 1;
		}
	}
}

void Tetromino::erase(Field* f)
{
	int pat = pattern[type%7][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				f->erase(x + j - 3, y + i);
			}

			mask = mask >> 1;
		}
	}
}

void Tetromino::moveRight(Field* f)
{
	int pat = patternR[type%7][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				if (f->getField(x + j + 1, y + i) != 0)
				{
					return;
				}
			}

			mask = mask >> 1;
		}
	}
//	if(landed == 0){
	erase(f);
	x++;
	draw(f);
//	}
}

void Tetromino::moveLeft(Field* f)
{
	int pat = patternL[type%7][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				if (f->getField(x + j - 1, y + i) != 0)
				{
					return;
				}
			}

			mask = mask >> 1;
		}
	}
//	if(landed == 0){
	erase(f);
	x--;
	draw(f);
//	}
}

/*
 * —‰º‚Å‚«‚È‚¢‚Æ‚«1‚ğ•Ô‚·B
 * ¡‰ñ‚Í—‰º‚Å‚«‚é‚¯‚ÇAŸ‚Í—‰º‚Å‚«‚È‚¢‚Æ‚«‚ÍA2‚ğ•Ô‚·B
 */
int Tetromino::moveDown(Field* f)
{
	int pat = patternD[type%7][turn%4];
	int mask = 0x8000;
	int rc = 0;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				if (f->getField(x + j, y + i + 1) != 0)
				{
					return 1;
				}
				else if (f->getField(x + j, y + i + 2) != 0)
				{
					rc = 2;
				}
			}

			mask = mask >> 1;
		}
	}
//	if(landed == 0){
	erase(f);
	y++;
	draw(f);
//	}
	return rc;
}

void Tetromino::turnRight(Field* f)
{
	/*
		int npat = pattern[type%7][turn%4];
		int pat = pattern[type%7][(turn+1) % 4];
		int mask = 0x8000;
		for(int i=0; i<4; i++){
			for(int j=0; j<4; j++){
				if((pat & mask) != 0 && (npat & mask) == 0){
					if(f->getField(x + j, y + i) != 0){
						return;
					}
				}

				mask = mask >> 1;
			}
		}
		erase(f);
		turn = (turn+1) % 4;
		draw(f);
	//*/
//*
	int npat = pattern[type%7][turn%4];
	int pat = pattern[type%7][(turn+1) % 4];
	int mask = 0x8000;

	int nflg = 1;
	int lflg = 1;
	int rflg = 1;

	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
//				if((npat & mask) == 0 && f->getField(x + j, y + i) != 0 && f->getFlg(x + j, y + i) != -1){
				if ((npat & mask) == 0 && f->getField(x + j, y + i) != 0)
				{
					nflg = 0;
				}
				if (f->getField(x + j + 1, y + i) != 0 &&
						f->getFlg(x + j - 2, y + i) != -1)
				{
					lflg = 0;
				}
				if (f->getField(x + j - 1, y + i) != 0 &&
						f->getFlg(x + j - 4, y + i) != -1)
				{
					rflg = 0;
				}
				if (nflg == 0 && lflg == 0 && rflg == 0)
				{
					draw(f);
					return;
				}
			}

			mask = mask >> 1;
		}
	}
	erase(f);
//	if(landed == 0){
	if (nflg == 0)
	{
		if (type == 5)
		{ // c–_‚Í•ÇR‚è‚µ‚È‚¢
			draw(f);
			return;
		}
		if (rflg == 1)
		{
			x--;
		}
		else if (lflg == 1)
		{
			x++;
		}
	}
	turn = (turn+1) % 4;
//	}
	draw(f);
//*/
}

void Tetromino::turnLeft(Field* f)
{
	int npat = pattern[type%7][turn%4];
	int pat = pattern[type%7][(turn+3) % 4];
	int mask = 0x8000;

	int nflg = 1;
	int lflg = 1;
	int rflg = 1;

	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
//				if((npat & mask) == 0 && f->getField(x + j, y + i) != 0 && f->getFlg(x + j, y + i) != -1){
				if ((npat & mask) == 0 && f->getField(x + j, y + i) != 0)
				{
					nflg = 0;
				}
				if (f->getField(x + j + 1, y + i) != 0 &&
						f->getFlg(x + j - 2, y + i) != -1)
				{
					lflg = 0;
				}
				if (f->getField(x + j - 1, y + i) != 0 &&
						f->getFlg(x + j - 4, y + i) != -1)
				{
					rflg = 0;
				}
				if (nflg == 0 && lflg == 0 && rflg == 0)
				{
					draw(f);
					return;
				}
			}

			mask = mask >> 1;
		}
	}
//	if(landed == 0){
	erase(f);
	if (nflg == 0)
	{
		if (type == 5)
		{ // c–_‚Í•ÇR‚è‚µ‚È‚¢
			draw(f);
			return;
		}
		if (rflg == 1)
		{
			x--;
		}
		else if (lflg == 1)
		{
			x++;
		}
	}
	turn = (turn+3) % 4;
//	}
	draw(f);
	/*
		int npat = pattern[type%7][turn%4];
		int pat = pattern[type%7][(turn+3) % 4];
		int mask = 0x8000;
		for(int i=0; i<4; i++){
			for(int j=0; j<4; j++){
				if((pat & mask) != 0 && (npat & mask) == 0){
					if(f->getField(x + j, y + i) != 0){
						return;
					}
				}

				mask = mask >> 1;
			}
		}
		erase(f);
		turn = (turn+3) % 4;
		draw(f);
	//*/
}

void Tetromino::drawNext(Graphics* g, int type, int turn)
{
	type = (type + 1) % 7;
	int pat = pattern[type%7][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				g->drawImage(Block::imgs[(type+6)%7], 505 + j * 16 , 82 + i * 16);
			}
			else
			{
				g->setColor(Color::black);
				g->fillRect(505 + j * 16 , 82 + i * 16, 16, 16);
			}

			mask = mask >> 1;
		}
	}
}

void Tetromino::drawNext2(Graphics* g, int type, int turn)
{
	type = (type + 1) % 7;
	int pat = pattern[type][turn%4];
	int mask = 0x8000;
	for (int i=0; i<4; i++)
	{
		for (int j=0; j<4; j++)
		{
			if ((pat & mask) != 0)
			{
				g->drawImage(Block::imgs[(type+6)%7], 505 + j * 16 , 178 + i * 16);
			}
			else
			{
				g->setColor(Color::black);
				g->fillRect(505 + j * 16 , 178 + i * 16, 16, 16);
			}

			mask = mask >> 1;
		}
	}
}

