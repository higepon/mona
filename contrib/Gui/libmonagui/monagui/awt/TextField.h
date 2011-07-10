/*
Copyright (c) 2005 bayside
Copyright (c) 2011 Higepon

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#if !defined(_TEXTFIELD_H_INCLUDED_)
#define _TEXTFIELD_H_INCLUDED_

/** テキストフィールド文字列最大長 */
#define MAX_TEXT_LEN 128

class ImeManager;

namespace monagui {
    /**
     テキストボックスクラス
    */
    class TextField : public Container {
    protected:
        String text_;
        int cursor_;
        String accumUtf8_;
        int offx;
        int offy;
        bool selected_;
        int selectBeginningOffset_;
        Event textEvent;
        ImeManager* _imeManager;
        dword borderColor_;

    public:
        void forward();  // public for testability
        void backward(); // public for testability
    protected:

        void initialize();

        virtual void processKeyEvent(KeyEvent* event);

        virtual void accumulateUtf8(char c)
        {
            accumUtf8_ += c;
        }

        virtual String & getAccumulateUtf8()
        {
            return accumUtf8_;
        }

        virtual void clearAccumulateUtf8()
        {
            accumUtf8_ = "";
        }

        void deleteBackward();

        void moveBeginningOfLine()
        {
            if(cursor_ != 0) {
                cursor_ = 0;
                repaint();
           }
        }

        void copy();
        void paste();
        void cut();

        void moveEndOfLine()
        {
            if (cursor_ != text_.length()) {
                cursor_ = text_.length();
                repaint();
            }
        }
        void deleteForward()
        {
            if (cursor_ < text_.length()) {
                if (cursor_ == 0) {
                    text_ = text_.substring(1, text_.length() - 1);
                } else if (cursor_ == text_.length() - 1) {
                    text_ = text_.substring(0, cursor_);
                } else {
                    String head = text_.substring(0, cursor_);
                    String rest = text_.substring(cursor_ + 1, text_.length() - cursor_ - 1);
                    text_ = head;
                    text_ += rest;
                }
                repaint();
            }
        }

        void killLine()
        {
            if (cursor_ == text_.length()) {
                return;
            } else {
                text_ = text_.substring(0, cursor_);
                repaint();
            }
        }

        void toggleSelect()
        {
            if (selected_) {
                selected_ = false;
            } else {
                selected_ = true;
                selectBeginningOffset_ = cursor_;
            }
        }
    public: /* public for testability */
        /** 1文字挿入する */
        virtual void insertCharacter(char c);
        virtual void insertStringTail(const String & c);
        /** 一文字削除する */
        virtual void deleteCharacter();
        virtual int getCursor() const
        {
            return cursor_;
        }

    public:
        /** コンストラクタ */
        TextField();
        /** デストラクタ */
        virtual ~TextField();
        virtual bool isImeOn() const;


        /**
         テキストを設定する
         @param text
         */
        virtual void setText(const String & text);
        virtual void setTextNoRepaint(const String & text);
        /** テキストを得る */
        inline const char* getText()
        {
            return (const char*)text_; }

        /** 描画ハンドラ */
        virtual void paint(Graphics* g);

        /** イベントハンドラ */
        virtual void processEvent(Event* event);
        virtual void setBorderColor(dword c)
        {
            borderColor_ = c;
        }
    };
}

#endif // _TEXTFIELD_H_INCLUDED_
