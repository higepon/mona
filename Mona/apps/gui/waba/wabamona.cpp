/*
Copyright (C) 1998, 1999, 2000 Wabasoft

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details. 

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA. 
*/

#if defined(MONA)
#include <monapi.h>
#elif defined(MONAGUI)
#include <string.h>
#endif
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

#ifdef WIN32
#undef WIN32
#endif

#define  WABA_VERSION    "0.1.5"
#define  VM_OS           "MONA"
#define  VM_USER         "MONA"
#define  g_mainWinOffX   0
#define  g_mainWinOffY   0
#define  g_mainWinWidth  200
#define  g_mainWinHeight 200

extern "C" {
#include "waba.c"
}

static char* get_char_array(String s)
{
	static char ret[64];
	int len = s.get_Length();
	if (len > 63) len = 63;
	for (int i = 0; i < len; i++)
	{
		ret[i] = s[i] < 128 ? s[i] : '?';
	}
	ret[len] = '\0';
	return ret;
}

class WabaPanel : public Control {
public:
	WabaPanel() {
	/* コンストラクタ */
		this->set_Size(Size(g_mainWinWidth, g_mainWinHeight));
	}

	virtual ~WabaPanel() {
	/* デストラクタ */
	}

	void Next() {
	/* 次のフレームを表示 */
		this->Refresh();
	}

protected:
	virtual void Create() {
		Control::Create();
		image = this->buffer;
	}
};

class Form1 : public Form {
public:
	Form1() {
	/* コンストラクタ */
		this->InitializeComponent();
	}

private:
	_P<WabaPanel> wabaPanel;

	void Form1::mouseDown(_P<Object> sender, _P<MouseEventArgs> e) {
	/* マウスクリック */
		int vlist[6];

		printf("x, y %d,%d\n", e->X, e->Y);
		vlist[0] = 200; // type
		vlist[1] = 0; // key
		vlist[2] = e->X; // x
		vlist[3] = e->Y; // y
		vlist[4] = 0; // modifiers
		vlist[5] = 0; // timeStamp
		processEvent(vlist);
	}

	void Form1::mouseMove(_P<Object> sender, _P<MouseEventArgs> e) {
	/* マウスドラッグ */
		int vlist[6];

		//printf("x, y %d,%d\n", e->X, e->Y);
		vlist[0] = 201; // type
		vlist[1] = 0; // key
		vlist[2] = e->X; // x
		vlist[3] = e->Y; // y
		vlist[4] = 0; // modifiers
		vlist[5] = 0; // timeStamp
		processEvent(vlist);
	}

	void Form1::mouseUp(_P<Object> sender, _P<MouseEventArgs> e) {
	/* マウスアップ */
		int vlist[6];

		//printf("x, y %d,%d\n", e->X, e->Y);
		vlist[0] = 202; // type
		vlist[1] = 0; // key
		vlist[2] = e->X; // x
		vlist[3] = e->Y; // y
		vlist[4] = 0; // modifiers
		vlist[5] = 0; // timeStamp
		processEvent(vlist);
	}

#if 0
	void keyDown(_P<KeyEventArgs> e){
	/* キークリック */
		int vlist[6];

		printf("keycode %d\n", e->keycode);
		vlist[0] = 100; // type
		vlist[1] = e->keycode; // key
		vlist[2] = 0; // x
		vlist[3] = 0; // y
		vlist[4] = e->modifiers; // modifiers
		vlist[5] = 0; // timeStamp
		processEvent(vlist);
	}
#endif

	void InitializeComponent() {
	/* コンポーネント初期化 */
		// ファイルを選択
		// 各種初期化
		this->wabaPanel = new WabaPanel();
		this->set_Location(Point(200, 100));
		this->set_ClientSize(Size(g_mainWinWidth, g_mainWinHeight));
		this->set_Text("WABA");
		this->get_Controls()->Add(this->wabaPanel.get());
		this->wabaPanel->add_MouseDown(new MouseEventHandler<Form1>(this, &Form1::mouseDown));
		this->wabaPanel->add_MouseMove(new MouseEventHandler<Form1>(this, &Form1::mouseMove));
		this->wabaPanel->add_MouseUp(new MouseEventHandler<Form1>(this, &Form1::mouseUp));
	}

public:
	static void Main(_A<String> args) {
		char* filename;
		if (args.get_Length() != 1)
		{
#ifdef MONA
			filename = "/APPS/HELLO.WRP"; 
#elif defined(MONAGUI)
			filename = "hello.wrp";
#endif
		}
		else
		{
			filename = get_char_array(args[0]);
		}
		_P<Form1> f = new Form1();
		f->Create();
		startApp(filename);
		Application::Run(f.get());
	 }
};

SET_MAIN_CLASS(Form1)
