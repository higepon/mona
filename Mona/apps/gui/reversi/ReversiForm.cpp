// This file's encoding is UTF-8.

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
    _A<_A<_P<Piece> > > pieces;
    _P<Button> button1;
    _P<Label> label1, label2, label3, label4;
    _P<ReversiBoard> board;

public:
    ReversiForm(_P<Bitmap> white, _P<Bitmap> black)
    {
        this->InitializeComponent();

        board.Set(new ReversiBoard(), true);
        board->addObserver(this);
        this->button1->add_Click(new EventHandler<ReversiForm>(this, &ReversiForm::button1_Click));

        // 描画用盤面初期化
        this->pieces.Alloc(8);
        for (int x = 0; x < 8; x++)
        {
            this->pieces[x].Alloc(8);
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
        this->set_ClientSize(Size(BOARD_SIZE, BOARD_SIZE + 20));
        this->set_Text("Reversi");

        // ボタン
        this->button1 = new Button();
        this->button1->set_Bounds(Rectangle(0, BOARD_SIZE + 2, 50, 16));
        this->button1->set_Text("reset");
        this->get_Controls()->Add(this->button1.get());

        // ラベル
        this->label1 = new Label();
        this->label2 = new Label();
        this->label3 = new Label();
        this->label4 = new Label();
        this->label1->set_Bounds(Rectangle( 60, BOARD_SIZE + 2, 40, 16));
        this->label2->set_Bounds(Rectangle(105, BOARD_SIZE + 2, 20, 16));
        this->label3->set_Bounds(Rectangle(130, BOARD_SIZE + 2, 40, 16));
        this->label4->set_Bounds(Rectangle(175, BOARD_SIZE + 2, 20, 16));
        this->label1->set_Text("  red");
        this->label2->set_Text("0");
        this->label3->set_Text(" white");
        this->label4->set_Text("0");
        this->get_Controls()->Add(this->label1.get());
        this->get_Controls()->Add(this->label2.get());
        this->get_Controls()->Add(this->label3.get());
        this->get_Controls()->Add(this->label4.get());
    }

    void refreshLables()
    {
        switch (this->board->getCurrentHand())
        {
            case ReversiBoard::BLACK:
            	this->label1->set_ForeColor(Color::get_White());
                this->label1->set_BackColor(Color::get_Blue());
                this->label3->set_ForeColor(this->get_ForeColor());
                this->label3->set_BackColor(this->get_BackColor());
                break;
            case ReversiBoard::WHITE:
                this->label1->set_ForeColor(this->get_ForeColor());
                this->label1->set_BackColor(this->get_BackColor());
                this->label3->set_ForeColor(Color::get_White());
                this->label3->set_BackColor(Color::get_Blue());
                break;
        }

        int black = 0, white = 0;
        for (int y = 0; y < 8; y++)
        {
            for (int x = 0; x < 8; x++)
            {
                switch (this->board->getPiece(x, y))
                {
                    case ReversiBoard::BLACK:
                        black++;
                        break;
                    case ReversiBoard::WHITE:
                        white++;
                        break;
                }
            }
        }
        char buf[16];
        sprintf(buf, "%d", black);
        this->label2->set_Text(buf);
        sprintf(buf, "%d", white);
        this->label4->set_Text(buf);
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
