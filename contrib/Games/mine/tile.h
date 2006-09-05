/*
Copyright (c) 2005 mizyo
All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, provided that the above copyright notice(s) and this permission notice appear in all copies of the Software and that both the above copyright notice(s) and this permission notice appear in supporting documentation.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder.
*/

#ifndef tile_h_
#define tile_h_

#include <baygui.h>

class mine_tile : public Component {
public:
	typedef enum {
		init,
		init_lock,
		opened,
		marked,
		missmark,
		mark_lock,
		bomb,
		exploded
	} tile_state;
private:
	tile_state state;
	int value;
	mine_tile* around[8];
	static int click_state;
	static Component* click_sender;
	static int bombs_num;
	static int init_tile_num;
	static Event* event;
	static bool start;
	
	void clickProc(MouseEvent*);
	void draw_frame(Graphics*);
	void draw_grid(Graphics*);
	void draw_number(Graphics*);
	void draw_flag(Graphics*);
	void draw_bomb(Graphics*);
	void draw_cross(Graphics*);
	void bit_blt(Graphics*, const int*);
	void send_gameover_to_parent();
	void send_gameover_to_tiles();
	void send_gamestart_to_parent();
	void send_clear_to_parent();
	void open_tile_on_gameover();
	void tiles_open();
	void count_around_bombs_num();
	void initialize();
	void check_gameclear();
public:
	mine_tile(){
		initialize();
	}
	mine_tile(int x, int y){
		initialize();
		setBounds(x, y, 16, 16);
	}

	void paint(Graphics*);
	void processEvent(Event*);
	void setTileValue(int);
	void setState(tile_state);
	int getTileValue();
	void setAround(mine_tile*, mine_tile*, mine_tile*, mine_tile*, mine_tile*, mine_tile*, mine_tile*, mine_tile*);
	
	enum {
		GAMESTART,
		GAMECLEAR,
		GAMEOVER
	};
};

#endif
