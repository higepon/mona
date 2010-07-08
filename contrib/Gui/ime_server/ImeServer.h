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

#if !defined(_IMESERVER_H_INCLUDED_)
#define _IMESERVER_H_INCLUDED_

#define MAX_TEXT_LEN     128
#define BASICDIC_NAME    "/APPS/BASICDIC.TXT"

/** IMEサーバークラス */
class ImeServer {
private:
    /** 辞書データ */
    char *basicDic;
    /** 辞書データサイズ */
    int basicDicSize;

protected:
    virtual bool loadDictionary();
    virtual int  getKanji(char *yomi, HList<MonAPI::CString>* result);
    virtual bool getYomi(char *kanji, char *result);
    virtual bool getKana(char *inputString, char *result);
    virtual bool getRoman(char *kana, char *result);

public:
    ImeServer();
    virtual ~ImeServer();
    virtual void service();
};

#endif // _IMESERVER_H_INCLUDED_
