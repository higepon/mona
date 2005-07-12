/*
Copyright (c) 2004 Hossy
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <baygui.h>

#define WIDTH_BASE "T"   // 文字を縦にそろえるための、文字の幅の基準。W が最も広く、I が最も狭い事が多い。
#define TAB_SIZE   4

/** コンストラクタ */
TextArea::TextArea(int buffer_size, bool draw_line)
{
	text = new char[buffer_size];
	_textEvent = new Event(Event::TEXT_CHANGED, this);
	_buffer_size = buffer_size; _draw_line = draw_line;

	// 当り障り無い初期値
	_overwrite = false;
	_max_line = 0; _max_col = 0;
	_offset_x = 0; _offset_y = 0;
	_ptr_x = 0; _ptr_y = 0;
	_text_len = 0; text[0] = '\0';
}

/** デストラクタ */
TextArea::~TextArea()
{
	delete _textEvent;
	delete[] text;
}

/**
 大きさを設定する
 @param x x座標
 @param y y座標
 @param width 幅
 @param height 高さ
*/
void TextArea::setRect(int x, int y, int width, int height)
{
	TextField::setRect( x, y, getWidth(), getHeight() );
	_max_line = getHeight() / getFontMetrics()->getHeight(WIDTH_BASE) - 1;
	_max_col = width / getFontMetrics()->getWidth(WIDTH_BASE) - 1;
}

/**
 テキスト設定
 @param text
 */
void TextArea::setText(char* text)
{
	if( (int)strlen(text) >= _buffer_size ) return;
	memset(this->text, 0, sizeof(this->text));
	strcpy(this->text, text);
	_text_len = strlen(text);
	SetPos(-1);
	OffsetChange();
	//if( firstpaint == true ) repaint();
}

/** テキスト取得 */
char* TextArea::getText()
{
	return text;
}

/** 行番号フォーマット */
void TextArea::LineNoFormat(char* buff, int line, int size)
{
	// 初期化
	for(int i=0; i<size; i++) buff[i] = ' '; buff[size] = '\0';

	// 10進文字列化
	for(int i=size-1; i>=0; i--) {
		buff[i] = '0' + (line % 10);
		line /= 10; if( line == 0 ) break;
	}
}

/** 再描画 */
void TextArea::paint(Graphics *g)
{
	/*if( firstpaint == false ) firstpaint = true;*/

	// 色
	int clr_focuse = 0xffc8c8c8;
	int clr_border = 0xff000000;
	int clr_back   = 0xffffffff;
	int clr_fore   = 0xff000000;
	int clr_caret  = 0xff000000;
	int clr_line   = 0xff000000;
	if (getFocused() == true && getEnabled() == true) {
		clr_focuse = 0xff0080ff;
	}
	if (getEnabled() == false) {
		clr_fore = 0xff808080;
	}

	// 文字サイズ
	int fh = getFontMetrics()->getHeight(WIDTH_BASE);
	int fw = getFontMetrics()->getWidth(WIDTH_BASE);

	// 枠線
	g->setColor(clr_focuse);
	g->drawRect(0, 0, getWidth(), getHeight());
	g->setColor(clr_border);
	g->drawRect(1, 1, getWidth() - 2, getHeight() - 2);
	g->setColor(clr_back);
	g->fillRect(2, 2, getWidth() - 3, getHeight() - 3);

	int draw_left = 4;
	int draw_top = 4;

	g->setColor(clr_line);
	if( _draw_line ) {
		draw_left += 2;
		int fw = getFontMetrics()->getWidth("0");
		int keta = 1, tmp = LineCount(); while( tmp >= 10 ) { keta++; tmp/=10; }

		char* buff = new char[keta+1];
		for(int i=0; i<_max_line; i++) {
			if( i + _offset_y >= LineCount() ) break;
			LineNoFormat( buff, i + _offset_y + 1, keta );
			g->drawText(buff, draw_left, i * fh + draw_top );
		}
		delete[] buff;

		draw_left += keta * fw + 3;
		g->drawLine( draw_left - 3, draw_top, draw_left - 3, getHeight() - draw_top );
	}

	// キャレットの描画
	if (getFocused() == true && getEnabled() == true) {
		if (_overwrite) {
			g->setColor(clr_caret);
			g->fillRect( (_ptr_x-_offset_x) * fw + draw_left - 1, (_ptr_y-_offset_y) * fh + draw_top, fw, fh );
		} else {
			g->setColor(clr_caret);
			g->drawLine( (_ptr_x-_offset_x) * fw + draw_left - 1, (_ptr_y-_offset_y) * fh + draw_top,
			              (_ptr_x-_offset_x) * fw + draw_left - 1, (_ptr_y-_offset_y) * fh + draw_top + fh - 2 );
		}
	}

	// 一行ずつ描画
	char* draw_text = text + Y2P(_offset_y);
	for(int i=0; i<_max_line; i++) {
		int col = 0;

		// 表示しない文字を飛ばす
		for(int j=0; j<_offset_x; ) {
			if( *draw_text == '\0' || *draw_text == '\n' ) break;
			if( *draw_text == '\t' ) j+=TAB_SIZE; else j++;
			if( j >= _offset_x ) col = j - _offset_x;
			draw_text++;
		}

		// 一文字ずつ描画
		for(; col<_max_col; col++) {
			if( *draw_text == '\0' || *draw_text == '\n' ) break;
			if( *draw_text == '\t' ) {
				col += TAB_SIZE - ( (col+_offset_x) % TAB_SIZE ) - 1;
			} else {
				char tmp[2] = { *draw_text, '\0' };
				int width = getFontMetrics()->getWidth(tmp);
				if( _overwrite && draw_text == text+XY2P(_ptr_x,_ptr_y) ) {
					g->setColor(clr_back);
				} else {
					g->setColor(clr_fore);
				}
				g->drawText(tmp, col * fw + draw_left + (fw-width) / 2, i * fh + draw_top );
			}
			draw_text++;
		}

		// 残りも飛ばす
		while( *draw_text != '\0' && *draw_text != '\n' ) draw_text++;
		if( *draw_text == '\0' ) break;
		draw_text++;
	}
}

/** 1文字挿入 */
void TextArea::insertCharacter(char c)
{
	if( _text_len >= _buffer_size ) return;
	int pos = XY2P(_ptr_x,_ptr_y);
	for(int i=_text_len; i>=pos; i--)
		text[i+1] = text[i];
	text[pos] = c;
	_text_len++;
	SetPos(pos+1);
	OffsetChange();
}

/** 1文字削除 */
void TextArea::deleteCharacter()
{
	int pos = XY2P(_ptr_x,_ptr_y);
	if( text[pos] == '\0' ) return;
	if( _ptr_x > _offset_x + _max_col - 1 ) _offset_x = _ptr_x - _max_col + 1;
	if( _ptr_y <= _offset_y ) _offset_y = (_ptr_y != 0) ? _ptr_y - 1 : _ptr_y;
	if( _ptr_x <= _offset_x ) _offset_x = (_ptr_x != 0) ? _ptr_x - 1 : _ptr_x;
	while( (text[pos] = text[pos+1]) != '\0' ) pos++;
	_text_len -= 1;
}

/** イベント処理 */
void TextArea::processEvent(Event* event)
{
	// 非活性の時はイベントを受け付けない
	if (getEnabled() == false) return;

	// キー押下
	if (event->getType() == KeyEvent::KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->getKeycode();
		if (keycode == KeyEvent::VKEY_BACKSPACE) {
			if (_ptr_x > 0 || _ptr_y > 0) {
				// バックスペース
				SetPos( XY2P(_ptr_x,_ptr_y) - 1 );
				OffsetChange();
				deleteCharacter();
				/*if (firstpaint == true)*/ repaint();
			}
		} else if (keycode == KeyEvent::VKEY_DELETE) {
			// 一文字削除
			if (text[XY2P(_ptr_x,_ptr_y)] != '\0' ) {
				deleteCharacter();
				/*if (firstpaint == true)*/ repaint();
			}
		} else if (keycode == KeyEvent::VKEY_INSERT) {
			// 上書き設定変更
			_overwrite = !_overwrite;
			/*if (firstpaint == true)*/ repaint();
		} else if (keycode == KeyEvent::VKEY_LEFT) {
			// ←移動
			if( _ptr_x != 0 || _ptr_y != 0 ) {
				SetPos( XY2P(_ptr_x,_ptr_y) - 1 );
				OffsetChange();
				/*if (firstpaint == true)*/ repaint();
			}
		} else if (keycode == KeyEvent::VKEY_RIGHT) {
			// →移動
			if (text[XY2P(_ptr_x,_ptr_y)] != '\0' ) {
				SetPos( XY2P(_ptr_x,_ptr_y) + 1 );
				OffsetChange();
				/*if (firstpaint == true)*/ repaint();
			}
		} else if (keycode == KeyEvent::VKEY_UP) {
			// ↑移動
			if (_ptr_y > 0) {
				SetPos( XY2P(_ptr_x,_ptr_y-1) );
				OffsetChange();
				/*if (firstpaint == true)*/ repaint();
			}
		} else if (keycode == KeyEvent::VKEY_DOWN) {
			// ↓移動
			if (_ptr_y < LineCount()-1) {
				SetPos( XY2P(_ptr_x,_ptr_y+1) );
				OffsetChange();
				/*if (firstpaint == true)*/ repaint();
			}
		} else if (keycode == KeyEvent::VKEY_PGUP) {
			// 前ページ
			_ptr_y -= _max_line - 1;
			if( _ptr_y < 0 ) _ptr_y = 0;
			_offset_y -= _max_line - 1;
			if( _offset_y < 0 ) _offset_y = 0;
			SetPos( XY2P(_ptr_x,_ptr_y) );
			OffsetChange();
			/*if (firstpaint == true)*/ repaint();
		} else if (keycode == KeyEvent::VKEY_PGDOWN) {
			// 次ページ
			_ptr_y += _max_line - 1;
			if( _ptr_y > LineCount() ) _ptr_y = LineCount();
			_offset_y += _max_line - 1;
			if( _offset_y > LineCount() - _max_line / 2 ) _offset_y = LineCount() - _max_line / 2;
			if( _offset_y < 0 ) _offset_y = 0;
			SetPos( XY2P(_ptr_x,_ptr_y) );
			OffsetChange();
			/*if (firstpaint == true)*/ repaint();
		} else if (keycode == KeyEvent::VKEY_HOME) {
			// 行の先頭へ移動
			SetPos( Y2P(_ptr_y) );
			OffsetChange();
			/*if (firstpaint == true)*/ repaint();
		} else if (keycode == KeyEvent::VKEY_END) {
			// 行の末尾へ移動
			SetPos( Y2P(_ptr_y+1) - 1 );
			OffsetChange();
			/*if (firstpaint == true)*/ repaint();
		} else if (keycode == KeyEvent::VKEY_TAB) {
			// タブの挿入
			insertCharacter( '\t' );
			/*if (firstpaint == true)*/ repaint();
		} else if (keycode == KeyEvent::VKEY_ENTER) {
			// 改行
			insertCharacter( '\n' );
			/*if (firstpaint == true)*/ repaint();
		} else if (keycode < 128) {
			// 1文字挿入
			if (_overwrite && text[XY2P(_ptr_x,_ptr_y)] != '\n') deleteCharacter();
			insertCharacter(keycode);
			/*if (firstpaint == true)*/ repaint();
		}
	// マウスで押された
	} else if (event->getType() == MouseEvent::MOUSE_PRESSED) {
		int ry = ((MouseEvent*)event)->getY();
		int rx = ((MouseEvent*)event)->getX();
		int fh = getFontMetrics()->getHeight(WIDTH_BASE);
		int fw = getFontMetrics()->getWidth(WIDTH_BASE);
		SetPos( XY2P( ( rx - 4 ) / fw, ( ry - 4 ) / fh ) );
		/*if (firstpaint == true)*/ repaint();
	// フォーカス状態変更
	} else if (event->getType() == Event::FOCUS_IN || event->getType() == Event::FOCUS_OUT) {
		//if (firstpaint == true) {
			repaint();
			//Control::postEvent(_focusEvent);
		//}
	}
}

/** キャレットが画面に入るよう _offset を調整 */
void TextArea::OffsetChange()
{
	if( _ptr_x > _offset_x + _max_col ) _offset_x = _ptr_x - _max_col;
	if( _ptr_x < _offset_x ) _offset_x = _ptr_x;
	if( _ptr_y < _offset_y ) _offset_y = _ptr_y;
	if( _ptr_y > _offset_y + _max_line - 1 ) _offset_y = _ptr_y - _max_line + 1;
}

/** 現在の行数 */
int TextArea::LineCount()
{
	int cnt = 0;
	for(int i=0; i<_text_len; i++)
		if( text[i] == '\n' ) cnt++;
	return cnt + 1;
}

/** y 行目の先頭文字を n 文字目に換算 */
int TextArea::Y2P(int y)
{
	int cnt = 0;
	if( y == 0 ) return 0;
	if( y >= LineCount() ) return _text_len + 1;
	for(cnt=0; cnt<_text_len; cnt++)
		if( text[cnt] == '\n' ) if( --y == 0 ) break;
	return cnt + 1;
}

/** xy 座標を n 文字目に換算 */
int TextArea::XY2P(int x, int y)
{
	int cnt, i;
	for( i=0,cnt=Y2P(y); cnt<_text_len && (i<x || x==-1); cnt++ ) {
		if( text[cnt] == '\n' ) break;
		if( text[cnt] == '\t' ) while( ++i % TAB_SIZE != 0 ) ; else i++;
	}
	return cnt;
}

/** バッファ先頭位置からキャレットを設定 */
void TextArea::SetPos(int pos)
{
	int cnt = 0, last = 0; _ptr_y = 0; _ptr_x = 0;
	if( pos == -1 ) pos = _text_len;
	for(cnt=0; cnt<_text_len && cnt<pos; cnt++)
		if( text[cnt] == '\n' ) { _ptr_y++; last = cnt + 1; }
	for(cnt=last; cnt<_text_len && cnt<pos; cnt++)
		if( text[cnt] == '\t' ) while( ++_ptr_x % TAB_SIZE != 0 ) ; else _ptr_x++;
}
