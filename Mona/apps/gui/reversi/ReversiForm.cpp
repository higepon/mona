#include <gui/System/Console.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Button.h>
#include <gui/System/Mona/Forms/Label.h>
#include <gui/System/Mona/Forms/Form.h>
#include "Piece.h"
#include "ReversiBoard.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Mona::Forms;

#define PIECE_SIZE 25
#define BOARD_SIZE (PIECE_SIZE * 8)

class ReversiForm : public Form, public MonAPI::Observer
{
private:
    _P<Piece> pieces[8][8];
    _P<Button> button1;
    _P<Label> label1;
    _P<ReversiBoard> board;

public:
    ReversiForm(_P<Bitmap> white, _P<Bitmap> black)
    {
        this->InitializeComponent();

        board.Set(new ReversiBoard(), true);
        board->addObserver(this);
        this->button1->add_Click(new EventHandler<ReversiForm>(this, &ReversiForm::button1_Click));

        // 描画用盤面初期化
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                this->pieces[x][y] = new Piece(board->getPiece(x, y), x, y, white, black);
                this->pieces[x][y]->set_Bounds(Rectangle(x * PIECE_SIZE, y * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE));
                this->pieces[x][y]->add_Click(new EventHandler<ReversiForm>(this, &ReversiForm::piece_Click));
                this->get_Controls()->Add(this->pieces[x][y].get());
            }
        }

        refreshLables();
    }

private:
    void InitializeComponent()
    {
        // Form
        this->set_Location(::System::Drawing::Point(20, 64));
        this->set_ClientSize(Size(BOARD_SIZE + 20, BOARD_SIZE + 20));
        this->set_Text("Reversi");

        // ボタン
        this->button1 = new Button();
        this->button1->set_Bounds(Rectangle(0, BOARD_SIZE, 50, 15));
        this->button1->set_Text("reset");
        this->get_Controls()->Add(this->button1.get());

        // ラベル
        this->label1 = new Label();
        this->label1->set_Bounds(Rectangle(50, BOARD_SIZE, 50, 15));
        this->get_Controls()->Add(this->label1.get());
    }

    void refreshLables()
    {
        this->label1->set_Text(this->board->getCurrentHand() == ReversiBoard::BLACK ? " black" : " white");
    }

    void button1_Click(Pointer<Object> sender, Pointer<EventArgs> e)
    {
        board->resetBoard();
        refreshLables();
    }

    void piece_Click(Pointer<Object> sender, Pointer<EventArgs> e)
    {
        _P<Piece> p = sender.get();
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
        refreshLables();
    }

    void update(MonAPI::Observable* o, void* e)
    {
        if (e == NULL)
        {
            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    this->pieces[x][y]->SetState(this->board->getPiece(x, y));
                    this->pieces[x][y]->Refresh();
                }
            }
        }
        else
        {
            Point2D* point = (::Point2D*)e;
            int x = point->x;
            int y = point->y;
            this->pieces[x][y]->SetState(this->board->getPiece(x, y));
            this->pieces[x][y]->Refresh();

            sleep(100);
        }
    }

public:
    static void Main(Array<String> args)
    {
        _P<Bitmap> bitmapWhite = new Bitmap("/WHITE.BMP");
        _P<Bitmap> bitmapBlack = new Bitmap("/BLACK.BMP");

        if (bitmapBlack.get() == NULL || bitmapWhite.get() == NULL)
        {
            Console::WriteLine("Can not open resource file!");
            return;
        }

        Application::Run(new ReversiForm(bitmapWhite, bitmapBlack));
    }
};

SET_MAIN_CLASS(ReversiForm)
