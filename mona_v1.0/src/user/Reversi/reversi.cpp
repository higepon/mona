#include<userlib.h>
#include "ReversiBoard.h"

/*----------------------------------------------------------------------
    Monaアプリケーション
----------------------------------------------------------------------*/
class myApplication : public MonaApplication, public Observer {

  public:
    myApplication();
    virtual ~myApplication() {}

  public:
    virtual int main(List<char*>* pekoe);
    virtual void onKeyDown(int keycode, int modifiers);
    virtual void onMouseClick(int x, int y);
    virtual void update(Observable* o, void* arg);

  public:
    virtual void init();
    virtual void drawBoard();
    virtual void drawPieces(int x, int y);

  public:
    static const int PIECE_SIZE = 40;

  protected:
    ReversiBoard* board;
    Screen* virtualScreen;
    Screen screen;

};

/*----------------------------------------------------------------------
    Main関数
----------------------------------------------------------------------*/
int MonaMain(List<char*>* pekoe) {

    monaApp = new myApplication();
    return monaApp->main(pekoe);
}

/*----------------------------------------------------------------------
    myApplication
----------------------------------------------------------------------*/

// コンストラクタ
myApplication::myApplication() : MonaApplication() {
}

// main
int myApplication::main(List<char*>* pekoe) {

    // 初期処理
    init();

    // オセロ板の初期状態を描画
    drawBoard();

    // イベントを待つ
    for (;;);
    return 0;
}

// オセロ板を描画
void myApplication::drawBoard() {

    for (int x = 0; x < ReversiBoard::BOARDW; x++) {
        for (int y = 0; y < ReversiBoard::BOARDH; y++) {
            drawPieces(x, y);
        }
    }
}

// (x, y)にひとこま描画
void myApplication::drawPieces(int x, int y) {

    int piece = this->board->getPiece(x, y);
    int s = PIECE_SIZE;

    // 裏バッファから実VRAMに転送
    switch(piece) {

    case(ReversiBoard::BLACK):
        Screen::bitblt(&screen, x * s, y * s, s, s, virtualScreen, 0, 0);
        break;
    case(ReversiBoard::WHITE):
        Screen::bitblt(&screen, x * s, y * s, s, s, virtualScreen, s, 0);
        break;
    case(ReversiBoard::EMPTY):
        Screen::bitblt(&screen, x * s, y * s, s, s, virtualScreen, 2 * s, 0);
        break;
    default:
        break;
    }
}

// キー入力はしない
void myApplication::onKeyDown(int keycode, int modifiers) {
}

// マウスクリックイベント
void myApplication::onMouseClick(int x, int y) {

    // オセロ板のどこがクリックされた
    int bx = (x - x % PIECE_SIZE) / PIECE_SIZE;
    int by = (y - y % PIECE_SIZE) / PIECE_SIZE;

    // 範囲チェック
    if (!board->checkRange(bx, by)) {
        return;
    }

    // 現在のプレイヤーがこまをおく
    int current = board->getCurrentHand();
    if (!(board->setPiece(bx, by, current))) {
        return;
    }

    // ひっくり返されるこまを一枚ずつ描画
    while (board->existNotReversedPieces()) {

        board->reverseNext();
    }
}

// オセロ板の内容に変更があったことがこの関数に通知される
void myApplication::update(Observable* o, void* arg) {

    if (arg == NULL) {
        drawBoard();
    } else {

        Point* point = (Point*)arg;
        drawPieces(point->x, point->y);
    }
}

void myApplication::init() {

    // マウスサーバーと自分自身のPIDを得る
    dword myPid   = System::getPID();
    dword destPid = Message::lookup("MOUSE.SVR");

    // マウスサーバは起動している？
    if (destPid == 0) {
        printf("process MOUSE.SVR not found\n");
        exit(-1);
    }

    // マウスサーバーにマウス情報をくれるように自分自身を登録するメッセージを送信 
    MessageInfo info;
    Message::create(&info, MSG_MOUSE_REGIST_TO_SERVER, myPid, 0, 0, NULL);
    if (Message::send(destPid, &info)) {
        printf("regist error\n");
    }

    // オセロ板を生成
    this->board = new ReversiBoard();

    // オセロ板に変更があったら通知してもらう
    this->board->addObserver(this);

    // 裏バッファ用に仮想スクリーンを確保
    virtualScreen = new VirtualScreen(200 * 1024);

    // 描画部品として白丸・黒丸・空の部品を裏バッファに描画
    int s = PIECE_SIZE;
    int s1 = (int)(0.5 * s);
    virtualScreen->fillRect16(0, 0, s, s, Color::rgb(0x00, 0xFF, 0x00));
    virtualScreen->fillRect16(s , 0, s, s, Color::rgb(0x00, 0xFF, 0x00));
    virtualScreen->fillCircle16(s1 * 3, s1, s1 - 4, Color::rgb(0xFF, 0xFF, 0xFF));
    virtualScreen->fillCircle16(s1, s1, s1 - 4, Color::rgb(0x00, 0x00, 0x00));
    virtualScreen->fillRect16(s * 2, 0, s, s, Color::rgb(0x00, 0xFF, 0x00));

    for (int x = 0; x < 3 * s; x++) {
        virtualScreen->putPixel16(x, s - 1, Color::rgb(0x00, 0x00, 0x00));
    }

    for (int x = 0; x < 3 * s; x++) {
        virtualScreen->putPixel16(x, 0, Color::rgb(0x00, 0x00, 0x00));
    }

    for (int x = 0; x <= 3 * s; x += s) {
        for (int y = 0; y < 3 * s; y++) {
            virtualScreen->putPixel16(x - 1, y, Color::rgb(0x00, 0x00, 0x00));
        }
    }
}

