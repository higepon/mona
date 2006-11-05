/**
 *	monaSq ヘッダ
 */
/*
 *	Copyright (c) 2006 okayu punch
 *
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without restriction,
 *	including without limitation the rights to use, copy, modify, merge,
 *	publish, distribute, sublicense, and/or sell copies of the Software,
 *	and to permit persons to whom the Software is furnished to do so,
 *	subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *	THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#if !defined(MONASQ_H)
#define MONASQ_H

#include "monasq_base.h"

// include monAPI / BayGUI
#include <baygui.h>

// include Squirrel header
#include <squirrel.h>
#include <sqstdblob.h>
#include <sqstdsystem.h>
#include <sqstdio.h>
#include <sqstdmath.h>	
#include <sqstdstring.h>
#include <sqstdaux.h>

// include STL header
#include <vector>
#include <list>
#include <string>
using namespace std;

// include monasq::TextArea
#include "misc/TextArea.h"


// クラス前方宣言
class SqRunner;
class SqEnvironment;
class MainFrame;
class InputArea;
class ConsoleArea;


//================================================================
// マクロ
//================================================================

// 配列の要素数
#define ARRAY_LENGTH(x)		(sizeof(x) / sizeof(x[0]))

// 可変引数リストの展開マクロ
#define GET_VA_ARGS(buf, size, fmt) \
	va_list vl; \
	va_start(vl, fmt); \
	monasq_vsprintf(buf, fmt, vl); \
	va_end(vl);


//================================================================
// グローバル関数
//================================================================

// Squirrel 実行環境
extern SqEnvironment* getSqEnvironment();

//printf フォーマットから string オブジェクト生成
extern string fmtString(const char* fmt, ...);

//Squirrel スクリプトを実行
extern void exec_sq_script(HSQUIRRELVM v, SQChar* buffer);



//================================================================
// 定数
//================================================================

// メッセージ定数
enum {
	MONASQ_MSG_START = 0x00010000,	// システムメッセージとかぶらなさそうな適当な値

	// Squirrel の実行開始／終了通知メッセージ
	MONASQ_MSG_SQRUNNER_START = MONASQ_MSG_START,
	MONASQ_MSG_SQRUNNER_END,

	// コンソール欄の操作
	MONASQ_MSG_CONSOLE_ADDTEXT,
	MONASQ_MSG_CONSOLE_CLEAR,
};


//================================================================
// クラス
//================================================================
/**
 *	Squirrel スレッド実行用クラス
 */
class SqRunner : public Runnable {
	string m_script;
	bool m_bPrintExp;

	bool m_bFinished;
	dword m_threadId;
	MonAPI::Mutex m_mutex;
	
	int tryLock() { return m_mutex.tryLock(); }
	void unlock() { m_mutex.unlock(); }

public:
	SqRunner() : m_bFinished(true), m_threadId(0) {}
	virtual ~SqRunner() {}

	bool readyToRun(const SQChar* script, size_t len, bool bPrintExp);
	
	bool isBreaking() { return !running; }
	bool isFinished() { return m_bFinished; }

	virtual void run();
	void kill();
};


/**
 *	Squirrel 実行環境
 *	<br>
 *	※複数の実行環境の作成や並列実行はできません
 */
class SqEnvironment {
	Thread* m_scriptThread;
	SqRunner m_runner;
	list<Window*> m_scriptWndList;		// スクリプトが管理しているウインドウリスト
	
public:
	SqEnvironment() : m_scriptThread(NULL) {}
	virtual ~SqEnvironment() { release(); }

	virtual bool init();
	virtual void release();

	virtual bool isRunning();

	virtual bool run(const SQChar* script, size_t len, bool bPrintExp);
	virtual void stop();
	virtual void stop_anyway();
	
	// Squirrel スレッド実行オブジェクトを返す
	SqRunner* getSqRunner() { return &m_runner; }

	// ウインドウ管理
	virtual void addScriptWindow(Window* wnd) { m_scriptWndList.push_back(wnd); }
	virtual void removeScriptWindow(Window* wnd) { m_scriptWndList.remove(wnd); }
	virtual void stopAllScriptWindow();
};


/**
 *	ボタン
 */
class MonasqButton : public Button {
public:
	// ボタンハンドラ関数のプロトタイプ
	typedef void (*ButtonHandler)(class MonasqButton* sender, void* p);

private:
	ButtonHandler mFunc;
	void* mpData;

public:
	MonasqButton() :
		Button(), mFunc(NULL), mpData(NULL) {}
	MonasqButton(const String& label) :
		Button(label), mFunc(NULL), mpData(NULL) {}

	//クリック時のハンドラを設定する
	virtual void setHandler(ButtonHandler func, void* pData = NULL);

	//イベント処理
	void processEvent(Event *event);
};


/**
 *	スクロールバー
 */
class MonasqScrollbar : public Scrollbar {
	monasq::TextArea* mLinkedTextArea;

public:
	MonasqScrollbar(int orientation) :
		Scrollbar(orientation), mLinkedTextArea(NULL) {}
	
	void linkTextArea(monasq::TextArea* linkedTextArea) {
		mLinkedTextArea = linkedTextArea;
	}

	//値を設定する
	virtual void setValue(int value);
};


/**
 *	入力欄テキストエリア
 */
class InputArea : public monasq::TextArea {

	bool mbModified;

protected:
	//イベント処理
	void processEvent(Event *event);

public:
	InputArea(int buffer_size, bool draw_line)
 		: monasq::TextArea(buffer_size, draw_line), mbModified(false) {}

	// 変更フラグ
	bool isModified() { return mbModified; }
	void setModifyFlag() { mbModified = true; }
	void resetModifyFlag() { mbModified = false; }
};


/**
 *	コンソール欄エリア
 */
class ConsoleArea : public monasq::TextArea {
	MonAPI::Mutex m_mutex;

protected:
	//イベント処理
	void processEvent(Event *event);

public:
	ConsoleArea(int buffer_size, bool draw_line)
 		: monasq::TextArea(buffer_size, draw_line) {}

	int lock() { return m_mutex.lock(); }
	void unlock() { m_mutex.unlock(); }
};


/**
 *	メインウインドウ
 */
class MainFrame : public Frame {

	static MainFrame* s_inst;
	static dword s_mainThreadId;

	// ＧＵＩ部品
	MonasqButton* mPrevHistoryButton;
	MonasqButton* mNextHistoryButton;
	MonasqButton* mRunStopButton;
	MonasqButton* mClearInputButton;
	MonasqButton* mClearOutputButton;

	MonasqScrollbar* mInputScrollbar;

	InputArea* mInputArea;
	ConsoleArea* mConsoleArea;
	
	
	// プログラム引数
	List<char*>* m_pekoe;
	
	// 履歴管理
	vector<string> mHistory;
	unsigned int mHistCursor;
	
	// コンソール操作
	list<char*> m_outputMsgQue;		// テキスト出力メッセージ用キュー
	
	// その他フラグ
	bool mbCreated;


protected:
	void onCreate();
	void onButtonClick(MonasqButton* sender, void* pData);
	void processEvent(Event *event);
	void updateRunStopButton();

public:
	/**
	 *	メインウインドウのポインタを取得
	 */
	static MainFrame* MainFrame::getMainFrame() {
		return s_inst;
	}

	/**
	 *	メインウインドウにメッセージ送信
	 */
	static int send(dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL) {
		return MonAPI::Message::send(s_mainThreadId, header, arg1, arg2, arg3, str);
	}

	// コンソール欄へテキスト追加メッセージを送る
	static bool sendConsoleAddText(char* text);


	// 入力欄に打ち込まれたスクリプトを実行
	void runScript();

	// スクリプトの実行を停止
	void stopScript();

	// 現在行のスクリプトを実行して結果を出力
	void printLineScript();

	// カレントの履歴内文字列を返す
	const string& getCurrentHistoryString() {
		return mHistory[mHistCursor];
	}


	// 前の履歴へ
	void prevHistory();

	// 次の履歴へ
	void nextHistory();


	// 入力欄へフォーカス
	void focusInput(bool bFocused);

	// コンソール欄へフォーカス
	void focusConsole(bool bFocused);


	// プログラム引数を取得
	List<char*>* getProgramArgs() {
		return m_pekoe;
	}

	// コンストラクタ・デストラクタ
	MainFrame(List<char*>* pekoe);
	virtual ~MainFrame();
	
	
public:
	// ボタン処理ハンドラ
	static void MainFrame::buttonHandler(MonasqButton* sender, void* pData);
};


#endif /* !defined(MONASQ_H) */
