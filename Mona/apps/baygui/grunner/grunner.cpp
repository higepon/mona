/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this history of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this history of conditions and the following disclaimer in the
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
 名前を指定して実行（コマンド履歴つき）
*/
class GRunner : public Window {
private:
	TextField *text;
	/** コマンド履歴 */
	LinkedList *history;
	/** コマンド履歴ポインター */
	int historyPtr;

public:
	GRunner::GRunner();
	virtual GRunner::~GRunner();
	virtual void onEvent(Event *event);
};

GRunner::GRunner()
{
	setRect((800 - 200) / 2, (600 - 48) / 2, 200, 48);
	setTitle("ファイル名を指定して実行");
	text = new TextField();
	text->setText("/APPS/BAYGUI/");
	text->setRect(0,0,188,20);
	add(text);
	history = new LinkedList();
	historyPtr = 0;
}

GRunner::~GRunner()
{
	delete(text);
	delete(history);
}

void GRunner::onEvent(Event *event)
{
	// 実行
	if (event->type == TEXT_CHANGED) {
		// 履歴追加
		history->add(new LinkedItem(new String(text->getText())));
		historyPtr = history->getLength();
		monapi_call_process_execute_file(text->getText(), MONAPI_FALSE);
		text->setText("/APPS/BAYGUI/");
	// キー押下
	} else if (event->type == KEY_PRESSED) {
		int keycode = ((KeyEvent *)event)->keycode;
		// １つ前の履歴
		if (keycode == VKEY_UP) {
			if (historyPtr > 0) {
				historyPtr--;
				text->setText(((String *)history->get(historyPtr))->getBytes());
			}
		// １つ次の履歴
		} else if (keycode == VKEY_DOWN) {
			if (historyPtr < history->getLength() - 1) {
				historyPtr++;
				text->setText(((String *)history->get(historyPtr))->getBytes());
			} else {
				text->setText("/APPS/BAYGUI/");
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
	GRunner *runner = new GRunner();
	runner->run();
	delete(runner);
	return 0;
}
