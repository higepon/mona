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

/** クラス定義部 */
class GuiTest : public Window {
private:
	Button *button1;
	Label *label1, *label2, *label3;
	TextField *text1, *text2, *text3;
	Checkbox *check1, *check2;
	CheckboxGroup *group1;
	ListBox *list1;
	Scrollbar *scroll1, *scroll2;
	Window *dialog;

public:
	/** コンストラクタ */
	GuiTest::GuiTest()
	{
		setBounds((800 - 300) / 2,(600 - 200) / 2,300,200);
		setTitle("(oAo) ﾅｶﾏ");
		
		// 部品を初期化
		button1 = new Button("ｵﾏｴﾓﾅ orz");
		button1->setBounds(5,5,80,20);
		label1 = new Label("Label<-");
		label1->setFontStyle(Font::BOLD);
		label1->setBounds(5,26,100,20);
		label2 = new Label("=Label=", Label::CENTER);
		label2->setFontStyle(Font::ITALIC);
		label2->setBackground(Color::black);
		label2->setForeground(Color::white);
		label2->setBounds(5,46,100,20);
		label3 = new Label("Label->", Label::RIGHT);
		label3->setFontStyle(Font::BOLD | Font::ITALIC);
		label3->setBounds(5,66,100,20);
		text1 = new TextField();
		text1->setBounds(5,86,100,20);
		text1->setText("Hello,World");
		text2 = new TextField();
		text2->setBounds(5,107,100,20);
		text3 = new TextField();
		text3->setBounds(5,128,100,20);
		check1 = new Checkbox("本気ですか？");
		check1->setBounds(110,5,100,20);
		check2 = new Checkbox("買っちゃう？");
		check2->setBounds(110,25,100,20);
		group1 = new CheckboxGroup();
		group1->add(check1);
		group1->add(check2);
		list1 = new ListBox();
		list1->setBounds(110,50,100,100);
		list1->add("ひげぽん");
		list1->add("ｷﾀｰｰ!!");
		list1->add("Mona is here");
		scroll1 = new Scrollbar();
		scroll1->setBounds(273,0,16,158);
		scroll2 = new Scrollbar(Scrollbar::HORIZONTAL);
		scroll2->setBounds(0,157,274,16);
		
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
		add(scroll1);
		add(scroll2);
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
int main(int argc, char** argv)
#endif
{
	GuiTest* window = new GuiTest();
	window->run();
	delete(window);
	
	return 0;
}
