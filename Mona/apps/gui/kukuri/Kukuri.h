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

#ifndef __KUKURI_H__
#define __KUKURI_H__

#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/Timer.h>
#include <gui/System/Drawing/Bitmap.h>

#define KUKURI_WIDTH  32
#define KUKURI_HEIGHT 48
#define KUKURI_NONE   Color::FromArgb(142, 187, 138)  // 透過色 (DarkSeaGreen)
#define MIRROR 100

#define PI       (atan(1) * 4)   // 円周率
#define RAD(d)   (d / 180 * PI)  // 度数→ラジアン
#define DEG(r)   (r * 180 / PI)  // ラジアン→度数

#define IDOU    5       // 移動量

enum KukuriBitmaps
{
    kukuri_b,
    kukuri_b2,
    kukuri_br,
    kukuri_br2,
    kukuri_br3,
    kukuri_bye_1,
    kukuri_bye_2,
    kukuri_bye_3,
    kukuri_bye_4,
    kukuri_f,
    kukuri_f2,
    kukuri_fr,
    kukuri_fr2,
    kukuri_fr3,
    kukuri_r,
    kukuri_r2,
    kukuri_r3,
    kukuri_sl1,
    kukuri_sl2,
    kukuri_th1,
    kukuri_th2,
    kukuri_th3,
    kukuri_th4,
    kukuri_turn_b,
    kukuri_turn_br,
    kukuri_turn_f,
    kukuri_turn_fr,
    kukuri_turn_r,
    kukuri_turn_r2,
    kukuri_wow
};

/**
  ククリ様のお姿
*/

class Kukuri: public System::Mona::Forms::Form
{
public:
  Kukuri();
  virtual ~Kukuri();
  
  virtual void Create();
  virtual void Dispose();
  
  static void Main(_A<System::String> args);
  
protected:
  virtual void OnPaint();
  virtual System::Mona::Forms::Control::NCState NCHitTest(int x, int y);
  virtual void OnNCMouseDown(_P<System::Mona::Forms::MouseEventArgs> e);
  virtual void OnNCMouseUp(_P<System::Mona::Forms::MouseEventArgs> e);
  
private:
  void setBitmaps();
  void timer_Tick(_P<System::Object> sender, _P<System::EventArgs> e);
  void normal();
  void calculation(int tmp_x, int tmp_y);
  void turn();
  void sleeping();
  void byebye();
  void wow();
  
private:
  _P<System::EventHandler<Kukuri> > timerHandler;
  int nowCount, count;
  /** 角度 */
  int r;
  /** 方向 */
  int d;
  /** 普通時の状態 */
  int n_state;
  /** 勝手に動くときの状態 */
  int a_state;
  /** 歩くときのXPM計算用 */
  int w_num;
  /** 回転の後経過した歩数(カウント) */
  int a_t_num;
  /** 回転後どちらにずれるか */
  int a_t_dir;
  /** 回転後どちらにずれたか */
  bool a_t_dir2;
  /** マウスポインタとの距離 */
  double distance;
  /** x増分 */
  double px;
  /** y増分 */
  double py;
  /** 自動移動時の移動先x座標 */
  int auto_x;
  /** 自動移動時の移動先y座標 */
  int auto_y;
  /** 眠るまでのカウント */
  int s_interval;
  /** 回転時のXPMの順番 */
  int t_num;
  int tmpo;
  /** 寝てるときの画像の番号 */
  bool s_pixmap;
  /** 眠ってからのカウント */
  int s_count;
  /** 自動モード関係 */
  bool a_flag;
  /** 考え中のXPM番号 */
  int th_num;
  int nowPix;
  /** ククリ様の画像 */
  int kuPix[32];
  /** 睡眠中の画像 */
  int kuSleep[2];
  int kuTurn[11];
  int kuBye[4];
  int kuWow[2];
  int kuThink[4];
};

#endif  // __KUKURI_H__
