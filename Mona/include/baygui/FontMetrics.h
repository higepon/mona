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

#if !defined(_FONTMETRICS_H_INCLUDED_)
#define _FONTMETRICS_H_INCLUDED_

/**
 フォントマネージャクラス
*/
class FontMetrics : public Object {
protected:
	/** シングルトンインスタンス */
	static FontMetrics *instance;
	/** フォント名 */
	char name[9];
	int width;
	int height;
	int offsetListLength;
	int *offsetList;
	unsigned char *fp;
	FontMetrics::FontMetrics();
	virtual void loadFont(char *path);
	dword fontsvrID;

public:
	//monapi_cmemoryinfo *fpMemory;
	static FontMetrics *getInstance();
	virtual FontMetrics::~FontMetrics();
	virtual const char *className() {return "baygui.FontMetrics";}
	virtual bool decodeCharacter(unsigned int utf16, int *width, int *height, char *data);
	virtual int getWidth(char *str);
	virtual int getHeight();
};

#endif // _FONTMETRICS_H_INCLUDED_
