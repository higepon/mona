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
#include "document.hpp"

namespace gnote {
	//
	Document::Document() {
		line = new Vector();
	}
	//
	Document::Document(const Document& d) {
		line = new Vector();
		for (int ii = 1; ii <= d.GetMaxLineNumber(); ii++) {
			line->add(d.GetLine(ii));
		}
	}
	//
	Document& Document::operator=(const Document& d) {
		delete line;
		line = new Vector();
		for (int ii = 1; ii <= d.GetMaxLineNumber(); ii++) {
			line->add(d.GetLine(ii));
		}
		return *this;
	}
	//
	Document::~Document() {
		if (line) {
			delete line;
		}
	}
	//
	int Document::GetLength() const {
		int r = 0;
		for (int ii = 0; ii < line->size(); ii++) {
			r += ((String*)line->get(ii))->length();
		}
		return r;
	}
	//
	String* Document::GetLine(int y) const{
		return (y > 0 && line->size() >= y) ? (String*)(line->get(y - 1)) : 0;
	}
	//
	String Document::GetSubLine(int y, int x, int l) const {
		String* s = GetLine(y);
		if (!s || l <= 0 || s->length() < x + l) {
			return 0;
		}
		return s->substring(x - 1, (l) ? l : s->length() - x);
	}
	//
	int Document::GetMaxLineNumber() const {
		return line->size();
	}
	//
	bool Document::Delete(int y, int x) {
		if (y < 1 || y > line->size()) {
			return false;
		}
		String* s = GetLine(y);
		if (s->length() == x && s->charAt(x - 1) == '\n') {
			if (!DeleteChar(*s, x)) {
				return false;
			}
			String* next = GetLine(y + 1);
			if (next) {
				if (!InsertString(*s, *next, x) || !Delete(y + 1)) {
					return false;
				}
			}
		} else {
			if (!DeleteChar(*s, x)) {
				return false;
			}
		}
		if (s->length() < 1) {
			line->remove(y - 1);
		}
		return true;
	}
	//
	bool Document::Delete(int y) {
		if (y < 1 || y > line->size()) {
			return false;
		}
		line->remove(y - 1);
		return true;
	}
	//
	bool Document::Delete(int y, int x, int l) {
		if (y < 1 || y > line->size()) {
			return false;
		}
		String* s = GetLine(y);
		if (!s || l <= 0 || s->length() < x + l) {
			return false;
		}
		return DeleteChar(*s, x, l);
	}
	// too bad :-)
	bool Document::Insert(char c, int y, int x) {
		if (y < 1 || y > line->size() + 1) {
			return false;
		}
		String* s = 0;
		if (y == line->size() + 1) {
			if (x != 1) {
				return false;
			}
			if (y == 1) {
				line->add(s = new String(""));
			} else {
				String* t = GetLine(y - 1);
				if (t->charAt(t->length() - 1) != '\n') {
					return false;
				}
				line->add(s = new String(""));
			}
		} else {
			s = GetLine(y);
		}
		if (c == '\n' && s->length() >= x) {
			if (InsertEmptyLine(y + 1)) {
				return false;
			}
			String* tmp = GetLine(y + 1);
			*tmp = s->substring(x - 1, s->length() - x + 1);
			*s = s->substring(0, x - 1);
			*s += '\n';
		} else {
			if (!InsertChar(*s, c, x)) {
				return false;
			}
		}
		return true;
	}
	//
	bool Document::InsertEmptyLine(int y) {
		if (y < 1 || y > line->size() + 1) {
			return false;
		}
		line->add(new String(""));
		for (int ii = line->size() - 2; ii >= y - 1; ii--) {
			*GetLine(ii + 2) = *GetLine(ii + 1);
		}
		*GetLine(y) = String("\n");
		return false;
	}
	// too bad :-)
	bool Document::Insert(const Document& d, int y, int x) {
		if (y < 1 || y > line->size() + 1) {
			return false;
		}
		for (int ii = d.GetMaxLineNumber(); ii > 0; ii--) {
			String* s = d.GetLine(ii);
			for (int jj = s->length(); jj > 0; jj--) {
				if (!Insert(s->charAt(jj - 1), y, x)) {
					return false;
				}
			}
		}
		return true;
	}
	//
	bool Document::Insert(const String& s, int y, int x) {
		if (y < 1 || y > line->size() + 1) {
			return false;
		}
		String* t = 0;
		if (y == line->size() + 1) {
			if (x != 1) {
				return false;
			}
			if (y == 1) {
				line->add(t = new String(""));
			} else {
				String* u = GetLine(y - 1);
				if (u->charAt(u->length() - 1) != '\n') {
					return false;
				}
				line->add(t = new String(""));
			}
		} else {
			t = GetLine(y);
		}
		return InsertString(*t, s, x);
	}
	//
	bool Document::DeleteChar(String& s, int x, int l) {
		if (x < 1 || x + l > s.length() + 1) {
			return false;
		}
		String tmp;
		if (x > 1) {
			tmp = s.substring(0, x - 1);
		}
		if (x <= s.length()) {
			tmp += s.substring(x + l - 1, s.length() - x - l + 1);
		}
		s = tmp;
		return true;
	}
	//
	bool Document::InsertChar(String& s, char c, int x) {
		if (x < 1 || x > s.length() + 1) {
			return false;
		}
		String tmp;
		if (x > 1) {
			tmp = s.substring(0, x - 1);
		}
		tmp += c;
		if (x <= s.length()) {
			tmp += s.substring(x - 1, s.length() - x + 1);
		}
		s = tmp;
		return true;
	}
	//
	bool Document::InsertString(String& s, const String& t, int x) {
		if (x < 1 || x > s.length() + 1) {
			return false;
		}
		String tmp;
		if (x > 1) {
			tmp = s.substring(0, x - 1);
		}
		tmp += t;
		if (x <= s.length()) {
			tmp += s.substring(x - 1, s.length() - x + 1);
		}
		s = tmp;
		return true;
	}
	// too bad :-)
	bool Document::Append(const String& s) {
		bool r = true;
		for (int ii = 0; ii < s.length(); ii++) {
			r &= Append(s.charAt(ii));
			if (!r) {
				break;
			}
		}
		return r;
	}
	//
	bool Document::Append(char c) {
		if (line->size() > 0) {
			String* s = (String*)line->get(line->size() - 1);
			if (s->charAt(s->length() - 1) == '\n') {
				s = new String();
				*s += c;
				line->add(s);
			} else {
				*s += c;
			}
		} else {
			String* s = new String();
			*s += c;
			line->add(s);
		}
		return true;
	}
	//
	bool Document::Delete(int sy, int sx, int ey, int ex) {
		if (sy == ey) {
			int lx, rx;
			if (sx > ex) {
				lx = ex;
				rx = sx;
			} else {
				lx = sx;
				rx = ex;
			}
			String* s;
			if (sy < 1 || sy > line->size() || lx < 1
				|| rx > (s = (String*)line->get(sy - 1))->length() + 1) {
				return false;
			}
			*s = s->substring(0, lx - 1) + s->substring(rx - 1, s->length() - rx + 1);
			if (s->length() < 1) {
				line->remove(sy - 1);
			} else if (s->charAt(s->length() - 1) != '\n' && line->size() > sy) {
				*s += *(String*)line->get(sy);
				line->remove(sy);
			}
		} else {
			int tx, ty, bx, by;
			if (sy > ey) {
				ty = ey;
				tx = ex;
				by = sy;
				bx = sx;
			} else {
				ty = sy;
				tx = sx;
				by = ey;
				bx = ex;
			}
			if (ty < 1 || ey > line->size() + 1) {
				return false;
			}
			for (int ii = by; ii >= ty ; ii--) {
				String* s;
				if (ii > line->size() || !(s = (String*)line->get(ii - 1))) {
					continue;
				}
				if (ii == ty && tx != 1) {
					*s = s->substring(0, tx - 1);
					if (line->size() > ii) {
						*s += *(String*)line->get(ii);
						line->remove(ii);
					}
				} else if (ii == by) {
					*s = s->substring(bx - 1, s->length() - bx + 1);
					if (s->length() < 1) {
						line->remove(ii - 1);
					}
				} else {
					line->remove(ii - 1);
				}
			}
		}
		return true;
	}

	//
	Document Document::GetSubDocument(int sy, int sx, int ey, int ex) const {
		Document d;
		if (sy == ey) {
			int lx, rx;
			if (sx > ex) {
				lx = ex;
				rx = sx;
			} else {
				lx = sx;
				rx = ex;
			}
			String* s;
			if (sy > 0 && sy <= line->size() && lx > 0 && rx <= (s = (String*)line->get(sy - 1))->length() + 1) {
				String* t = new String();
				*t = s->substring(lx - 1, rx - lx);
				d.Append(*t);
			}
		} else {
			int tx, ty, bx, by;
			if (sy > ey) {
				ty = ey;
				tx = ex;
				by = sy;
				bx = sx;
			} else {
				ty = sy;
				tx = sx;
				by = ey;
				bx = ex;
			}
			if (ty > 0 && ey <= line->size() + 1) {
				for (int ii = ty; ii <= by && ii <= line->size(); ii++) {
					String* s = (String*)line->get(ii - 1);
					if (ii == ty && tx != 1) {
						String t = s->substring(tx - 1, s->length() - tx + 1);
						if (t.length() > 0) {
							d.Append(t);
							}
					} else if (ii == by && s->length() + 1 != bx) {
						String t = s->substring(0, bx - 1);
						if (t.length() > 0) {
							d.Append(t);
						}
					} else {
						String t = *s;
						d.Append(t);
					}
				}
			}
		}
		return d;
	}
}
