//#define DEBUG__

#include "gtetris.h"

int GTetris::scores[4] = {40, 120, 400, 1200};

GTetris::GTetris()
{
#ifdef DEBUG__
	printf("GTetris::<init>\n");
#endif
	noPlaying = 1;
	setTitle("GTetris");
	setBounds((800-638)/2, (600-413)/2, 638, 413);

	background = new Image("/APPS/BAYGUI/GTETRIS.APP/MAIN.BM5");

	field = new Field();
	/*
		field->set(0, 21, 1);
		field->set(0, 20, 1);
		field->set(0, 19, 1);
		field->set(0, 18, 1);
		field->set(1, 21, 2);
		field->set(1, 20, 2);
		field->set(1, 19, 2);
		field->set(2, 19, 2);
		field->set(2, 21, 3);
		field->set(2, 20, 3);
		field->set(3, 20, 3);
		field->set(3, 19, 3);
		field->set(3, 21, 4);
		field->set(4, 21, 4);
		field->set(4, 20, 4);
		field->set(5, 20, 4);
		field->set(4, 18, 5);
		field->set(4, 19, 5);
		field->set(5, 19, 5);
		field->set(6, 19, 5);
		field->set(8, 21, 6);
		field->set(8, 20, 6);
		field->set(7, 20, 6);
		field->set(7, 19, 6);
		field->set(5, 21, 7);
		field->set(6, 21, 7);
		field->set(7, 21, 7);
		field->set(6, 20, 7);
	*/

	random.setSeed(MonAPI::System::getTick());
	tetromino = new Tetromino(3, -1, 0, random.nextInt() % 7);
//	tetromino->draw(field);

	next[0] = random.nextInt() % 7;
	next[1] = random.nextInt() % 7;

	removeFlg = 0;
	speed = 0.2;
}

GTetris::~GTetris()
{
#ifdef DEBUG__
	printf("GTetris::<delete>\n");
#endif
	delete background;
	delete tetromino;
	delete field;
}

void GTetris::paint(Graphics* g)
{
#ifdef DEBUG__
	printf("GTetris::paint()\n");
#endif
	g->drawImage(background, 0, 0);
	if (noPlaying == 0)
	{
		field->drawAllField(g);
		Tetromino::drawNext(g, next[0], 0);
		Tetromino::drawNext2(g, next[1], 0);
		drawScores(g);
	}
}

void GTetris::processEvent(Event* e)
{
#ifdef DEBUG__
	printf("GTetris::processEvent()\n");
#endif
	if (e->getType() == e->TIMER)
	{
		if (noPlaying != 0)
		{
			return;
		}
		if (removeFlg == 0)
		{
			double interval =  speed;
			int temp = 0;
			land = tetromino->addInterval(field, interval, &temp);
			if (land == 1)
			{
				checkLine();
				if (removeFlg > 0)
				{
					field->drawField(_g);
					setTimer(500);
					//return;
				}
				else
				{
					noPlaying = tetromino->setNew(field, 3 + LRPush, -1, ZXPush, next[0]);
					next[0] = next[1];
					next[1] = random.nextInt() % 7;
					Tetromino::drawNext(_g, next[0], 0);
					Tetromino::drawNext2(_g, next[1], 0);
					tetromino->draw(field);
					land = 0;
					levelUp();
					drawScores(_g);
					field->drawField(_g);
					setTimer(250);
					//return;
				}
			}
			else
			{
				field->drawField(_g);
				setTimer(25);
				//return;
			}
		}
		else
		{
			noPlaying = tetromino->setNew(field, 3 + LRPush, -1, ZXPush, next[0]);
			next[0] = next[1];
			next[1] = random.nextInt() % 7;
			tetromino->draw(field);
			land = 0;
			field->drop();
			field->drawField(_g);
			Tetromino::drawNext(_g, next[0], 0);
			Tetromino::drawNext2(_g, next[1], 0);
			line += removeFlg;

			score += scores[removeFlg-1];
			levelUp();
			drawScores(_g);

			removeFlg = 0;
			setTimer(250);
		}
		update();
	}
	else if (e->getType() == e->KEY_PRESSED && removeFlg == 0)
	{
		KeyEvent* ke = (KeyEvent *)e;
		int keycode = ke->getKeycode();
//		printf("%d\n", keycode);
//		land = 0;
		double interval = 0;
		if (noPlaying != 0)
		{
			if (keycode != KeyEvent::VKEY_ENTER)
			{
				return;
			}
			else
			{
				newGame();
				setTimer(250);
				return;
			}
		}
		switch (keycode)
		{
		case KeyEvent::VKEY_LEFT:
			LRPush = -1;
			if (land == 0)
			{
				tetromino->moveLeft(field);
				field->drawField(_g);
			}
			break;
		case KeyEvent::VKEY_UP:
			break;
		case KeyEvent::VKEY_RIGHT:
			LRPush = 1;
			if (land == 0)
			{
				tetromino->moveRight(field);
				field->drawField(_g);
			}
			break;
		case KeyEvent::VKEY_DOWN:
			DownPush = 1;
//			interval =  speed * 16;
			interval =  16;
			if (speed > 16)
			{
				interval = speed;
			}
			land = tetromino->addInterval(field, interval, &score);
			if (land == 1)
			{
				checkLine();
				if (removeFlg > 0)
				{
					field->drawField(_g);
					setTimer(500);
				}
				else
				{
					noPlaying = tetromino->setNew(field, 3 + LRPush, -1, ZXPush, next[0]);
					next[0] = next[1];
					next[1] = random.nextInt() % 7;
					Tetromino::drawNext(_g, next[0], 0);
					Tetromino::drawNext2(_g, next[1], 0);
					levelUp();
					drawScores(_g);

					tetromino->draw(field);
					land = 0;
					field->drawField(_g);
					setTimer(500);
					return;
				}
			}
			field->drawField(_g);
			break;
		case 'z':
			if (ZXPush == 0 && land == 0)
			{
				tetromino->turnLeft(field);
				field->drawField(_g);
			}
			ZXPush = -1;
			break;
		case 'x':
			if (ZXPush == 0 && land == 0)
			{
				tetromino->turnRight(field);
				field->drawField(_g);
			}
			ZXPush = 1;
			break;
		}
	}
	else if (e->getType() == e->KEY_RELEASED)
	{
		KeyEvent* ke = (KeyEvent *)e;
		int keycode = ke->getKeycode();
//		printf("%d\n", keycode);
		switch (keycode)
		{
		case KeyEvent::VKEY_LEFT:
			LRPush = 0;
			break;
		case KeyEvent::VKEY_UP:
			break;
		case KeyEvent::VKEY_RIGHT:
			LRPush = 0;
			break;
		case KeyEvent::VKEY_DOWN:
			DownPush = 0;
			break;
		case 'z':
			ZXPush = 0;
			break;
		case 'x':
			ZXPush = 0;
			break;
		}
	}
	else if (e->getType() == e->FOCUS_IN)
	{
		//setTimer(25);
	}
}

void GTetris::checkLine()
{
	removeFlg = field->checkLine(_g);
}

void GTetris::drawScores(Graphics* g)
{
	char c[255];
	g->setColor(Color::black);
	g->fillRect(25, 26, 161, 32);
	g->fillRect(25, 104, 161, 34);
	g->fillRect(25, 311, 161, 56);
	g->setColor(Color::white);
	sprintf(c, "%10d", score);
	g->drawString(c, 100, 340);
	sprintf(c, "%10d", line);
	g->drawString(c, 100, 120);
	sprintf(c, "%10d", level);
	g->drawString(c, 100, 40);
}

void GTetris::levelUp()
{
	if (level >= 999)
	{
		return;
	}

	if ((level % 100 == 99 || level == 998) && removeFlg == 0)
	{
		// level ?99、および998のときはラインを消さないとレベルアップしない
		return;
	}

	level = level + 1;

	level = level + removeFlg;

	if (level < 200)
	{
		speed = level * 0.0391 + 0.25;
	}
	else if (level < 250)
	{
		speed = (level - 200) * 0.3 + 1;
	}
	else if (level < 300)
	{
		speed = 16;
	}
	else if (level < 400)
	{
		speed = (level - 300) * 0.32 + 32;
	}
	else if (level < 500)
	{
		speed = 80 - (level - 400) * 0.32;
	}
	else
	{
		speed = 352;
	}
}

void GTetris::newGame()
{
	tetromino = new Tetromino(3, -1, 0, random.nextInt() % 7);
	next[0] = random.nextInt() % 7;
	next[1] = random.nextInt() % 7;

	removeFlg = 0;
	speed = 0.25;
	noPlaying = 0;
	level = 1;

	for (int i=0; i<22; i++)
	{
		for (int j=0; j<10; j++)
		{
			field->erase(j, i);
		}
	}

	field->drawField(_g);
	field->reset();
	tetromino->draw(field);
	Tetromino::drawNext(_g, next[0], 0);
	Tetromino::drawNext2(_g, next[1], 0);
	drawScores(_g);
}

int MonaMain(List<char*>* pekoe)
{
	Block::imgs[0] = new Image("/APPS/BAYGUI/GTETRIS.APP/BLOCK0.BM5");
	Block::imgs[1] = new Image("/APPS/BAYGUI/GTETRIS.APP/BLOCK1.BM5");
	Block::imgs[2] = new Image("/APPS/BAYGUI/GTETRIS.APP/BLOCK2.BM5");
	Block::imgs[3] = new Image("/APPS/BAYGUI/GTETRIS.APP/BLOCK3.BM5");
	Block::imgs[4] = new Image("/APPS/BAYGUI/GTETRIS.APP/BLOCK4.BM5");
	Block::imgs[5] = new Image("/APPS/BAYGUI/GTETRIS.APP/BLOCK5.BM5");
	Block::imgs[6] = new Image("/APPS/BAYGUI/GTETRIS.APP/BLOCK6.BM5");

	GTetris* watch = new GTetris();
	watch->run();
	delete watch;

	for (int i=0; i<7; i++)
	{
		delete Block::imgs[i];
	}
	return 0;
}
