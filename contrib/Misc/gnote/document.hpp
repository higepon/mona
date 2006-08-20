/*
 Copyright (c) 2005 hetappi
 All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to
 do so, provided that the above copyright notice(s) and this permission
 notice appear in all copies of the Software and that both the above
 copyright notice(s) and this permission notice appear in supporting
 documentation.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
 IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
 LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
 ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

 Except as contained in this notice, the name of a copyright holder shall not
 be used in advertising or otherwise to promote the sale, use or other
 dealings in this Software without prior written authorization of the
 copyright holder.
*/
#ifndef __GNOTE_DOCUMENT_HPP__
#define __GNOTE_DOCUMENT_HPP__

#include "baygui.h"

namespace gnote {
	class Document {
	protected:
		// Vector‚æ‚èList‚Ì•û‚ª‚¢‚¢‚æ‚Ëc
		Vector* line;
	public:
		Document();
		~Document();
		Document(const Document& d);
		Document& operator=(const Document& d);
		String* GetLine(int y) const;
		String GetSubLine(int y, int x, int l = 0) const;
		Document GetSubDocument(int by, int bx, int ey, int ex) const;
		int GetMaxLineNumber() const;
		int GetLength() const;
		bool Delete(int y);
		bool Delete(int y, int x);
		bool Delete(int y, int x, int l);
		bool Delete(int by, int bx, int ey, int ex);
		bool Insert(char c, int y, int x);
		bool Insert(const String& s, int y, int x);
		bool Insert(const Document& d, int y, int x);
		bool Append(char c);
		bool Append(const String& s);
		void inline Clear() const { line->removeAll(); };
	protected:
		bool InsertEmptyLine(int y);
		bool DeleteChar(String& s, int x, int l = 1);
		bool InsertChar(String& s, char c, int x);
		bool InsertString(String& s, const String& t, int x);
	};
}

#endif
