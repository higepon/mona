/* Kukuri for Mona
 * 
 * Copyright (c) 2000 h-muro
 * Copyright (c) 2004 Tino, Higepon
 * 
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies of
 * the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
 * OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// This file's encoding is UTF-8.

#include "Kukuri.h"
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Cursor.h>
#include <gui/messages.h>
#include <monapi.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern dword __gui_server;
extern _P<MonAPI::Screen> GetDefaultScreen();

static MonAPI::Random rand;


Kukuri::Kukuri()
{
  nowCount = 0;
  count = 2;
  auto_x = 0;
  auto_y = 0;
  tmpo = 0;
  nowPix = 0;
  
  px = 0; py = 0;   // x増分, y増分
  d =  2;           // 方向
  r = 90;           // 角度
  t_num =  0;       // 回転時のXPM の順番 (turning_number)
                    // 及び turn(),byebye(),wow()での経過カウント
  distance = 0;     // マウスとポインタの距離
  s_pixmap = 0;     // 寝てるときの画像の番号
  n_state = 1;      // 普通時の状態 (normal_state)
  a_state = 0;      // 自動時の状態 (auto_state)
  w_num = 0;        // 歩くときのXPM計算用 (walking_number)
  a_t_num = 0;      // 回転の後経過した歩数(カウント)
                    //      (number_after_turning)
  a_t_dir = 0;      // 回転後どちらにずれるか
                    //      (direction_after_turning)
  a_t_dir2= 0;      // 上に同じ
  s_interval = 0;   // 眠るまでのカウント (sleep_interval)
  s_count = 0;      // 眠ってからのカウント (sleep_count)
  a_flag = true;    // 自動モードで方向転換するか(自動モード中)
                    // 自動モードであるかどうか(失敗作成中)
                    //      (auto_flag)
  th_num = 0;       // 考え中の XPM 番号 (thinking_number)
  
  MonAPI::Date date;
  date.refresh();
  rand.setSeed(date.hour() * 3600 + date.min() * 60 + date.sec());
  
  this->offset = Point::get_Empty();
  this->set_ClientSize(Size(KUKURI_WIDTH, KUKURI_HEIGHT));
  
  _P<MonAPI::Screen> scr = GetDefaultScreen();
  this->set_Location(Point(
    (scr->getWidth () - KUKURI_WIDTH ) / 2,
    (scr->getHeight() - KUKURI_HEIGHT) / 2));
  
  this->set_TransparencyKey(KUKURI_NONE);
  this->set_Text("ククリ様");
  
  this->timer = new Timer();
  this->timer->set_Interval(100);
  this->timer->add_Tick(new EventHandler<Kukuri>(this, &Kukuri::timer_Tick));
  
  setBitmaps();
}


/**
  右から時計回りにぐるっと回る順番
*/

void Kukuri::setBitmaps()
{
  String bundlePath = MonAPI::System::getBundlePath();
  this->kukuri = new Bitmap(bundlePath + "/KUKURI.BM2");
  
  int w = this->kukuri->get_Width(), h = this->kukuri->get_Height();
  this->kukuriMirror = new Bitmap(w, h);
  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      this->kukuriMirror->SetPixel(x, y, this->kukuri->GetPixel(w - x - 1, y));
    }
  }
  
  // 歩いているときの画像
  kuPix[ 0] = kukuri_r  ;  // 右   1
  kuPix[ 1] = kukuri_fr ;  // 右下 1
  kuPix[ 2] = kukuri_f  ;  // 　下 1
  kuPix[ 6] = kukuri_b  ;  // 　上 1
  kuPix[ 7] = kukuri_br ;  // 右上 1
  kuPix[ 8] = kukuri_r2 ;  // 右   2
  kuPix[ 9] = kukuri_fr2;  // 右下 2
  kuPix[10] = kukuri_f2 ;  // 　下 2
  kuPix[14] = kukuri_b2 ;  // 　上 2
  kuPix[15] = kukuri_br2;  // 右上 2
  kuPix[24] = kukuri_r3 ;  // 右   3
  kuPix[25] = kukuri_fr3;  // 右下 3
  kuPix[31] = kukuri_br3;  // 右上 3
  kuPix[ 3] = kuPix[ 1] + MIRROR; // 左下 1
  kuPix[ 4] = kuPix[ 0] + MIRROR; // 左   1
  kuPix[ 5] = kuPix[ 7] + MIRROR; // 左上 1
  kuPix[11] = kuPix[ 9] + MIRROR; // 左下 2
  kuPix[12] = kuPix[ 8] + MIRROR; // 左   2
  kuPix[13] = kuPix[15] + MIRROR; // 左上 2
  kuPix[26] = kuPix[10] + MIRROR; // 　下 2
  kuPix[27] = kuPix[25] + MIRROR; // 左下 3
  kuPix[28] = kuPix[24] + MIRROR; // 左   3
  kuPix[29] = kuPix[31] + MIRROR; // 左上 3
  kuPix[30] = kuPix[14] + MIRROR; // 　上 2
  
  kuTurn[ 0] = kukuri_turn_f ; // 　下
  kuTurn[ 4] = kukuri_turn_b ; // 　上
  kuTurn[ 5] = kukuri_turn_br; // 右上
  kuTurn[ 6] = kukuri_turn_r ; // 右
  kuTurn[ 7] = kukuri_turn_fr; // 右下
  kuTurn[ 8] = kukuri_turn_f ; // 　下
  kuTurn[10] = kukuri_turn_r2; // 右杖
  kuTurn[ 1] = kuTurn[ 7] + MIRROR;   // 左下
  kuTurn[ 2] = kuTurn[ 6] + MIRROR;   // 左
  kuTurn[ 3] = kuTurn[ 5] + MIRROR;   // 左上
  kuTurn[ 9] = kuTurn[10] + MIRROR;   // 左杖
  
  // こっくり
  kuSleep[0] = kukuri_sl1; 
  kuSleep[1] = kukuri_sl2;
  
  // ばいばい
  kuBye[0] = kukuri_bye_1;
  kuBye[1] = kukuri_bye_2;
  kuBye[2] = kukuri_bye_3;
  kuBye[3] = kukuri_bye_4;
  
  // ワッ！
  kuWow[1] = kukuri_wow;
  kuWow[0] = kuWow[1] + MIRROR;
  
  //沈黙は金
  kuThink[0] = kukuri_th1;
  kuThink[1] = kukuri_th2;
  kuThink[2] = kukuri_th3;
  kuThink[3] = kukuri_th4;
}


Kukuri::~Kukuri()
{
}


void Kukuri::Create()
{
  Form::Create();
  this->timer->Start();
}


void Kukuri::Dispose()
{
  this->timer->Dispose();
  this->kukuri->Dispose();
  this->kukuriMirror->Dispose();
  Form::Dispose();
}


/**
  この関数は直接呼んではいけない。
  必ずRefresh()により間接的に呼ぶこと。
*/

void Kukuri::OnPaint()
{
  _P<Graphics> g = this->CreateGraphics();
  g->DrawImage(this->nowPix < MIRROR ? this->kukuri : this->kukuriMirror,
    0, 0 - KUKURI_HEIGHT * (this->nowPix % MIRROR));
  g->Dispose();
  
  // 最前面に移動
  MonAPI::Message::send(__gui_server, MSG_GUISERVER_WINDOWTOFRONTMOST, this->get_Handle());
}


Control::NCState Kukuri::NCHitTest(int x, int y)
{
  return NCState_TitleBar;
}


void Kukuri::OnNCMouseDown(_P<MouseEventArgs> e)
{
  if (e->Button != 2) Form::OnNCMouseDown(e);
}


void Kukuri::OnNCMouseUp(_P<MouseEventArgs> e)
{
  int x = e->X + this->offset.X, y = e->Y + this->offset.Y;
  if (e->Button == 2 && 0 <= x && x < this->get_Width() && 0 <= y && y < this->get_Height())
  {
     if (this->n_state != 6) this->n_state = 6;
  }
  else
  {
    Form::OnNCMouseUp(e);
  }
}


/**
  タイマー
*/

void Kukuri::timer_Tick(_P<Object> sender, _P<EventArgs> e)
{
  this->nowCount++;
  if (this->nowCount < this->count) return;
  
  this->nowCount = 0;
  this->count = 2;
  
  switch(this->n_state){
  case 1:  // 普通の時
    this->normal();
    break;
  case 2:  // ぐうぐう。
    this->sleeping();
    break;
  case 3:  // くるくる。
  case 4:  // メニュー編集
  case 5:  // About表示
    count = 1;
    this->turn();
    break;
  case 7:  // 失敗生成
    this->turn();
    break;
  case 6:  // ばいばい
    count = 4;
    this->byebye();
    break;
  case 8:  // びっくり
    this->wow();
    break;
  case 9:  // 勝手モード
    //this->self();
    break;
  default: // 回避。落とす。
    this->Hide();
    Application::Exit();
    return;
  }
}


/**
  通常の状態(追っかける)
*/

void Kukuri::normal()
{
  /** 古い方向 */
  int old_d = 0;
  int w_tmp = 0;
  
  Point mp = Cursor::get_Position();
  this->calculation(mp.X, mp.Y);
  
  if(distance > 16){    // 近すぎるときは向きを変えない
    old_d = d;          //360度を8等分すると45度なので、
    d = int((r+22)/45); //22.5度から45度ずつで向きを決める。
    if(d > 7) d -= 8;
  }
  
  // w_num=0, 2 は気を付け。1 は右足, 3 は左足が前。
  // 2 のときは w_tmp = 0 を掛けてチャラにする
  if(w_num == 2) w_tmp = 0; else w_tmp = 1;
  nowPix = kuPix[d + (8 * w_num) * w_tmp];
  
  // 距離が70以上の時だけ動かす
  if(distance > 70){
    if(w_num == 3) w_num = 0; else w_num++;
    this->set_Location(Point((int)(this->get_X() + px), (int)(this->get_Y() + py)));
    s_interval = 0;
  }
  this->Refresh();
}


/**
  位置の計算
  
  画像の中心から計算するように補正。
*/

void Kukuri::calculation(int tmp_x, int tmp_y)
{
  double tx = tmp_x - (this->get_X() + KUKURI_WIDTH  / 2);
  double ty = tmp_y - (this->get_Y() + KUKURI_HEIGHT / 2);
  distance = sqrt(tx * tx + ty * ty);
  /** 角度（ラジアン） */
  double rr = atan2(tx, ty);
  
  r = int(DEG(rr));
  
  px = IDOU * cos(rr);
  py = IDOU * sin(rr);
}


/**
  一回転して横にずれる
*/

void Kukuri::turn()
{
  int t = 0;
  
  if(t_num < 9){  // 回転 Pixmap は9枚
    nowPix = kuTurn[t_num];
    t_num++;
    this->Refresh();
    return;
  }
  
  if(a_t_num == 11){  // 移動も終了,杖もついたら
    t_num = a_t_num = 0;
    switch(n_state){  // ここからそれぞれ実行
    //case 4:  editMenu();n_state=1;break;
    //case 5:  about(0, 0);break;
    //case 7:  generate();          break;
    default:
      n_state = 1;
    }
    a_t_dir = 0;
    return;
  }else if(a_t_num > 6){  // 移動だけ終了してたら杖をつく
    switch(a_t_num){  // 原始的な処理
    case 7:
    case 10:
      if(a_t_dir2) t = 2; else t = 6;
      break;
    case 8:
    case 9:
      if(a_t_dir2) t = 9; else t = 10;
      break;
    default:
      this->Hide();
      Application::Exit();
      return;
    }
    nowPix = kuTurn[t];
    a_t_num++;
  }else{  // 横にずれる
    if(a_t_dir == 0){  // 初めてだったら処理
      a_t_dir= 1; a_t_dir2=1; // 左向きで右移動
    }
    switch(a_t_num){  // 原始的な処理
    case 0:
    case 4:
    case 2:
    case 6:
      t = 0;
      break;
    case 1:
    case 5:
      t = 8;
      break;
    case 3:
    case 7:
      t = 24;
      break;
    default:
      this->Hide();
      Application::Exit();
      return;
    }
    nowPix = kuPix[t + a_t_dir2 * 4];
    this->set_Location(Point(this->get_X() + (a_t_dir * 4), this->get_Y()));
    a_t_num++;
  }
  this->Refresh();
}


/**
  ククリ様ねむねむ
*/

void Kukuri::sleeping()
{
  // 10カウントでこっくりする
  if(s_count != 10){
    s_count++;  // カウントする
    return;
  }
  
  s_pixmap = !s_pixmap; // 0, 1 で Pixmapを入れ換え
  nowPix = kuSleep[s_pixmap];
  s_count = 0;
  
  // もし自動モードなら起きるかどうか判定
  if(a_state == 3 && (int)(rand.nextDouble() * 10) == 0){
    n_state = 9;
    a_state = 0;
  }
}


/**
  ばいばーい
*/

void Kukuri::byebye()
{
  if(t_num >= 8){
    this->Hide();
    Application::Exit();
    return;
  }
  
  switch(t_num){
  case 4:   // 原始的に Pixmapを決定
  case 6:
  case 8:
    nowPix = kuBye[2];
    break;
  case 5:
  case 7:
    nowPix = kuBye[3];
    break;
  default:
    nowPix = kuBye[t_num];
  }
  this->Refresh();
  t_num++;
}


/**
  びっくり
*/

void Kukuri::wow()
{
  int t = 0;
  
  // turn() の中に入れると、
  // turn() が終わったら次が実行されるので
  // びっくりが終わってから 失敗が生成されてしまい
  // 都合が悪いので別にしている
  
  if(t_num < 13){  // びっくりのカウント中
    if(a_t_dir2) t = 0; else t = 1; // Pixmap決定
    nowPix = kuWow[t];
    t_num++;
  }else{  // カウントが終わったら
    nowPix = kuPix[d];
    t_num = 0;
    if(a_state == 2){
      // 自動モードで来た場合の処理
      n_state = 9;
      a_state = 0;
    }else{
      n_state = 1;
    }
  }
  this->Refresh();
}

void Kukuri::Main(_A<String> args)
{
    _P<Kukuri> kukuri = new Kukuri();
    Application::Run(kukuri.get());
    kukuri->Dispose();
}

SET_MAIN_CLASS(Kukuri)
