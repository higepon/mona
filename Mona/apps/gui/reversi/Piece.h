#ifndef __REVERSI_PIECE_H__
#define __REVERSI_PIECE_H__

#include <gui/System/Mona/Forms/Control.h>

using namespace System;
using namespace System::Mona::Forms;

class Piece : public Control
{
protected:
    int state;
    int x;
    int y;

public:
    virtual String get_TypeName() { return "Reversi.Piece"; }
    Piece();
    Piece(int state, int x, int y);
    virtual ~Piece();

    virtual void Create();
    virtual void SetState(int state) { this->state = state;}
    virtual int GetState() const { return this->state; }
    virtual int GetX() const { return this->x; }
    virtual int GetY() const { return this->y; }

protected:
    virtual void OnPaint();
    virtual void OnMouseMove(_P<MouseEventArgs> e);
    virtual void OnMouseDown(_P<MouseEventArgs> e);
    virtual void OnMouseUp(_P<MouseEventArgs> e);
};

#endif
