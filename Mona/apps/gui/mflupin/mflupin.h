/*!
    \file  mflupin.h
    \brief Mona ルパンタイトル MonaForms版 定義ファイル

    Copyright (c) 2004 Yamami
    WITHOUT ANY WARRANTY

    \author  Yamami
    \version $Revision$
    \date   create:2004/11/04 update:$Date$
*/

// This file's encoding is UTF-8.

#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <gui/System/Mona/Forms/ControlPaint.h>
#include <monapi/CString.h>

#include "TITLE.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

#define SCREEN_W 256
#define SCREEN_H 256

//---------------------------------------------------------------------------
const int MAXSTRING = 256;          //表示文字列の最大長
//---------------------------------------------------------------------------


#define FONT_WIDTH   6
#define FONT_HEIGHT  12
#define TEXT_LENGTH  MAXSTRING

//GetDefaultScreen は、Control.cpp で定義されている。
extern _P<MonAPI::Screen> GetDefaultScreen();



/*! \class Mflupin
 *  \brief ルパンタイトル MonaForms版
 *      Form を継承
 */
class Mflupin : public Form
{
private:
    _P<Timer> timer1;
    //char time[64];
    //String clock;
    MonAPI::Date date;

    int nowTitle;           //現在表示中のタイトル
    int nowViewChar;        //現在表示中の文字位置
    int geted;              //現在の状態 タイトル取得済みかどうか？ 0:未取得 0以外取得済み
    int titleStrCount;      //現在表示中のタイトル文字数
    MonAPI::Random *rnd;

    String ltytle;
    String Viewtytle;

    int myCount;            //タイマー自己カウント用
    int GetUpCount;         //タイマー、これだけカウントすれば処理起動
public:
    
    Mflupin();


private:
    
    void InitializeComponent();

    void refresh(_P<Object> sender, _P<EventArgs> e);

    String GetString(int num);

    void DrawString(_P<Graphics> g, String s, Color c, int pos_x, int pos_y);

public:
    virtual void Create();
    virtual void Dispose();

protected:
    virtual void OnPaint();

/*
    virtual NCState NCHitTest(int x, int y);

    virtual void OnNCMouseDown(_P<MouseEventArgs> e);

    virtual void OnNCMouseUp(_P<MouseEventArgs> e);
*/

public:
    static void Main(_A<String> args);
};

