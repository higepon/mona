/*
Copyright (c) 2004 bayside
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

/**
 チャットクラス.
 <pre>
 [画面構成]
 
 +---------------------------+
 +     #name - IRCもどき     |
 +-------------+-------------+
 +             +             +
 + messageList + memberList  +
 +             +             +
 +-------------+-------------+
 + text        + channelList +
 +-------------+             +
 + consoleList +             +
 +-------------+-------------+
 </pre>
*/
class GChat : public Window {
private:
	ListBox *messageList, *memberList, *channelList, *consoleList;
	/** コマンド */
	TextField *text;
	/** コマンド履歴 */
	LinkedList *history;
	/** コマンド履歴ポインター */
	int historyPtr;

public:
	GChat::GChat();
	virtual GChat::~GChat();
	virtual void onEvent(Event *e);
};

GChat::GChat()
{
	setRect((800 - 312) / 2, (600 - 328) /2, 312, 328);
	setTitle("#osdev-j - IRCもどき");
	messageList = new ListBox();
	memberList  = new ListBox();
	channelList = new ListBox();
	consoleList = new ListBox();
	text = new TextField();
	// メッセージとコンソールは選択枠を出さないようにする
	messageList->setEnabled(false);
	consoleList->setEnabled(false);
	// 部品位置設定
	messageList->setRect(0,0,220,220);
	memberList->setRect(220,0,80,220);
	consoleList->setRect(0,240,220,60);
	channelList->setRect(220,220,80,80);
	text->setRect(0,220,220,20);
	// テストデータ挿入
	memberList->add("@mona");
	memberList->add("@pekoe");
	memberList->add("osask");
	memberList->add("nwsos");
	consoleList->add("12:34 irc.hoge.jp:6667 に接続しました.");
	channelList->add("0 *Console*");
	channelList->add("1 #osdev-j");
	text->setText("");
	// 部品貼り付け
	add(messageList);
	add(memberList);
	add(channelList);
	add(consoleList);
	add(text);
	history = new LinkedList();
	historyPtr = 0;
}

GChat::~GChat()
{
	delete(messageList);
	delete(memberList);
	delete(channelList);
	delete(consoleList);
	delete(text);
	delete(history);
}

void GChat::onEvent(Event *event)
{
	// 実行
	if (event->getType() == Event::TEXT_CHANGED) {
		// 履歴追加
		history->add(new String(text->getText()));
		historyPtr = history->getLength();
		messageList->add(text->getText());
		messageList->repaint();
		text->setText("");
	// キー押下
	} else if (event->getType() == KeyEvent::KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->getKeycode();
		// １つ前の履歴
		if (keycode == KeyEvent::VKEY_UP) {
			if (historyPtr > 0) {
				historyPtr--;
				text->setText(((String *)history->get(historyPtr))->getBytes());
			}
		// １つ次の履歴
		} else if (keycode == KeyEvent::VKEY_DOWN) {
			if (historyPtr < history->getLength() - 1) {
				historyPtr++;
				text->setText(((String *)history->get(historyPtr))->getBytes());
			} else {
				text->setText("");
			}
		}
	}
}

/** メイン */
#if defined(MONA)
int MonaMain(List<char*>* pekoe)
#else
int main(int argc, char **argv)
#endif
{
	GChat *chat = new GChat();
	chat->run();
	delete(chat);
	return 0;
}
