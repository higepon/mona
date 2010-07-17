/*
Copyright (c) 2005 bayside

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

namespace baygui {
    /**
     テキストボックスクラス
    */
    class TextField : public Container {
    private:
        String text_;
        int cursor_;
        String accumUtf8_;

        /** オフセットX */
        int offx;
        /** オフセットY */
        int offy;

    protected:
        /** テキストイベント */
        Event textEvent;
        ImeManager* _imeManager;

    private:

        virtual void initialize();

        virtual void processKeyEvent(KeyEvent* event);

        void backspace();
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

    public: /* public for testability */
        /** 1文字挿入する */
        virtual void insertCharacter(char c);
        virtual void insertStringTail(const String & c);
        /** 一文字削除する */
        virtual void deleteCharacter();
        virtual bool cursorLeft();
        virtual bool cursorRight();
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
        /** テキストを得る */
        inline const char *getText()
        {
            return (const char*)(text_); }

        /** 描画ハンドラ */
        virtual void paint(Graphics* g);

        /** イベントハンドラ */
        virtual void processEvent(Event* event);
    };
}

#endif // _TEXTFIELD_H_INCLUDED_
