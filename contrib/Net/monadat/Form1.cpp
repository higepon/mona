// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#ifdef MONA
#include <monapi/Array.h>
#include <monapi/messages.h>
#include <monalibc.h>
#else
#include <stdio.h>
#endif
#include <monapi/Keys.h>
#include <gui/System/Math.h>
#include <gui/System/Collections/ArrayList.h>
#include <gui/System/Mona/Forms/Application.h>
#include <gui/System/Mona/Forms/Form.h>
#include <gui/System/Mona/Forms/ScrollableControl.h>
#include "SheetTab.h"
#include "http.h"

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;
using namespace System::Mona::Forms;

extern String StringDecode(const char* data, int offset, int len);
extern bool StringCompare(const char* data, int offset, int len, const char* str);

class RichTextObject : public Object
{
protected:
    Size size;
    String url;

public:
    Color ForeColor, BackColor;

    RichTextObject()
    {
        this->ForeColor = Color::get_Empty();
        this->BackColor = Color::get_Empty();
    }

    virtual ~RichTextObject() {}
    virtual void Draw(_P<Graphics> g, int x, int y) {}
    inline Size get_Size() { return this->size; }
    inline int get_Width() { return this->size.Width; }
    inline int get_Height() { return this->size.Height; }

    int get_Anchor()
    {
        int len = this->url.get_Length();
        if (len == 0) return 0;

        for (int i = len - 1; i >= 0; i--)
        {
            if (this->url[i] == '/')
            {
                int ret = 0;
                for (i++; i < len; i++)
                {
                    wchar ch = this->url[i];
                    if (ch < '0' || ch > '9') break;
                    ret *= 10;
                    ret += ch - '0';
                }
                return ret;
            }
        }
        return 0;
    }
};

class RichTextString : public RichTextObject
{
private:
    String text;
    _P<System::Drawing::Font> font;
    bool bold, ul;

public:
    RichTextString(String text)
    {
        this->text = text;
        this->font = Control::get_DefaultFont();
        this->size = this->font->GetSize(text);
        this->bold = this->ul = false;
    }

    RichTextString(String text, Color fore, bool bold = false, bool ul = false)
    {
        this->text = text;
        this->ForeColor = fore;
        this->font = Control::get_DefaultFont();
        this->size = this->font->GetSize(text);
        if (bold) this->size.Width += text.get_Length();
        this->bold = bold;
        this->ul = ul;
    }

    virtual ~RichTextString() {}

    virtual void Draw(_P<Graphics> g, int x, int y)
    {
        if (!this->bold)
        {
            g->DrawString(this->text, this->font, this->ForeColor, x, y);
        }
        else
        {
            int xx = x;
            FOREACH (wchar, ch, this->text)
            {
                this->font->Draw(g, ch, this->ForeColor, xx++, y);
                this->font->Draw(g, ch, this->ForeColor, xx, y);
                xx += this->font->GetSize(ch).Width;
            } END_FOREACH
        }
        if (ul)
        {
            int yy = y + this->size.Height - 1;
            g->DrawLine(this->ForeColor, x, yy, x + this->size.Width - 1, yy);
        }
    }
};

class RichTextLink : public RichTextString
{
public:
    RichTextLink(String text, String url)
        : RichTextString(text, Color::get_Blue(), false, true)
    {
        this->url = url;
    }

    virtual ~RichTextLink() {}
};

class RichTextBitmap : public RichTextObject
{
private:
    _P<Bitmap> bmp;

public:
    RichTextBitmap(_P<Bitmap> bmp)
    {
        this->bmp = bmp;
        this->size = Size(bmp->get_Width(), bmp->get_Height());
    }

    virtual ~RichTextBitmap() {}

    virtual void Draw(_P<Graphics> g, int x, int y)
    {
        g->DrawImage(this->bmp, x, y);
    }
};

class RichTextLine : public Object
{
private:
    int width;
    _P<ArrayList<_P<RichTextObject> > > objects;

public:
    Color ForeColor, BackColor;
    int Indent, Height, Y, Line;

public:
    RichTextLine() : width(0), Indent(0), Y(0), Line(0)
    {
        this->objects = new ArrayList<_P<RichTextObject> >();
        this->Height = Control::get_DefaultFont()->get_Height() + 1;
        this->ForeColor = Color::get_Black();
        this->BackColor = Color::get_Empty();
    }

    virtual ~RichTextLine() {}
    inline int get_Count() { return this->objects->get_Count(); }
    inline int get_Width() { return this->width; }

    void Add(_P<RichTextObject> object)
    {
        this->objects->Add(object);
        Size sz = object->get_Size();
        this->width += sz.Width;
        if (this->Height < sz.Height) this->Height = sz.Height;
        if (object->ForeColor.ToArgb() == 0) object->ForeColor = this->ForeColor;
        if (object->BackColor.ToArgb() == 0) object->BackColor = this->BackColor;
    }

    void Draw(_P<Graphics> g, int x, int y)
    {
        x += this->Indent;
        FOREACH_AL (_P<RichTextObject>, obj, this->objects)
        {
            Size sz = obj->get_Size();
            obj->Draw(g, x, y + (this->Height - sz.Height) / 2);
            x += sz.Width;
        } END_FOREACH_AL
    }

    _P<RichTextObject> GetObject(int x)
    {
        int xx = this->Indent;
        FOREACH_AL (_P<RichTextObject>, obj, this->objects)
        {
            int w = obj->get_Width();
            if (xx <= x && x < xx + w) return obj;
            xx += w;
        } END_FOREACH_AL
        return NULL;
    }
};

static _P<Bitmap> MonaLogo, Hearts;

class DatThread : public Object
{
private:
    _P<ArrayList<_P<RichTextLine> > > headers, lines;
    _P<RichTextLine> header, title;
    int count, width, height;

public:
    int X, Y, DrawStart, DrawEnd;

    DatThread(_A<char> data) : width(0), height(0), X(0), Y(0), DrawStart(-1), DrawEnd(-1)
    {
        if (MonaLogo == NULL)
        {
#ifdef MONA
            String bundlePath = MonAPI::System::getBundlePath();
            MonaLogo = new Bitmap(bundlePath + "/MONALOGO.BM5");
#else
            MonaLogo = new Bitmap("dat\\MonaLogo.bmp");
#endif
            Hearts = new Bitmap(8, 12);
            _P<Graphics> g = Graphics::FromImage(Hearts);
            Color red = Color::get_Red();
            g->FillRectangle(red, 0, 1, 3, 4);
            g->FillRectangle(red, 4, 1, 3, 4);
            g->FillRectangle(red, 1, 2, 5, 6);
            g->FillRectangle(red, 2, 8, 3, 2);
            g->SetPixel(1,  0, red);
            g->SetPixel(5,  0, red);
            g->SetPixel(3, 10, red);
            g->Dispose();
        }
        this->SetData(data);
    }

    virtual ~DatThread() {}
    inline int get_Width() { return this->width; }
    inline int get_Height() { return this->height; }
    inline int get_LineCount() { return this->lines->get_Count(); }

    _P<RichTextLine> GetHeader(int index)
    {
        if (index < 0 || index >= this->headers->get_Count()) return NULL;
        return this->headers->get_Item(index);
    }

    _P<RichTextLine> GetLine(int line)
    {
        if (line < 0 || line >= this->lines->get_Count()) return NULL;
        return this->lines->get_Item(line);
    }

    void SetData(_A<char> data)
    {
        this->count = 0;

        this->headers = new ArrayList<_P<RichTextLine> >();
        this->lines   = new ArrayList<_P<RichTextLine> >();

        this->header = new RichTextLine();
        this->header->BackColor = Color::FromArgb(153, 179, 204);
        this->header->Add(new RichTextBitmap(MonaLogo));
        this->Add(this->header);

        this->title = new RichTextLine();
        this->title->BackColor = Color::FromArgb(221, 221, 221);
        this->title->Height = 30;
        this->title->Indent = 32;
        this->Add(this->title);

        this->Add(new RichTextLine());
        for (int i = 0, pos = 0; i <= data.get_Length(); i++)
        {
            char ch = i < data.get_Length() ? data[i] : '\r';
            if (ch == '\r' || ch == '\n')
            {
                if (i > pos) this->Add(data.get(), pos, i - pos);
                pos = i + 1;
            }
        }
        int y = 0;
        FOREACH_AL (_P<RichTextLine>, line, this->lines)
        {
            line->Y = y;
            y += line->Height;
        } END_FOREACH_AL
        this->height = y;
    }

    void Add(const char* data, int offset, int len)
    {
        int p = 0, hdr = 0;
        _P<RichTextLine> hdrl = new RichTextLine();
        hdrl->Indent = 8;
        char buf[16];
        sprintf(buf, "%d", ++this->count);
        hdrl->Add(new RichTextString(buf, Color::get_Black(), false, true));
        hdrl->Add(new RichTextString(" 名前： "));
        String name, trip, url;
        _P<RichTextLine> line;
        for (int i = 0; i < len; i++)
        {
            if (data[offset + i] == '&' && StringCompare(data, offset + i, len - i, "&hearts;"))
            {
                if (i > p) line->Add(new RichTextString(StringDecode(data, offset + p, i - p)));
                line->Add(new RichTextBitmap(Hearts));
                i += 7;
                p = i + 1;
                continue;
            }
            else if (data[offset + i] != '<' || i >= len - 1)
            {
                continue;
            }

            bool sep = data[offset + i + 1] == '>';
            if (sep && hdr < 3)
            {
                String s = StringDecode(data, offset + p, i - p);
                switch (hdr++)
                {
                    case 0:
                        if (s.get_Length() > 0) name = s;
                        break;
                    case 1:
                        if (s == "")
                        {
                            hdrl->Add(new RichTextString(name, Color::FromArgb(34, 139, 34), true));
                            if (trip.get_Length() > 0)
                            {
                                hdrl->Add(new RichTextString(trip, Color::FromArgb(34, 139, 34)));
                            }
                        }
                        else
                        {
                            hdrl->Add(new RichTextString(name, Color::get_Blue(), true, true));
                            const char* br = " [";
                            if (trip.get_Length() > 0)
                            {
                                hdrl->Add(new RichTextString(trip, Color::get_Blue(), false, true));
                                br = "[";
                            }
                            hdrl->Add(new RichTextString(br + s + "]", Color::FromArgb(85, 0, 255), true));
                        }
                        break;
                    case 2:
                        hdrl->Add(new RichTextString(" 投稿日： " + s));
                        this->Add(hdrl);
                        this->headers->Add(hdrl);
                        line = new RichTextLine();
                        break;
                }
                i++;
                p = i + 1;
            }
            else if (StringCompare(data, offset + i, len - i, "</b>"))
            {
                name = StringDecode(data, offset + p, i - p);
                i += 3;
                p = i + 1;
            }
            else if (StringCompare(data, offset + i, len - i, "<b>"))
            {
                trip = StringDecode(data, offset + p, i - p);
                i += 2;
                p = i + 1;
            }
            else if (sep || StringCompare(data, offset + i, len - i, "<br>"))
            {
                if (i > p) line->Add(new RichTextString(StringDecode(data, offset + p, i - p)));
                line->Indent = 24;
                this->Add(line);
                line = new RichTextLine();
                i += sep ? 1 : 3;
                p = i + 1;
            }
            else if (StringCompare(data, offset + i, len - i, "<a href=\""))
            {
                if (i > p) line->Add(new RichTextString(StringDecode(data, offset + p, i - p)));
                i += 9;
                p = i;
                for (; i < len && data[offset + i] != '\"'; i++);
                url = StringDecode(data, offset + p, i - p);
                for (; i < len && data[offset + i] != '>'; i++);
                p = i + 1;
            }
            else if (StringCompare(data, offset + i, len - i, "</a>"))
            {
                if (i > p) line->Add(new RichTextLink(StringDecode(data, offset + p, i - p), url));
                i += 3;
                p = i + 1;
                url = "";
            }
        }
        if (p < len)
        {
            if (this->title->get_Count() == 0)
            {
                this->title->Add(new RichTextString(
                    StringDecode(data, offset + p, len - p), Color::FromArgb(51, 102, 153), true));
            }
        }
        this->Add(line);
    }

    void Add(_P<RichTextLine> line)
    {
        line->Line = this->lines->get_Count();
        this->lines->Add(line);
        int w = line->Indent + line->get_Width();
        if (this->width < w) this->width = w;
    }

    void Draw(_P<Graphics> g, Point p, Size sz)
    {
        this->DrawStart = -1;
        this->DrawEnd = this->lines->get_Count();
        g->FillRectangle(Color::get_White(), 0, 0, sz.Width, sz.Height);
        int bottom = p.Y + sz.Height, i = 0;
        FOREACH_AL (_P<RichTextLine>, line, this->lines)
        {
            int t = line->Y, b = t + line->Height;
            if ((p.Y <= t && t < bottom)
                || (p.Y <= b && b < bottom)
                || (t <= p.Y && p.Y < b)
                || (t <= bottom && bottom < b))
            {
                if (this->DrawStart < 0) this->DrawStart = i;
                int ly = line->Y - p.Y;
                if (line->BackColor.ToArgb() != 0)
                {
                    g->FillRectangle(line->BackColor, 0, ly, sz.Width, line->Height);
                }
                line->Draw(g, -p.X, ly);
            }
            else if (this->DrawStart >= 0)
            {
                this->DrawEnd = i;
                break;
            }
            i++;
        } END_FOREACH_AL
    }

    _P<RichTextLine> GetLineFromPosition(int y)
    {
        if (this->DrawStart < 0) return NULL;

        for (int i = this->DrawStart; i < this->DrawEnd; i++)
        {
            _P<RichTextLine> line = this->lines->get_Item(i);
            if (line->Y <= y && y < line->Y + line->Height) return line;
        }
        return NULL;
    }
};

class ToolTip : public Form
{
protected:
    _P<ArrayList<_P<RichTextLine> > > lines;

public:
    ToolTip()
    {
        this->lines = new ArrayList<_P<RichTextLine> >;
    }

    virtual ~ToolTip() {}

    void Add(_P<RichTextLine> line)
    {
        this->lines->Add(line);
    }

    void AdjustSize()
    {
        if (this->lines->get_Count() == 0) return;

        int w = 0, h = 0;
        FOREACH_AL (_P<RichTextLine>, line, this->lines)
        {
            int lw = line->Indent + line->get_Width();
            if (w < lw) w = lw;
            h += line->Height;
        } END_FOREACH_AL
        this->set_Size(Size(w + 4, h + 4));
    }

protected:
    NCState NCHitTest(int x, int y)
    {
        return NCState_None;
    }

    virtual void OnPaint()
    {
        _P<Graphics> g = Graphics::FromImage(this->buffer);
        int w = this->get_Width(), h = this->get_Height();
        g->FillRectangle(Color::FromArgb(255, 255, 225), 0, 0, w, h);
        g->DrawRectangle(Color::get_Black(), 0, 0, w - 1, h - 1);
        int y = 2;
        FOREACH_AL (_P<RichTextLine>, line, this->lines)
        {
            line->Draw(g, -2, y);
            y += line->Height;
        } END_FOREACH_AL
        g->Dispose();
    }
};

class DatViewer : public ScrollableControl
{
private:
    _P<ArrayList<_P<DatThread> > > dats;
    _P<DatThread> current;
    _P<SheetTab> tab;
    String message;
    _P<Timer> timer1;
    _P<RichTextObject> anchor;
    _P<ToolTip> toolTip;
    Point toolTipPos;

public:
    DatViewer()
    {
        this->dats = new ArrayList<_P<DatThread> >();
        this->tab = new SheetTab();
        this->tab->add_SelectedIndexChanged(new EventHandler<DatViewer>(this, &DatViewer::tab_SelectedIndexChanged));
        this->get_Controls()->Add(this->tab.get());
        this->vScrollBar->SmallChange++;

        this->timer1 = new Timer();
        this->timer1->set_Interval(1000);
        this->timer1->add_Tick(new EventHandler<DatViewer>(this, &DatViewer::timer1_Tick));
    }

    virtual ~DatViewer() {}

    virtual void Create()
    {
        ScrollableControl::Create();
        this->timer1->Start();
    }

    virtual void Dispose()
    {
        this->timer1->Dispose();
        ScrollableControl::Dispose();
    }

    void OpenURL(String name, const char* host, const char* path)
    {
        this->tab->AddSelect(name);
        this->message = String(path) + "最新スレッド取得中。。。";
        this->Refresh();

        _P<DatThread> dat;
        Buffer* data = http_get(host, path);
        dat = new DatThread(_A<char>((char*)data->get(), data->size(), false));
        delete data;
        this->dats->Add(dat);
        this->message = "";

    }

    void Open(String name, const char* path)
    {
        this->tab->AddSelect(name);
        this->message = String(path) + " を読み込み中です。。。";
        this->Refresh();

        _P<DatThread> dat;
#ifdef MONA
        monapi_cmemoryinfo* mi = monapi_call_file_decompress_st5_file(path, MONAPI_FALSE);
        if (mi != NULL)
        {
            dat = new DatThread(_A<char>((char*)mi->Data, mi->Size, false));
            monapi_cmemoryinfo_dispose(mi);
            monapi_cmemoryinfo_delete(mi);
        }
#else
        FILE* f = fopen(path, "rb");
        if (f != NULL)
        {
            fseek(f, 0, SEEK_END);
            int len = ftell(f);
            _A<char> data(len);
            fseek(f, 0, SEEK_SET);
            fread(data.get(), 1, len, f);
            fclose(f);
            dat = new DatThread(data);
        }
#endif
        this->dats->Add(dat);
        this->message = "";
    }

    void set_SelectedIndex(int n)
    {
        this->tab->set_SelectedIndex(n);
    }

    _P<RichTextObject> GetObject(int x, int y)
    {
        if (this->current == NULL) return NULL;

        Point p = this->get_ScrollPosition();

        _P<RichTextLine> line = this->current->GetLineFromPosition(p.Y + y);
        if (line == NULL) return NULL;

        return line->GetObject(p.X + x);
    }

    void JumpTo(int res)
    {
        if (res == 0) return;

        int ds = this->current->DrawStart;
        _P<RichTextLine> hdr = this->current->GetHeader(res - 1);
        if (hdr->Line < ds) this->set_ScrollPosition(Point(0, hdr->Y));
    }

    void Form_KeyDown(_P<Object> sender, _P<KeyEventArgs> e)
    {
        switch (e->KeyCode)
        {
            case MonAPI::Keys::Up:
            case MonAPI::Keys::NumPad8:
                this->vScrollBar->set_Value(this->vScrollBar->get_Value() - this->vScrollBar->SmallChange);
                break;
            case MonAPI::Keys::Down:
            case MonAPI::Keys::NumPad2:
                this->vScrollBar->set_Value(this->vScrollBar->get_Value() + this->vScrollBar->SmallChange);
                break;
            case MonAPI::Keys::Left:
            case MonAPI::Keys::NumPad4:
                this->hScrollBar->set_Value(this->hScrollBar->get_Value() - this->hScrollBar->SmallChange);
                break;
            case MonAPI::Keys::Right:
            case MonAPI::Keys::NumPad6:
                this->hScrollBar->set_Value(this->hScrollBar->get_Value() + this->hScrollBar->SmallChange);
                break;
            case MonAPI::Keys::PageUp:
            case MonAPI::Keys::NumPad9:
                this->vScrollBar->set_Value(this->vScrollBar->get_Value() - this->vScrollBar->LargeChange);
                break;
            case MonAPI::Keys::PageDown:
            case MonAPI::Keys::NumPad3:
                this->vScrollBar->set_Value(this->vScrollBar->get_Value() + this->vScrollBar->LargeChange);
                break;
            case MonAPI::Keys::Home:
            case MonAPI::Keys::NumPad7:
                this->vScrollBar->set_Value(0);
                break;
            case MonAPI::Keys::End:
            case MonAPI::Keys::NumPad1:
                this->vScrollBar->set_Value(this->vScrollBar->Maximum);
                break;
        }
    }

private:
    void tab_SelectedIndexChanged(_P<Object> sender, _P<EventArgs> e)
    {
        int sel = this->tab->get_SelectedIndex();
        if (this->dats->get_Count() <= sel) return;

        if (this->current != NULL)
        {
            Point p = this->get_ScrollPosition();
            this->current->X = p.X;
            this->current->Y = p.Y;
        }

        this->current = this->dats->get_Item(this->tab->get_SelectedIndex());
        this->set_ScrollArea(Rectangle(0, 0, this->current->get_Width(), this->current->get_Height()));
        this->set_ScrollPosition(Point(this->current->X, this->current->Y));
    }

    void timer1_Tick(_P<Object> sender, _P<EventArgs> e)
    {
        this->timer1->Stop();
        if (this->anchor == NULL) return;

        int res = this->anchor->get_Anchor();
        if (res == 0) return;

        _P<RichTextLine> hdr = this->current->GetHeader(res - 1);
        int end = this->current->get_LineCount();
        _P<RichTextLine> next = this->current->GetHeader(res);
        if (next != NULL) end = next->Line;

        this->toolTip = new ToolTip();
        this->toolTip->set_Location(this->toolTipPos);
        for (int i = hdr->Line; i < end - 1; i++)
        {
            this->toolTip->Add(this->current->GetLine(i));
        }
        this->toolTip->AdjustSize();
        this->toolTip->Show();
    }

    void DisposeToolTip()
    {
        if (this->toolTip == NULL) return;

        this->toolTip->Hide();
        this->toolTip->Dispose();
        this->toolTip = NULL;
#ifndef MONA
        this->get_Parent()->Refresh();
#endif
    }

protected:
    virtual void SetScrollBarPosition()
    {
        Size sz = this->get_ClientSize();
        int ds = ScrollBar::DefaultSize;
        int tw = 242;
        this->tab->set_Bounds(Rectangle(0, sz.Height - ds, tw, ds));
        this->hScrollBar->set_Bounds(Rectangle(tw, sz.Height - ds, sz.Width - tw - ds, ds));
        this->vScrollBar->set_Bounds(Rectangle(sz.Width - ds, 0, ds, sz.Height - ds));
    }

    virtual void OnPaint()
    {
        _P<Graphics> g = this->CreateGraphics();
        Size sz = this->get_ClientSize();
        sz.Width  -= ScrollBar::DefaultSize + 4;
        sz.Height -= ScrollBar::DefaultSize + 4;
        if (message.get_Length() > 0)
        {
            g->FillRectangle(this->get_BackColor(), 0, 0, sz.Width, sz.Height);
            _P<Font> f = Control::get_DefaultFont();
            Size fsz = g->MeasureString(this->message, f);
            int mx = (sz.Width  - fsz.Width ) / 2;
            int my = (sz.Height - fsz.Height) / 2;
            g->DrawString(message, f, Color::get_Black(), mx, my);
        }
        else if (this->current != NULL)
        {
            Point p = this->get_ScrollPosition();
            this->current->Draw(g, p, sz);
        }
        g->Dispose();

        ScrollableControl::OnPaint();
    }

    virtual void OnScroll(_P<EventArgs> e)
    {
        ScrollableControl::OnScroll(e);
        this->Refresh();
    }

    virtual void OnMouseDown(_P<MouseEventArgs> e)
    {
        this->timer1->Stop();
        this->DisposeToolTip();
        ScrollableControl::OnMouseDown(e);
        if (e->Button != 1) return;

        _P<RichTextObject> obj = this->GetObject(e->X, e->Y);
        if (obj != NULL) this->JumpTo(obj->get_Anchor());
    }

    virtual void OnMouseMove(_P<MouseEventArgs> e)
    {
        this->timer1->Stop();
        this->DisposeToolTip();
        ScrollableControl::OnMouseMove(e);

        this->anchor = this->GetObject(e->X, e->Y);
        if (this->anchor == NULL || this->anchor->get_Anchor() == 0) return;

        this->toolTipPos = this->PointToScreen(Point(e->X + 16, e->Y + 16));
        this->timer1->Start();
    }
};

class Form1 : public Form
{
private:
    _P<DatViewer> dat;

public:
    Form1()
    {
        this->InitializeComponent();
    }

private:
    void InitializeComponent()
    {
        int w = 512, h = 400;
        this->set_Location(Point(48, 16));
        this->set_ClientSize(Size(w, h));
        this->set_Text("MonaDat");

        this->dat = new DatViewer();
        this->dat->set_Bounds(Rectangle(2, 2, w - 4, h - 4));
        this->get_Controls()->Add(this->dat.get());

        this->add_KeyDown(new KeyEventHandler<DatViewer>(this->dat, &DatViewer::Form_KeyDown));
    }

public:
    static void Main(Array<String> args)
    {
        _P<Form1> f = new Form1();
        f->Show();

        f->dat->OpenURL("現行スレ", "pc11.2ch.net", "/os/dat/1264212631.dat");

        char name[8], path[34];
        for (int i = 2; i <= 11; i++)
        {
            sprintf(name, "%d", i);
#ifdef MONA
            sprintf(path, "%s/%02d.DT5", MonAPI::System::getBundlePath(), i);
#else
            sprintf(path, "dat\\%02d.dat", i);
#endif
            f->dat->Open(name, path);
        }
        f->dat->set_SelectedIndex(0);
        Application::Run(f.get());
    }
};

SET_MAIN_CLASS(Form1)
