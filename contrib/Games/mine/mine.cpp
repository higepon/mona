/*
Copyright (c) 2005 mizyo
All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both the above copyright notice(s) and this permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.
*/

#include "mine.h"

/*!
	@biref コンストラクタ
*/
mine::mine(){
	this->setTitle("マインスイーパ");
	int w = 144 + getInsets()->left + getInsets()->right;
	int h = 144 + getInsets()->top + getInsets()->bottom;
	this->setBounds(100, 100, w, h);
	tile_init();
	tick = 0;
	start = false;
}

/*!
	@brief タイル群の初期化
*/
void mine::tile_init(){
	bool* bomb_pos = new bool[81];
	make_bombs_position(bomb_pos);
	for(int x=0; x<9; x++){
		for(int y=0; y<9; y++){
			tiles[x][y] = new mine_tile((x << 4), (y << 4));
			
			//とりあえず、そのタイルが爆弾か、爆弾じゃないかを設定
			tiles[x][y]->setTileValue(bomb_pos[((x << 3) + x) + y] ? -1 : 0);
			
		}
	}
	
	//周囲のタイル情報を設定
	for(int x=0; x<9; x++){
		for(int y=0; y<9; y++){
			mine_tile* tile = tiles[x][y];
			if(x == 0){
				if(y == 0){
					tile->setAround(NULL, NULL,
									tiles[x+1][y], tiles[x+1][y+1],
									tiles[x][y+1], NULL,
									NULL, NULL);
				} else if(y == 8){
					tile->setAround(tiles[x][y-1], tiles[x+1][y-1],
									tiles[x+1][y], NULL,
									NULL, NULL,
									NULL, NULL);
				} else {
					tile->setAround(tiles[x][y-1], tiles[x+1][y-1],
									tiles[x+1][y], tiles[x+1][y+1],
									tiles[x][y+1], NULL,
									NULL, NULL);
				}
			} else if(x == 8){
				if(y == 0){
					tile->setAround(NULL, NULL,
									NULL, NULL,
									tiles[x][y+1], tiles[x-1][y+1],
									tiles[x-1][y], NULL);
				} else if(y == 8){
					tile->setAround(tiles[x-1][y], NULL,
									NULL, NULL,
									NULL, NULL,
									tiles[x-1][y], tiles[x-1][y-1]);
				} else {
					tile->setAround(tiles[x][y-1], NULL,
									NULL, NULL,
									tiles[x][y+1], tiles[x-1][y+1],
									tiles[x-1][y], tiles[x-1][y-1]);
				}
			} else {
				if(y == 0){
					tile->setAround(NULL, NULL,
									tiles[x+1][y], tiles[x+1][y+1],
									tiles[x][y+1], tiles[x-1][y+1],
									tiles[x-1][y], NULL);
				} else if(y == 8){
					tile->setAround(tiles[x][y-1], tiles[x+1][y-1],
									tiles[x+1][y], NULL,
									NULL, NULL,
									tiles[x-1][y], tiles[x-1][y-1]);
				} else {
					tile->setAround(tiles[x][y-1], tiles[x+1][y-1],
									tiles[x+1][y], tiles[x+1][y+1],
									tiles[x][y+1], tiles[x-1][y+1],
									tiles[x-1][y], tiles[x-1][y-1]);
				}
			}
			add(tile);
		}
	}
}

/*!
	@brief デストラクタ
*/
mine::~mine(){
	for(int x=0; x<9; x++){
		for(int y=0; y<9; y++){
			delete tiles[x][y];
		}
	}
}

/*!
	@brief 送信オブジェクトを判定
*/
bool mine::is_game_event(Event* evt){
	if(evt->getType() != Event::CUSTOM_EVENT){
		return false;
	}
	
	Component* src = evt->getSource();
	for(int x=0; x<9; x++){
		for(int y=0; y<9; y++){
			if(src == tiles[x][y]){
				return true;
			}
		}
	}
	return false;
}

/*!
	@brief イベントプロシージャ
*/
void mine::processEvent(Event* evt){
	if(evt->getType() == Event::TIMER){
		timer_proc();
		return;
	}
	
	if(is_game_event(evt)){
		switch(evt->arg1){
		case mine_tile::GAMEOVER:
			start = false;
			printf("(　ﾟдﾟ)ﾄﾞｶﾝｰ\n");
			break;
		case mine_tile::GAMECLEAR:
			start = false;
			printf("お(･∀･)め(･∀･)で(･∀･)と(･∀･)う!\n");
			printf("タイム： %d秒\n", tick);
			break;
		case mine_tile::GAMESTART:
			start = true;
			setTimer(1000);
			break;
		}
	}
}

int MonaMain(List<char*>* pekoe){
	mine* app = new mine();
	app->run();
	delete app;
	return 0;
}

/*!
	@brief 爆弾の初期位置情報を生成する
*/
void mine::make_bombs_position(bool* ret){
	//初期配置
	for(int i=0; i<81; i++){
		ret[i] = (i < 10) ? true : false;
	}
	
	//シャッフリング
	MonAPI::Random random;
	random.setSeed(syscall_get_tick());
	for(int i=0; i<81; i++){
		int swap = random.nextInt() % 81 - i;
		bool tmp = ret[i];
		ret[i] = ret[i+swap];
		ret[i+swap] = tmp;
	}
}

void mine::timer_proc(){
	if(start){
		tick++;
		setTimer(1000);
	}
}
