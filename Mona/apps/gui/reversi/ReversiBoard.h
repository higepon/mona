#ifndef _MONA_REVERSI_BOARD_
#define _MONA_REVERSI_BOARD_

#include <sys/types.h>

/*----------------------------------------------------------------------
    Point (x, y)の座標管理
----------------------------------------------------------------------*/
class Point {
  public:
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Point(Point* point) {
        this->x = point->x;
        this->y = point->y;
    }

    virtual ~Point() {}

  public:
    int x;
    int y;
};

/*----------------------------------------------------------------------
    Point3D (x, y, こま)の管理
----------------------------------------------------------------------*/
class Point3D {

  public:
    Point3D(int x, int y, int z) {
        this->x = x;
        this->y = y;
        this->z  = z;
    }

    virtual ~Point3D() {}
  public:
    int x;
    int y;
    int z;
};

/*----------------------------------------------------------------------
    リバーシ板(描画部は分離されているので他のOSでも使いまわせます)
----------------------------------------------------------------------*/
class ReversiBoard : public MonAPI::Observable {

  public:
    ReversiBoard();
    virtual ~ReversiBoard() {

        for (int i = 0; i< allNotReversedPieces->size(); i++) {
            delete (allNotReversedPieces->get(i));
        }

        for (int i = 0; i< allTurns->size(); i++) {
            delete (allTurns->get(i));
        }

        delete allNotReversedPieces;
        delete allTurns;
    }

  public:
    int countPieces(int piece);
    int getPiece(int x, int y);
    int getPiece(Point* point);
    int getCurrentHand() const;
    bool setPieces(int x, int y, int piece);
    bool existNotReversedPieces();
    void reverseNext();
    int getTurn() const;
    void undo();
    int countReversiblePieces(int x, int y, int piece);
    void resetBoard();
    bool setPiece(int x, int y, int piece);
    bool checkRange(int x, int y);

  private:
    void initBoard();
    void reversePiece(int x, int y);
    int countReversiblePieceToOneAngle(int x, int y, int toX,int toY, int piece, bool recordFlag);
    int turnColor(int piece);
    bool recordReversiblePieces(int x, int y, int piece);
    void setNextHand();
    bool isTherePlace(int piece);
    void init();
  public:
    static const int BLACK  = 1;
    static const int WHITE  = -1;
    static const int EMPTY  = 0;
    static const int BOARDW = 8;
    static const int BOARDH = 8;

  private:
    int board[8][8];
    int turn;
    int currentHand;
    List<Point*>* allNotReversedPieces;
    List<Point3D*>* allTurns;

};

#endif
