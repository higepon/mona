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
        /** カーソルの位置 */
        int textPtr;
        /** 文字列の長さ */
        int textLen;
        /** オフセットX */
        int offx;
        /** オフセットY */
        int offy;
        /** 内部文字列 */
        char text[MAX_TEXT_LEN];

    protected:
        /** テキストイベント */
        Event textEvent;
        ImeManager* _imeManager;

    private:
        /** 1文字挿入する */
        virtual void insertCharacter(char c);

        /** 一文字削除する */
        virtual void deleteCharacter();

    public:
        /** コンストラクタ */
        TextField();

        /** デストラクタ */
        virtual ~TextField();

        /**
         テキストを設定する
         @param text
         */
        virtual void setText(const String& text);

        /** テキストを得る */
        inline char* getText() { return this->text; }

        /** 描画ハンドラ */
        virtual void paint(Graphics* g);

        /** イベントハンドラ */
        virtual void processEvent(Event* event);
    };
}

#endif // _TEXTFIELD_H_INCLUDED_
