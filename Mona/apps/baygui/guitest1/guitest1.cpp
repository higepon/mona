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

/** メイン（テストプログラム） */
#if defined(MONA)
int MonaMain(List<char*>* pekoe)
#else
int main(int argc, char **argv)
#endif
{
	// ウィンドウを作成
	Window *window = new Window();
	window->setRect((800 - 300) / 2,(600 - 200) / 2,300,200);
	window->setTitle("(oAo) ﾅｶﾏ");	
	
	// 部品を追加
	Button *button1 = new Button("ｵﾏｴﾓﾅ orz");
	button1->setRect(5,5,80,20);
	Label *label1 = new Label("Label<-");
	Font *font;
	font = label1->getFont();
	font->setStyle(FONT_BOLD);
	label1->setRect(5,26,100,20);
	Label *label2 = new Label("=Label=", ALIGN_CENTER);
	label2->setBackground(0x0);
	label2->setForeground(0xFFFFFF);
	label2->setRect(5,46,100,20);
	Label *label3 = new Label("Label->", ALIGN_RIGHT);
	label3->setRect(5,66,100,20);
	TextField *text1 = new TextField();
	text1->setRect(5,86,100,20);
	text1->setText("Hello,World");
	TextField *text2 = new TextField();
	text2->setRect(5,107,100,20);
	TextField *text3 = new TextField();
	text3->setRect(5,128,100,20);
	ListBox *list1 = new ListBox();
	list1->setRect(110,5,100,100);
	list1->add("ひげぽん");
	list1->add("ｷﾀｰｰ!!");
	list1->add("Mona is here");
	window->add(button1);
	window->add(label1);
	window->add(label2);
	window->add(label3);
	window->add(text1);
	window->add(text2);
	window->add(text3);
	window->add(list1);
	window->run();
	delete(window);
	
	return 0;
}
