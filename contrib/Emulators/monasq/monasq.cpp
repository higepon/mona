/**
 *  monaSq メインルーチン
 */
/*
 *  Copyright (c) 2006 okayu punch
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge,
 *  publish, distribute, sublicense, and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *  THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "monasq.h"
#include "sqplus.h"

using namespace SqPlus;

#define MONASQ_TITLE        "MonaSq"
#define MONASQ_VERSION      "MonaSq 1.0.0"


// スクリプト履歴の初期化文字列（init_script.cpp で定義）
extern const SQChar* g_init_script[];
extern const SQChar* g_startup_script;
extern size_t g_init_script_len;
extern int g_default_history_cursor;

// MonaSq ライブラリの初期化
extern void monasq_init_monasq_lib(HSQUIRRELVM v);

// MonaSq/monapi ライブラリの初期化
extern void monasq_init_monapi_lib(HSQUIRRELVM v);

// MonaSq/baygui ライブラリの初期化
extern void monasq_init_baygui_lib(HSQUIRRELVM v);




//================================================================
// SqRunner クラス
//================================================================
/**
 *  Squirrel プログラム実行準備
 */
bool SqRunner::readyToRun(const SQChar* script, size_t len, bool bPrintExp) {
    if (tryLock() != 0) return false;   // プログラム実行ロック

    m_bFinished = false;
    m_bPrintExp = bPrintExp;
    m_script.assign( script, len );

    if (m_bPrintExp) {
        m_script.insert(0, "monasq.inspect(");
        m_script.append(")");
    }
    return true;
}

/**
 *  Squirrel プログラム実行
 */
void SqRunner::run() {
    // 開始処理
    m_threadId = MonAPI::System::getThreadID();
    running = true;

    // メインウインドウに開始メッセージ送信
    MainFrame::send( MONASQ_MSG_SQRUNNER_START );

    // コンパイル
    SquirrelObject mainFunc = SquirrelVM::CompileBuffer( m_script.c_str() );

    // 実行
    SquirrelVM::RunScript( mainFunc );

    // 終了処理
    m_threadId = 0;
    running = false;
    m_bFinished = true;

    // メインウインドウに終了メッセージ送信
    MainFrame::send( MONASQ_MSG_SQRUNNER_END );

    unlock();   // プログラム実行ロック解除
}

/**
 *  スレッドを停止
 */
void SqRunner::kill() {
    if (m_threadId == 0) return;
    MonAPI::System::kill( m_threadId );
    m_threadId = 0;
    running = false;
    m_bFinished = true;

    unlock();   // プログラム実行ロック解除
}


//================================================================
// SqEnvironment クラス
//================================================================

/**
 *  実行環境の初期化
 */
bool SqEnvironment::init() {
    // VM 初期化
    SquirrelVM::Init();
    HSQUIRRELVM v = SquirrelVM::GetVMPtr();
    if (v == NULL) return false;

    // MonaSq ライブラリを初期化
    monasq_init_monasq_lib(v);

    // MonaSq/monapi ライブラリの初期化
    monasq_init_monapi_lib(v);

    // MonaSq/baygui ライブラリの初期化
    monasq_init_baygui_lib(v);

    return true;
}

/**
 *  実行環境の破棄
 */
void SqEnvironment::release() {
    // 必ず実行を停止する
    stop_anyway();

    // スレッドオブジェクト破棄
    if (m_scriptThread) {
        delete m_scriptThread;
        m_scriptThread = NULL;
    }

    // VM 解放
    SquirrelVM::Shutdown();
}

/**
 *  Squirrel プログラムを実行
 */
bool SqEnvironment::run(const SQChar* script, size_t len, bool bPrintExp) {
    static MonAPI::Mutex* s_mutex = NULL;
    if (NULL == s_mutex) {
        s_mutex = new MonAPI::Mutex;
    }
    s_mutex->lock();
    { // synchronized
        if (!SquirrelVM::IsInitialized()) goto failure;
        if (isRunning()) goto failure;

        // プログラム実行準備
        bool bOK = m_runner.readyToRun(script, len, bPrintExp);
        if (!bOK) goto failure;

        // スレッド作成
        if (m_scriptThread) delete m_scriptThread;
        m_scriptThread = new Thread(&m_runner);

        // スレッドを開始
        m_scriptThread->start();
    }
    s_mutex->unlock();
    return true;
failure:
    s_mutex->unlock();
    return false;
}

/**
 *  実行中のプログラムに停止要求を送る
 */
void SqEnvironment::stop() {
    if (!isRunning()) return;
    m_runner.stop();
    stopAllScriptWindow();
}

/**
 *  実行中のプログラムを強制的に停止
 */
void SqEnvironment::stop_anyway() {
    stop();
    int i = 0;
    while (!m_runner.isFinished()) {
        if (i++ >= 10) {
            // １秒待っても終了しなければ kill() する
            m_runner.kill();
            return;
        }
        sleep(100);
    };
}

/**
 *  プログラムが実行中なら true を返す
 */
bool SqEnvironment::isRunning() {
    return !m_runner.isFinished();
}

/**
 *  スクリプトが管理している全てのウインドウに停止要求を送る
 */
void SqEnvironment::stopAllScriptWindow() {
    list<Window*>::iterator it = m_scriptWndList.begin();
    while (it != m_scriptWndList.end()) {
        (*it)->stop();
        ++it;
    }
}


//================================================================
// InputArea クラス
//================================================================
/**
 *  入力欄のイベント処理
 */
void InputArea::processEvent(Event *event) {
    if (event->getType() == Event::KEY_PRESSED) {
        KeyEvent* keyEvent = (KeyEvent*)event;
        int keycode = keyEvent->getKeycode();
        if (keycode == KeyEvent::VKEY_F5) {
            // F5：スクリプト実行（※ VKEY_F5 は送られてこない？）
            MainFrame::getMainFrame()->runScript();
            return;
        }
        if (keyEvent->getModifiers() == KeyEvent::VKEY_CTRL) {
            if (keycode == KeyEvent::VKEY_ENTER) {
                // Ctrl+Enter：スクリプト１行実行＆結果出力
                MainFrame::getMainFrame()->printLineScript();
                return;
            }
            if (keycode == KeyEvent::VKEY_TAB) {
                // Ctrl+TAB：コンソール欄へフォーカス移動
                MainFrame::getMainFrame()->focusInput(false);
                MainFrame::getMainFrame()->focusConsole(true);
                return;
            }
        }
        if (keyEvent->getModifiers() == KeyEvent::VKEY_ALT) {
            if (keycode == KeyEvent::VKEY_ENTER) {
                // Alt+Enter：スクリプト実行
                MainFrame::getMainFrame()->runScript();
                return;
            }
            if (keycode == KeyEvent::VKEY_UP) {
                // Alt+↑：前の履歴
                MainFrame::getMainFrame()->prevHistory();
                return;
            }
            if (keycode == KeyEvent::VKEY_DOWN) {
                // Alt+↓：次の履歴
                MainFrame::getMainFrame()->nextHistory();
                return;
            }
        }
        if (keycode < 128 || keycode == KeyEvent::VKEY_DELETE) {
            // 変更フラグを立てる
            setModifyFlag();
        }
    }
    // スーパークラスに処理を渡す
    monasq::TextArea::processEvent(event);
}


//================================================================
// ConsoleArea クラス
//================================================================
/**
 *  コンソール欄のイベント処理
 */
void ConsoleArea::processEvent(Event *event) {
    if (event->getType() == Event::KEY_PRESSED) {
        KeyEvent* keyEvent = (KeyEvent*)event;
        int keycode = keyEvent->getKeycode();
        if (keyEvent->getModifiers() == KeyEvent::VKEY_CTRL) {
            if (keycode == KeyEvent::VKEY_TAB) {
                // Ctrl+TAB：入力欄へフォーカス移動
                MainFrame::getMainFrame()->focusConsole(false);
                MainFrame::getMainFrame()->focusInput(true);
                return;
            }
        }
    }
    // スーパークラスに処理を渡す
    monasq::TextArea::processEvent(event);
}


//================================================================
// MonasqButton クラス
//================================================================
/**
 *  クリック時のハンドラを設定する
 */
void MonasqButton::setHandler(ButtonHandler func, void* ptr) {
    mFunc = func;
    mpData = ptr;
}

/**
 *  ボタンのイベント処理
 */
void MonasqButton::processEvent(Event *event) {
    if (event->getType() == MouseEvent::MOUSE_RELEASED) {
        if (getPushed()) mFunc(this, mpData);
    }
    // スーパークラスに処理を渡す
    Button::processEvent(event);
}


//================================================================
// MonasqScrollbar クラス
//================================================================
/**
 *  スクロールバー位置の更新処理
 */
void MonasqScrollbar::setValue(int value) {
    if (value == getValue()) return;

    // スーパークラスに処理を渡す
    Scrollbar::setValue(value);

    if (mLinkedTextArea) {
        mLinkedTextArea->setVScroll(value);
    }
}


//================================================================
// MainFrame クラス
//================================================================
MainFrame* MainFrame::s_inst = NULL;    // 唯一のメインウインドウインスタンス
dword MainFrame::s_mainThreadId = 0;    // メインスレッドのスレッドＩＤ

/**
 *  コンソール欄へテキストを追加する（スレッドセーフ）
 *  @param text     malloc() で確保された文字列バッファ。
 *                  解放は MainFrame 側で行う。
 */
bool MainFrame::sendConsoleAddText(char* text) {
    s_inst->mConsoleArea->lock();
    s_inst->m_outputMsgQue.push_back(text);
    MainFrame::send(MONASQ_MSG_CONSOLE_ADDTEXT);
    s_inst->mConsoleArea->unlock();

    // ※ノータイムで送りつづけるとメインスレッドのメッセージキューが
    //   出力処理で埋まって反応が悪くなるみたいなので少し休む
    sleep(30);
    return true;
}


/**
 *  初期化
 */
MainFrame::MainFrame(List<char*>* pekoe) {
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    s_inst = this;
    s_mainThreadId = MonAPI::System::getThreadID();

    // プログラム引数
    m_pekoe = pekoe;

    // Squirrel 実行環境を初期化
    bool bSqEnvOK = getSqEnvironment()->init();

    // 履歴初期化
    for (size_t i=0; i<g_init_script_len; ++i) {
        mHistory.push_back( g_init_script[i] );
    }
    mHistCursor = max(0, min(g_default_history_cursor, (int)(mHistory.size() - 1)));

    // このウインドウの初期化
    setBounds(100, 100, 400, 445);
    setTitle( MONASQ_TITLE );

    // 前の履歴ボタン
    mPrevHistoryButton = new MonasqButton("↑");
    mPrevHistoryButton->setBounds(0,0,40,20);
    mPrevHistoryButton->setHandler(buttonHandler);
    add(mPrevHistoryButton);

    // 次の履歴ボタン
    mNextHistoryButton   = new MonasqButton("↓");
    mNextHistoryButton->setBounds(40,0,40,20);
    mNextHistoryButton->setHandler(buttonHandler);
    add(mNextHistoryButton);

    // 入力欄クリアボタン
    mClearInputButton   = new MonasqButton("入力欄クリア");
    mClearInputButton->setBounds(90,0,73,20);
    mClearInputButton->setHandler(buttonHandler);
    add(mClearInputButton);

    // 出力欄クリアボタン
    mClearOutputButton   = new MonasqButton("出力欄クリア");
    mClearOutputButton->setBounds(168,0,73,20);
    mClearOutputButton->setHandler(buttonHandler);
    add(mClearOutputButton);

    // 実行／停止ボタン
    mRunStopButton   = new MonasqButton();
    mRunStopButton->setBounds(251,0,110,20);
    mRunStopButton->setHandler(buttonHandler);
    updateRunStopButton();
    add(mRunStopButton);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    // 入力欄スクロールバー
    mInputScrollbar = new MonasqScrollbar( Scrollbar::VERTICAL );
    mInputScrollbar->setBounds(373, 21, 16, 235);
    add(mInputScrollbar);

    // 入力欄
    mInputArea = new InputArea(1024 * 32, true);
    mInputArea->setBounds(0, 21, 372, 235);
    mInputArea->setText( getCurrentHistoryString().c_str() );
//  mInputArea->setText( g_startup_script );
    mInputArea->moveCaret(0, 0);
    mInputArea->setFocused(true);
    add(mInputArea);

    // コンソール欄
    string text = fmtString("%s\n%s\n%s\n\n%s",
        MONASQ_VERSION,
        SQUIRREL_VERSION, SQUIRREL_COPYRIGHT,
        bSqEnvOK ? "" : "Squirrel VM initialize failed.\n");
    mConsoleArea = new ConsoleArea(1024 * 64, true);
    mConsoleArea->setBounds(0, 256, 389, 181);
    mConsoleArea->setText( text.c_str() );
    add(mConsoleArea);

    // ※タイマーイベントをウインドウ生成完了イベントとして代用
    mbCreated = false;
    setTimer(1);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
}

/**
 *  解放
 */
MainFrame::~MainFrame() {
    // VM 解放
    getSqEnvironment()->release();

    s_mainThreadId = 0;
    s_inst = NULL;
}

/**
 *  ボタンイベントの受け皿関数
 */
void MainFrame::buttonHandler(MonasqButton* sender, void* pData) {
    s_inst->onButtonClick(sender, pData);
}

/**
 *  ボタンイベント処理
 */
void MainFrame::onButtonClick(MonasqButton* sender, void* pData) {
    if (sender == mPrevHistoryButton) {
        // 前の履歴
        TRACE("PREV\n");
        prevHistory();
    }
    if (sender == mNextHistoryButton) {
        // 次の履歴
        TRACE("NEXT\n");
        nextHistory();
    }
    if (sender == mRunStopButton) {
        // スクリプト実行／停止
        TRACE("RUN/STOP\n");
        if (getSqEnvironment()->isRunning()) {
            stopScript();
        } else {
            runScript();
        }
    }
    if (sender == mClearInputButton) {
        TRACE("CLEAR INPUT\n");
        // 入力欄クリア
        mInputArea->setText("");
        mInputArea->repaint();
    }
    if (sender == mClearOutputButton) {
        // 出力欄クリア
        send( MONASQ_MSG_CONSOLE_CLEAR );
        TRACE("CLEAR CONSOLE\n");
    }
}

/**
 *  ウインドウ生成完了
 */
void MainFrame::onCreate() {
    TRACE("MainFrame::onCreate()\n");

    // テキストエリアとスクロールバーをリンク（ウインドウ生成後に行うこと！）
    mInputArea->linkScrollbar(mInputScrollbar);
    mInputScrollbar->linkTextArea(mInputArea);
}

/**
 *  イベント処理
 */
void MainFrame::processEvent(Event *event) {

    // タイマーイベント
    if (event->getType() == Event::TIMER) {
        if (!mbCreated) {
            // ※ウインドウ生成完了
            onCreate();
            mbCreated = true;
        }
    }

    // カスタムイベント
    if (event->getType() == Event::CUSTOM_EVENT) {
        switch (event->header) {
        case MONASQ_MSG_SQRUNNER_START:
        case MONASQ_MSG_SQRUNNER_END:
            // スクリプトの実行開始／終了メッセージ
            updateRunStopButton();
            break;
        case MONASQ_MSG_CONSOLE_ADDTEXT:
            mConsoleArea->lock();
            if (m_outputMsgQue.size() != 0) {
                char* text = m_outputMsgQue.front();
                m_outputMsgQue.pop_front();
                mConsoleArea->addText( text );
                free( text );
            }
            mConsoleArea->unlock();
            break;
        case MONASQ_MSG_CONSOLE_CLEAR:
            mConsoleArea->lock();
            mConsoleArea->setText("");
            mConsoleArea->repaint();
            mConsoleArea->unlock();
            break;
        }
    }

    // スーパークラスに処理を渡す
    Frame::processEvent(event);
}

/**
 *  実行／停止ボタンの表示を更新
 */
void MainFrame::updateRunStopButton() {
    if (getSqEnvironment()->isRunning()) {
        mRunStopButton->setLabel("停止");
    } else {
        mRunStopButton->setLabel("実行 (Alt+Enter)");
    }
}

/**
 *  入力欄に打ち込まれたスクリプトを実行
 */
void MainFrame::runScript() {
    if (getSqEnvironment()->isRunning()) return;

    // 実行
    getSqEnvironment()->run(mInputArea->getText(), mInputArea->getTextLength(), false);

    // 変更フラグが立っていれば履歴の末尾に追加
    if (mInputArea->isModified()) {
        mHistCursor = mHistory.size() - 1;
        if (getCurrentHistoryString().size() == 0) {
            mHistory.pop_back();
        }
        mHistory.push_back( mInputArea->getText() );
        mHistCursor = mHistory.size() - 1;
        mInputArea->resetModifyFlag();
    }
}

/**
 *  現在行のスクリプトを式として実行して結果を出力
 */
void MainFrame::printLineScript() {
    if (getSqEnvironment()->isRunning()) return;

    // 現在行の範囲を調べる
    char* start = mInputArea->getText() + mInputArea->getCaretLineStart();
    char* end = start;
    while (*end != '\n' && *end != '\0') ++end;

    // 実行
    getSqEnvironment()->run(start, end - start, true);
}

/**
 *  スクリプトの実行を停止
 */
void MainFrame::stopScript() {
    // 停止
    getSqEnvironment()->stop();
}

/**
 *  前の履歴へ
 */
void MainFrame::prevHistory() {
    if (mHistCursor == 0) goto finally;
    --mHistCursor;
    mInputArea->setText( getCurrentHistoryString().c_str() );
    mInputArea->moveCaret(0, 0);
    mInputArea->resetModifyFlag();
finally:
    mInputArea->setFocused(true);
}

/**
 *  次の履歴へ
 */
void MainFrame::nextHistory() {
    if (mHistCursor == mHistory.size() - 1) {
        if (getCurrentHistoryString().size() == 0) {
            goto finally;
        }
        mHistory.push_back("");
    }
    ++mHistCursor;
    mInputArea->setText( getCurrentHistoryString().c_str() );
    mInputArea->moveCaret(0, 0);
    mInputArea->resetModifyFlag();
finally:
    mInputArea->setFocused(true);
}

/**
 *  入力欄へフォーカス
 */
void MainFrame::focusInput(bool bFocused) {
    mInputArea->setFocused(bFocused);
}

/**
 *  コンソール欄へフォーカス
 */
void MainFrame::focusConsole(bool bFocused) {
    mConsoleArea->setFocused(bFocused);
}




//================================================================
// Squirrel プログラムから呼び出す関数の定義
//================================================================

// Squirrel 関数の登録用テーブル
static SQRegFunction monasq_funcs[]={
    //generic
    {0,0}
};

/**
 *  Squirrel 関数の登録
 */
void register_sq_func(HSQUIRRELVM v, const SQChar* name, SQFUNCTION func,
        SQInteger nparamscheck, const SQChar *typemask)
{
    sq_pushroottable(v);
    sq_pushstring(v, name, -1);
    sq_newclosure(v, func, 0);
    sq_setnativeclosurename(v, -1, name);
    sq_setparamscheck(v, nparamscheck, typemask);
    sq_createslot(v, -3);
    sq_pop(v, 1);
}

/**
 *  デバッグフック関数の登録
 *  <br>
 *  ステップ実行機能とかを作る時に使う
 */
void register_debug_hook(HSQUIRRELVM v, SQChar* name, SQFUNCTION func) {
    sq_pushregistrytable(v);
    sq_pushstring(v, name, -1);
    sq_newclosure(v, func, 0);
    sq_createslot(v, -3);

    sq_pushstring(v, name, -1);
    sq_rawget(v, -2);
    sq_setdebughook(v);
    sq_pop(v, 1);
}

/**
 *  Squirrel 関数の一括登録
 */
void monasq_init_monasq_lib(HSQUIRRELVM v)
{
    SQInteger i = 0;
    while (monasq_funcs[i].name != 0) {
        register_sq_func(v, monasq_funcs[i].name, monasq_funcs[i].f,
            monasq_funcs[i].nparamscheck, monasq_funcs[i].typemask);
        i++;
    }

    // root.monasq テーブルを作成
    SquirrelObject root = SquirrelVM::GetRootTable();
    SquirrelObject nameSpaceTable = SquirrelVM::CreateTable();
    root.SetValue(_T("monasq"), nameSpaceTable);

    // スタートアップスクリプトを実行
    SquirrelObject startup_func = SquirrelVM::CompileBuffer( g_startup_script );
    SquirrelVM::RunScript( startup_func );
}


//================================================================
// グローバル関数
//================================================================
/**
 *  printf フォーマットから string オブジェクト生成
 */
string fmtString(const char* fmt, ...) {
    char buf[1024 * 4];
    GET_VA_ARGS( buf, ARRAY_LENGTH(buf), fmt );
    return string(buf);
}

/**
 *  printf
 */
void monasq_printf(const char* fmt, ...) {
    const int BUFFER_LEN = 1024 * 8;
    char* buf = (char*)malloc(BUFFER_LEN);
    GET_VA_ARGS( buf, BUFFER_LEN, fmt );

    // コンソール欄へメッセージ送信
    MainFrame::sendConsoleAddText(buf);
}

/**
 *  snprintf
 */
int monasq_snprintf(char *buf, unsigned int size, const char* fmt, ...) {
    va_list vl;
    va_start(vl, fmt);
//  int result = vsnprintf(buf, size, fmt, vl);
    int result = monasq_vsprintf(buf, fmt, vl);
    va_end(vl);
    return result;
}


/**
 *  Squirrel 実行環境へのポインタを返す
 *  NULL は返さない
 */
SqEnvironment* getSqEnvironment() {
    static SqEnvironment* s_sqEnv = NULL;
    if (NULL == s_sqEnv) {
        s_sqEnv = new SqEnvironment;
    }
    return s_sqEnv;
}


/**
 *  SQVM::Execute() に追加したフック関数
 *  <br>
 *  プログラム実行の中止要求がある場合は true を返す
 */
bool monasq_break_hook(HSQUIRRELVM v) {
    SqRunner* runner = getSqEnvironment()->getSqRunner();
    if (runner->isBreaking()) {
        getSqEnvironment()->stopAllScriptWindow();
        return true;
    }
    return false;
}

/**
 *  メイン
 */
//int MonaMain(List<char*>* pekoe) {
int main(int argc, char* argv[]) {
    // ウインドウ初期化
    MainFrame* frame = new MainFrame(NULL);
    if (frame == NULL) return -1;

    // メインループ
    frame->run();

    // ウインドウ解放
    delete frame;

    return 0;
}
