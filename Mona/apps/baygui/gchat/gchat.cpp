/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
