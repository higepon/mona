/*
Copyright (c) 2004 bayside
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#if !defined(_IMESERVER_H_INCLUDED_)
#define _IMESERVER_H_INCLUDED_

#define MAX_TEXT_LEN     64
#define BASICDIC_NAME    "/BASICDIC.TX5"
#define BASICDIC_ENTRIES 24253

/** IMEサーバークラス */
class ImeServer : public MonAPI::Server {
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
	ImeServer::ImeServer();
	virtual ImeServer::~ImeServer();
	virtual void service();
};

#endif // _IMESERVER_H_INCLUDED_
