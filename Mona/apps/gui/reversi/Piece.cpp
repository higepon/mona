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

Piece::Piece(int state, int x, int y) : state(state), x(x), y(y)
{
}

Piece::~Piece()
{
}

void Piece::OnPaint()
{
    _P<Graphics> g = this->CreateGraphics();
    int w = this->get_Width();
    int h = this->get_Height();

    ::System::Drawing::Color c;

    if (this->state == ReversiBoard::BLACK)
    {
        c = ::System::Drawing::Color::get_Black();
    }
    else if (this->state == ReversiBoard::WHITE)
    {
        c = ::System::Drawing::Color::get_White();
    }
    else
    {
        c = ::System::Drawing::Color::FromArgb(50, 120, 2, 2);
    }

    g->FillRectangle(c, 0, 0, w, h);
    g->DrawRectangle(::System::Drawing::Color::get_Black(), 0, 0, w, h);

    g->Dispose();
}

void Piece::OnMouseUp(_P<MouseEventArgs> e)
{
    printf("on mouse up");
    if (e->Button == 1) this->OnClick(EventArgs::get_Empty());
    BASE::OnMouseUp(e);
}
