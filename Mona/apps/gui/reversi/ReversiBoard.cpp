#include <monapi.h>
#include "ReversiBoard.h"

using namespace MonAPI;

/**
 * コンストラクタ
 */
ReversiBoard::ReversiBoard() {
    init();
}

/**
 * 指定された駒が盤面上にいくつあるかを数える
 * @param  piece <code>ReversiBoard.WHITE, ReversiBoard.BLACK, ReversiBoard.EMPTY</code>
 * @return 駒の個数
 */
int ReversiBoard::countPieces(int piece) {

    int result = 0;

    // 駒の個数を数える
    for (int x = 0; x < BOARDW; x++) {
        for (int y = 0; y < BOARDH; y++) {
            if (this->board[x][y] == piece) result++;
        }
    }
    return result;
}

/**
 * 指定された位置の駒を返す
 * @param  x (x, y) 駒の位置
 * @param  y (x, y) 駒の位置
 * @return 駒 盤面の範囲外の場合は<code>ReversiBoard.EMPTY</code>
 */
int ReversiBoard::getPiece(int x, int y) {

    // x, yの範囲をチェック
    if (!checkRange(x, y)) return EMPTY;

    // 駒を返す
    return board[x][y];
}

/**
 * 指定された位置の駒を返す
 * @param  point 駒の位置
 * @return 駒 盤面の範囲外の場合は<code>ReversiBoard.EMPTY</code>
 */
int ReversiBoard::getPiece(Point* point) {
    return getPiece(point->x, point->y);
}

/**
 * 現在の手番のプレイヤーを返す<code>ReversiBoard.EMPTY</code>を返したときは勝負の決着がついている状態<BR>
 * @return 現在の手番のプレイヤー <code>ReversiBoard.BLACK, WHITE, EMPTY</code>
 */
int ReversiBoard::getCurrentHand() const {
    return this->currentHand;
}

/**
 * 指定された位置に駒を置く。<BR>
 * 駒が置けた場合、置いた場所を示す。<BR>
 * <code>Point</code>がnotifyObserversで<BR>
 * 監視元に通知される
 * @param x (x, y) 駒の位置
 * @param y (x, y) 駒の位置
 * @param piece 置く駒
 * @return 駒が置けたら<code>true</code>, 駒が置けなかったら<code>false</code>
 */
bool ReversiBoard::setPiece(int x, int y, int piece) {

    int numReversiblePieces; // 裏返すことのできる駒数

    // 現在の手と照合
    if (currentHand != piece) return false;

    // 前手が完全に終了したかどうかをチェック
    if (existNotReversedPieces()) return false;

    // そこに置けるかどうかを調べる
    numReversiblePieces = countReversiblePieces(x, y, piece);
    if (numReversiblePieces <= 0) return false;

    // pieceを(x, y)に置いたときに、裏返す駒をすべて記憶
    recordReversiblePieces(x, y, piece);

    // 駒を置く
    board[x][y] = piece;

    // ターンを記憶
    allTurns->add(new Point3D(x, y, piece));

    // 次手プレイヤーを判断
    setNextHand();

    // 変更をObserverに通知
    this->setChanged();
    Point* p = new Point(x, y);
    this->notifyObservers(p);
    delete p;
    this->clearChanged();

    return true;
}

/**
 * <code>setPiece()</code>した後に、裏返してない駒が存在するかを返す<BR>
 * <pre>
 *    使用方法
 *      board.setPiece(3, 5, OthlloBoard::BLACK);
 *      while (existNotReversedPieces()) {
 *          reverseNext();
 *          // 描画用処理
 *      }
 * </pre>
 * @return 裏返してない駒が存在する場合<code>true</code>
 */
bool ReversiBoard::existNotReversedPieces() {
    return allNotReversedPieces->size() > 0;
}

/**
 * 次の駒を裏返す<BR>
 * 裏返した場所を示す<code>Point</code>がnotifyObserversで監視元に通知される
 * existNotReversedPieces()の使用方法を参照
 */
void ReversiBoard::reverseNext() {

    // 裏返す位置を取得
    Point* point = allNotReversedPieces->get(0);

    // 裏返す
    reversePiece(point->x, point->y);

    // 変更を通知
    this->setChanged();
    notifyObservers(new Point(point));
    this->clearChanged();

    // 裏返したので位置を消去
    allNotReversedPieces->removeAt(0);

    return;
}

/**
 * 現在ターンを返す
 * @return 現在ターン
 */
int ReversiBoard::getTurn() const {
    return this->turn;
}

/**
 * undo機能
 * 1手戻る
 */
void ReversiBoard::undo() {

    // 1手前に戻れない場合
    if (getTurn() < 1) return;

    // 盤面初期化
    initBoard();

    // 盤面並べなおし
    for (int i = 0; i < allTurns->size() -1; i++) {

        // 駒を置く
        Point3D* p = allTurns->get(i);
        setPiece(p->x, p->y, p->z);
        while (existNotReversedPieces()) {
            reverseNext();
        }
    }

    // 最後のターンを消去
    allTurns->removeAt(allTurns->size() -1);

    return;
}

/**
 * 指定された位置に駒を置いたときに、いくつ駒が裏返せるかを返す
 * @param x
 * @param y
 * @param piece 置く駒
 * @return 裏返せる駒数
 */
int ReversiBoard::countReversiblePieces(int x, int y, int piece) {

    int result = 0;

    // x, yの範囲をチェック
    if (!checkRange(x, y)) return 0;

    // すでに駒が置かれているかをチェック
    if (board[x][y] != EMPTY) return 0;

    // (x, y)にpieceを置いた場合に裏返すことのできる駒を8方向に探索
    result += countReversiblePieceToOneAngle(x, y,  1,  0, piece, false);
    result += countReversiblePieceToOneAngle(x, y,  1,  1, piece, false);
    result += countReversiblePieceToOneAngle(x, y,  0,  1 ,piece, false);
    result += countReversiblePieceToOneAngle(x, y, -1,  1, piece, false);
    result += countReversiblePieceToOneAngle(x, y, -1,  0, piece, false);
    result += countReversiblePieceToOneAngle(x, y, -1, -1, piece, false);
    result += countReversiblePieceToOneAngle(x, y,  0, -1, piece, false);
    result += countReversiblePieceToOneAngle(x, y,  1, -1, piece, false);

    return result;
}

/**
 * リバーシ盤をリセットする
 */
void ReversiBoard::resetBoard() {

    delete (this->allTurns);
    this->allTurns = new HList<Point3D*>();
    initBoard();

    // 変更を通知
    this->setChanged();
    this->notifyObservers(NULL);
    this->clearChanged();
    return;
}

/**
 * ReversiBoard生成と初期化を行う
 */
void ReversiBoard::init() {

    // 盤面初期化
    initBoard();

    // すべてのターンを記憶するVector初期化
    allTurns = new HList<Point3D*>();

    return;
}

/**
 * 盤面初期化行う
 */
void ReversiBoard::initBoard() {

    // 現在ターンを設定
    this->turn = 0;

    // 先手は黒
    this->currentHand = BLACK;

    // 盤面を空にする
    for (int x = 0; x < BOARDW; x++) {
        for (int y = 0; y < BOARDH; y++) board[x][y] = EMPTY;
    }

    // 初期状態で置かれている駒をセット
    board[3][3] = BLACK;
    board[4][4] = BLACK;
    board[4][3] = WHITE;
    board[3][4] = WHITE;

    // 裏返すべき駒のすべて位置を記憶するList初期化
    allNotReversedPieces = new HList<Point*>();

    return;
}

/**
 * 指定した駒を裏返す
 */
void ReversiBoard::reversePiece(int x, int y) {

    // x, yの範囲をチェック
    if (!checkRange(x, y)) return;

    // 駒を裏返す､もし空なら何もしない
    board[x][y] = turnColor(board[x][y]);

    return;
}

/**
 * 入力された値が盤面上に存在するかチェックする
 */
bool ReversiBoard::checkRange(int x, int y) {

    // 盤面上にあるという条件
    bool xRange = x >= 0 && x < BOARDW;
    bool yRange = y >= 0 && y < BOARDH;

    // 盤面上にある
    if (xRange && yRange) return true;

    // 盤面上にない
    return false;
}

/**
 *  指定されたベクトル(toX, toY)の方向へ裏返せる駒数を数える
 *  reordFlagがtrueの時は位置を記憶
 */
int ReversiBoard::countReversiblePieceToOneAngle(int x, int y, int toX,int toY
                                                 , int piece, bool recordFlag) {
    int result = 0;

    // 隣の駒へ移動
    x += toX;
    y += toY;

    // 盤面の端まで探索を続ける
    while (x >= 0 && y >= 0 && x < BOARDW && y < BOARDH) {

        // 探索中の駒が裏返しの色なら裏返せる
        if (board[x][y] == turnColor(piece)) {

            // 裏返せる駒の位置を記憶
            if (recordFlag) allNotReversedPieces->add(new Point(x, y));

            // 駒数インクリメント
            result++;
        }

        // 同じ色だったら、そこまであった裏返し色を裏返すことができる
        else if (board[x][y] == piece) return result;

        // 空なら裏返せない
        else if (board[x][y] == EMPTY) {

            if (recordFlag) {

                // 記憶していた駒の位置
                for (int i = 0; i < result; i++) {

                    allNotReversedPieces->removeAt(allNotReversedPieces->size() - 1);
                }
            }
            return 0;
        }

        // さらに隣の駒へ移動
        x += toX;
        y += toY;
    }

    if (recordFlag) {

        // 記憶していた駒の位置
        for (int i = 0; i < result; i++) {

            allNotReversedPieces->removeAt(allNotReversedPieces->size() - 1);
        }
    }
    return 0;
}

/**
 * 裏返した色を返す
 */
int ReversiBoard::turnColor(int piece) {
    return piece * -1;
}

/**
 * pieceを(x, y)に置いたときに、裏返す駒をすべて記憶
 */
bool ReversiBoard::recordReversiblePieces(int x, int y, int piece) {

    // x, yの範囲をチェック
    if (!checkRange(x, y)) return false;

    // すでに駒が置かれているかをチェック
    if (board[x][y] != EMPTY) return false;

    // 駒位置記憶オブジェクトを初期化
    delete allNotReversedPieces;
    allNotReversedPieces = new HList<Point*>();

    // 裏返せる位置を記憶
    countReversiblePieceToOneAngle(x, y,  1,  0, piece, true);
    countReversiblePieceToOneAngle(x, y,  1,  1, piece, true);
    countReversiblePieceToOneAngle(x, y,  0,  1 ,piece, true);
    countReversiblePieceToOneAngle(x, y, -1,  1, piece, true);
    countReversiblePieceToOneAngle(x, y, -1,  0, piece, true);
    countReversiblePieceToOneAngle(x, y, -1, -1, piece, true);
    countReversiblePieceToOneAngle(x, y,  0, -1, piece, true);
    countReversiblePieceToOneAngle(x, y,  1, -1, piece, true);

    return true;
}

// 次手のプレイヤーを判断
void ReversiBoard::setNextHand() {

    if (isTherePlace(turnColor(currentHand))) {
        currentHand = turnColor(currentHand);
    } else if (isTherePlace(currentHand)) {

        // 続けてもう1手
    } else {

        // 両プレイヤー共に置く場所がない
        currentHand = EMPTY;
    }

    return;
}

// 盤面上に指定されたpieceをおくことができるかを判断
bool ReversiBoard::isTherePlace(int piece) {

    int result = 0;

    // 駒の個数を数える
    for (int x = 0; x < BOARDW; x++) {
        for (int y = 0; y < BOARDH; y++) {
            result += countReversiblePieces(x, y, piece);
        }
    }

    return result > 0;
}
