/*!
    \file  mflupin.cpp
    \brief Mona ルパンタイトル MonaForms版

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

#include "mflupin.h"

using namespace System;
using namespace System::Text;
using namespace System::Drawing;
using namespace System::Mona::Forms;


/*!
\brief Mflupin コンストラクタ

\author Yamami
\date   create:2004/11/04 update:$Date$
*/

Mflupin::Mflupin()
{
    //InitializeComponent メソッドコール
    this->InitializeComponent();
    
    //ランダムオブジェクト
    rnd = new MonAPI::Random();
    
    //ランダムのシード値を与える。
    MonAPI::Date date;
    date.refresh();
    rnd->setSeed(date.hour() * 3600 + date.min() * 60 + date.sec());
}



    
/*!
\brief InitializeComponent

\author Yamami
\date   create:2004/11/04 update:$Date$
*/
void Mflupin::InitializeComponent()
{
    //表示位置指定
    this->set_Location(Point(128, 128));
    //表示サイズ指定
    this->set_ClientSize(Size(SCREEN_W, SCREEN_H));
    //ウィンドウタイトル
    this->set_Text("MFLUPIN");

    //this->set_Opacity(0.8);
    //前景色は白
    //this->set_ForeColor(Color::get_White());
    this->set_ForeColor(Color::FromArgb( 255, 255, 255));

    //背景は黒
    //this->set_ForeColor(Color::get_Black());
    this->set_BackColor(Color::FromArgb(0, 0, 0));
    
    //タイマー起動
    this->timer1 = new Timer();
    this->timer1->set_Interval(100);
    // タイマーのイベントハンドラは、refresh
    this->timer1->add_Tick(new EventHandler<Mflupin>(this, &Mflupin::refresh));
    //myEvent = new EventHandler<Mflupin>(this, &Mflupin::refresh);
    //this->timer1->add_Tick(myEvent);
    
    //タイマー自己カウント用をクリア
    myCount = 0;
    GetUpCount = 0;
}


/*!
\brief refresh
   タイマーハンドラー
\author Yamami
\date   create:2004/11/04 update:$Date$
*/
void Mflupin::refresh(_P<Object> sender, _P<EventArgs> e)
{
    int tmprand;        //ランダム用シード値
    
    //タイマーイベント処理
    if (geted == 0){
        //タイトル未取得なら、タイトル取得
        if(nowTitle > MAX_TITLE){
            nowTitle = 0;
        }
        
        //タイトルはランダムで取得
        tmprand = rnd->nextInt();
        nowTitle = tmprand % MAX_TITLE;
        
        ltytle = this->GetString(nowTitle);
        geted = 1;
        
        //文字数カウント
        titleStrCount = ltytle.get_Length();
        
    }
    
    
    else{
        //タイトル取得済みなら、表示
        //ただし、状況に応じて、タイマー処理起動するかどうかを判定
        if (myCount < GetUpCount){
            //まだ、Wait に達していなければ カウントアップのみでリターン
            myCount++;
            //printf("myCount=%d , GetUpCount=%d\n",myCount,GetUpCount);
            return;
        }
        
        
        if(nowViewChar > titleStrCount){

            //
            GetUpCount = 15;
            myCount = 0;
            
            //タイトル全体表示。表示位置左
            this->Viewtytle = ltytle;
            this->Refresh();
            geted = 0;
            nowViewChar = 0;
            

        }
        else{
            //
            GetUpCount = 2;
            myCount = 0;
            
            //一文字表示、表示位置中央
            this->timer1->set_Interval(250);
            
            String s;
            s = ltytle.Substring(nowViewChar,1);
            
            this->Viewtytle = s;
            this->Refresh();
            nowViewChar++;
            
        }
    }

}


/*!
    \brief GetString
        指定した番号のタイトルを得る。

    \param int num タイトル番号
    \return 結果
    
    \author  Yamami
    \date    create:2004/11/02 update:2004/11/02
*/
String Mflupin::GetString(int num)
{
    char tytleBuf[MAXSTRING];
    
    if(num < MAX_TITLE){
        strcpy(tytleBuf, Titles[num]);
    }

    return tytleBuf;
    

}



void Mflupin::DrawString(_P<Graphics> g, String s, Color c, int pos_x, int pos_y)
{
    _P<Font> f = Control::get_DefaultFont();
    int x = pos_x, y = pos_y;
    
    //タテ位置 真ん中へ
    y = y + SCREEN_H/2;
    
    //横表示位置は真ん中へ
    x = x + (SCREEN_W - (s.get_Length() * FONT_WIDTH * 2))/2;
    
    
    FOREACH (wchar, ch, s)
    {
        if (ch == '\n')
        {
            x = pos_x;
            y += FONT_HEIGHT;
        }
        else
        {
            _A<wchar> buf(1);
            buf[0] = ch;
            int sw = g->MeasureString(buf, Control::get_DefaultFont()).Width;
            int sw2 = ch < 256 ? FONT_WIDTH : FONT_WIDTH * 2;
            g->DrawString(buf, Control::get_DefaultFont(), Color::FromArgb( 255, 255, 255), x + (sw2 - sw) / 2, y);
            x += sw2;
        }
    }
    END_FOREACH
}


void Mflupin::Create()
{
    Form::Create();
    this->timer1->Start();
}

void Mflupin::Dispose()
{
    this->timer1->Dispose();
    Form::Dispose();
}


void Mflupin::OnPaint()
{
    Form::OnPaint();

    _P<Graphics> g = this->CreateGraphics();
    int w = this->get_Width(), h = this->get_Height();

    // Clear
    g->FillRectangle(this->get_BackColor(), 0, 0, w, h);

    // Border
    //ControlPaint::DrawEngraved(g, 0, 0, w, h);

    // ウィンドウへ描写
    this->DrawString(g, this->Viewtytle, this->get_ForeColor(), 0, 0);
    g->Dispose();
}


/*
NCState Mflupin::NCHitTest(int x, int y)
{
    return NCState_TitleBar;
}


void Mflupin::OnNCMouseDown(_P<MouseEventArgs> e)
{
    if (e->Button != 2) Form::OnNCMouseDown(e);
}


void Mflupin::OnNCMouseUp(_P<MouseEventArgs> e)
{

    int x = e->X + this->offset.X, y = e->Y + this->offset.Y;

    printf("OnNCMouseUp!! e->Button=%d x=%d  y=%d \n",e->Button,x,y);

    if (e->Button == 2 && 0 <= x && x < this->get_Width() && 0 <= y && y < this->get_Height())
    {
        this->Dispose();
    }
    else
    {
        Form::OnNCMouseUp(e);
    }
}
*/


void Mflupin::Main(_A<String> args)
{
    //Mflupin フォームをRun
    Application::Run(new Mflupin());
}

SET_MAIN_CLASS(Mflupin)
