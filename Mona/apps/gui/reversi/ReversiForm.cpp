#include <gui/System/Console.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Form.h>
#include "Piece.h"
#include "ReversiBoard.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

#define PIECE_SIZE 15
#define BOARD_SIZE (PIECE_SIZE * 8)

class ReversiForm : public Form, public MonAPI::Observer
{
public:
    Pointer<Piece> pieces[8][8];

    ReversiForm()
    {
        this->InitializeApplication();
        this->InitializeComponent();
    }

private:

    Pointer<ReversiBoard> board;

    int InitializeApplication()
    {
        board = new ReversiBoard();
        board->addObserver(this);
        return 0;
    }

    void InitializeComponent()
    {
        this->set_Location(::System::Drawing::Point(20, 64));
        this->set_ClientSize(Size(BOARD_SIZE + 20, BOARD_SIZE + 20));

        // 描画用盤面初期化
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                this->pieces[x][y] = new Piece(board->getPiece(x, y), x, y);
                this->pieces[x][y]->set_Bounds(Rectangle(x * PIECE_SIZE, y * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));
                this->pieces[x][y]->add_Click(new EventHandler<ReversiForm>(this, &ReversiForm::button1_Click));
                this->get_Controls()->Add(this->pieces[x][y].get());
            }
        }
    }

    void button1_Click(Pointer<Object> sender, Pointer<EventArgs> e)
    {
        Piece* p = (Piece*)(sender.get());
        int x = p->GetX();
        int y = p->GetY();

        // 範囲チェック
        if (!board->checkRange(x, y))
        {
            return;
        }

        // 現在のプレイヤーがこまをおく
        int current = board->getCurrentHand();
        if (!(board->setPiece(x, y, current)))
        {
            return;
        }

        // ひっくり返されるこまを一枚ずつ描画
        while (board->existNotReversedPieces())
        {
            board->reverseNext();
        }
    }

    void update(MonAPI::Observable* o, void* e)
    {
        if (e == NULL)
        {
        }
        else
        {
            ::Point* point = (::Point*)e;

            int x = point->x;
            int y = point->y;
            this->pieces[x][y]->SetState(this->board->getPiece(x, y));

            sleep(100);
            pieces[x][y]->Refresh();
        }
    }

public:
    static void Main(Array<String> args)
    {
        Application::Run(new ReversiForm());
    }
};

SET_MAIN_CLASS(ReversiForm)
