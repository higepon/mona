#include <monapi.h>
#include "Piece.h"
#include "ReversiBoard.h"

#define BASE Control

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

Piece::Piece() : state(ReversiBoard::EMPTY)
{
}

Piece::Piece(int state, int x, int y, _P<Bitmap> white, _P<Bitmap> black) : state(state), x(x), y(y), white(white), black(black)
{
}

Piece::~Piece()
{
}

void Piece::OnPaint()
{
    Control::OnPaint();
    _P<Graphics> g = this->CreateGraphics();

    int w = this->get_Width();
    int h = this->get_Height();

    ::System::Drawing::Color c;

    if (this->state == ReversiBoard::BLACK)
    {
        g->DrawImage(this->black, 0, 0);
    }
    else if (this->state == ReversiBoard::WHITE)
    {
        g->DrawImage(this->white, 0, 0);
    }
    else
    {
        c = ::System::Drawing::Color::get_Black();
        g->FillRectangle(c, 0, 0, w, h);
    }

    g->DrawRectangle(::System::Drawing::Color::get_White(), 0, 0, w, h);
    g->Dispose();
}

void Piece::OnMouseUp(_P<MouseEventArgs> e)
{
    if (e->Button == 1) this->OnClick(EventArgs::get_Empty());
    BASE::OnMouseUp(e);
}
