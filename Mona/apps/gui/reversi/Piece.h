#ifndef __REVERSI_PIECE_H__
#define __REVERSI_PIECE_H__

#include <gui/System/Mona/Forms/Control.h>

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

class Piece : public Control
{
protected:
    int state;
    int x;
    int y;
    _P<Bitmap> white;
    _P<Bitmap> black;

public:
    Piece();
    Piece(int state, int x, int y, _P<Bitmap> white, _P<Bitmap> black);
    virtual ~Piece();

    virtual void SetState(int state) { this->state = state; this->OnPaint(); }
    virtual int GetState() const { return this->state; }
    virtual int GetX() const { return this->x; }
    virtual int GetY() const { return this->y; }

protected:
    virtual void OnPaint();
    virtual void OnMouseUp(_P<MouseEventArgs> e);
};

#endif
