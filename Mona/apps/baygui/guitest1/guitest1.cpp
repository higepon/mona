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

/** クラス定義部 */
class GuiTest : public Window {
private:
	Button *button1;
	Label *label1, *label2, *label3;
	TextField *text1, *text2, *text3;
	Checkbox *check1, *check2;
	CheckboxGroup *group1;
	ListBox *list1;
	Window *dialog;

public:
	/** コンストラクタ */
	GuiTest::GuiTest()
	{
		setRect((800 - 300) / 2,(600 - 200) / 2,300,200);
		setTitle("(oAo) ﾅｶﾏ");
		
		// 部品を初期化
		button1 = new Button("ｵﾏｴﾓﾅ orz");
		button1->setRect(5,5,80,20);
		label1 = new Label("Label<-");
		label1->setFontStyle(Font::BOLD);
		label1->setRect(5,26,100,20);
		label2 = new Label("=Label=", Label::ALIGN_CENTER);
		label2->setFontStyle(Font::ITALIC);
		label2->setBackground(Color::BLACK);
		label2->setForeground(Color::WHITE);
		label2->setRect(5,46,100,20);
		label3 = new Label("Label->", Label::ALIGN_RIGHT);
		label3->setFontStyle(Font::BOLD | Font::ITALIC);
		label3->setRect(5,66,100,20);
		text1 = new TextField();
		text1->setRect(5,86,100,20);
		text1->setText("Hello,World");
		text2 = new TextField();
		text2->setRect(5,107,100,20);
		text3 = new TextField();
		text3->setRect(5,128,100,20);
		check1 = new Checkbox("本気ですか？");
		check1->setRect(110,5,100,20);
		check2 = new Checkbox("買っちゃう？");
		check2->setRect(110,25,100,20);
		group1 = new CheckboxGroup();
		group1->add(check1);
		group1->add(check2);
		list1 = new ListBox();
		list1->setRect(110,50,100,100);
		list1->add("ひげぽん");
		list1->add("ｷﾀｰｰ!!");
		list1->add("Mona is here");
		
		// 部品を追加
		add(button1);
		add(label1);
		add(label2);
		add(label3);
		add(text1);
		add(text2);
		add(text3);
		add(check1);
		add(check2);
		add(list1);
	}
	
	/** デストラクタ */
	GuiTest::~GuiTest()
	{
	}
};

/** メイン（テストプログラム） */
#if defined(MONA)
int MonaMain(List<char*>* pekoe)
#else
int main(int argc, char **argv)
#endif
{
	GuiTest *window = new GuiTest();
	window->run();
	delete(window);
	
	return 0;
}
