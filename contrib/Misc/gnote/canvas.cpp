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
#include "bmp.h"
#include "canvas.hpp"

namespace gnote {

	// :-)
	const String Canvas::ruler
		= String("0---------1---------2---------3---------4---------5---------6---------7---------8---------9---------0---------1---------2---------3---------4---------5---------6---------7---------8---------9---------");
	//
	Canvas::Canvas() : listener(0), document(0), cursol(0) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		FontMetrics* fm = getFontMetrics();
		gridWidth = fm->getWidth("0") + 2;
		gridHeight = fm->getHeight("0") + 2;
        logprintf("cursol=%x %d\n", cursol, __LINE__);
	}
	//
	void Canvas::processEvent(Event* event) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		Container* c = getParent();
		if (c) {
//			c->processEvent(event);
		}
	}
	// :-)
	void Canvas::repaintAt(int cy, int l) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		repaint();
	}
	//
	void Canvas::paint(Graphics* g) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		//
		g->setColor(Color::white);
		g->fillRect(0, 0, getWidth(), getHeight());
		g->setColor(Color::blue);
		g->drawLine(gridWidth * 4 + 4, gridHeight - 1, gridWidth * 4 + 4, getHeight());
		g->drawLine(gridWidth * 4 + 4, gridHeight - 1, getWidth(), gridHeight - 1);
		//
		if (cursol && document) {
            logprintf("g=%x cursol=%x %d\n", g, cursol, __LINE__);
			DrawRuler(g, *document, *cursol);
            logprintf("g=%x cursol=%x %d\n", g, cursol, __LINE__);
			if (cursol->range) {
				DrawRange(g, *document, *cursol);
			}
            logprintf("g=%x cursol=%x %d\n", g, cursol, __LINE__);
			DrawDocument(g, *document, *cursol);
            logprintf("g=%x cursol=%x %d\n", g, cursol, __LINE__);
			DrawCaretLine(g, *cursol);
			if (getMainWindow()->getFocused() && cursol->visible) {
				DrawCaret(g, *cursol);
			}
		}
	}
	//
	void Canvas::DrawRuler(Graphics* g, const Document& d, const Cursol& c) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		//
		g->setColor(Color::lightGray);
		g->fillRect((ToCanvasX(d, c.wy, c.wx, c.left) + 4) * gridWidth, 0, gridWidth, gridHeight - 1);
		//
		g->setColor(Color::blue);
		g->setFontStyle(Font::FIXED);
		g->drawString(ruler.substring((c.left - 1) % 100, Canvas::MAX_CANVASX), 5 * gridWidth, 0);
	}
	//
	void Canvas::DrawBr(Graphics* g, int cy, int cx) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		for (int xx = 0; xx < BMPH_BR_BMP_WIDTH; xx++) {
			for (int yy = 0; yy < BMPH_BR_BMP_HEIGHT; yy++) {
				if (bmph_pixel_data_br_bmp[yy][xx]) {
					g->drawPixel(
						(cx + 4) * gridWidth + xx,
						cy * gridHeight + 1 + yy,
						0xff000000 | bmph_color_palette[bmph_pixel_data_br_bmp[yy][xx]]);
				}
			}
		}
	}
	//
	void Canvas::DrawDocument(Graphics* g, const Document& d, const Cursol& c) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		g->setFontStyle(Font::FIXED);
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		for (int ii = 1; ii <= Canvas::MAX_CANVASY && c.top + ii - 1 <= d.GetMaxLineNumber(); ii++) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
			String* s = d.GetLine(c.top + ii - 1);
        logprintf("cursol=%x %d\n", cursol, __LINE__);
			if (s && s->length() > 0) {
				//
				const int BUFFER_SIZE = 5;
				char szNo[BUFFER_SIZE];
				int writtenSize = snprintf(szNo, BUFFER_SIZE, "%4d", (c.top + ii - 1) % 10000);
				// For now, snprint is just an alias for sprintf
				// So we check the size.
				ASSERT(writtenSize + 1 <= BUFFER_SIZE);
				g->setColor(Color::blue);
				g->drawString(szNo, 0, ii * gridHeight + 1);
				//
				g->setColor(Color::black);
				String cs = ToCanvasString(*s);
				cs = cs.substring(c.left - 1, cs.length() - c.left + 1);
				g->drawString(cs, 5 * gridWidth, ii * gridHeight + 1);
				if (cs.charAt(cs.length() - 1) == '\n') {
					DrawBr(g, ii, cs.length());
				}
			}
		}
        logprintf("cursol=%x %d\n", cursol, __LINE__);
	}
	//
	void Canvas::DrawRange(Graphics* g, const Document& d, const Cursol& c) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		g->setColor(Color::lightGray);
		if (c.rx != c.wx || c.ry != c.wy) {
			if (c.ry == c.wy) {
				int lwx = (c.rx > c.wx) ? c.wx : c.rx;
				int lcx = ToCanvasX(c.wy, lwx, c.left);
				int len = abs(ToCanvasX(c.wy, c.rx, c.left) - ToCanvasX(c.wy, c.wx, c.left));
				g->fillRect((lcx - 1 + 5) * gridWidth, ((c.wy - c.top) + 1) * gridHeight, len * gridWidth, gridHeight);
			} else {
				int ty, by, tx, bx;
				if (c.ry > c.wy) {
					ty = c.wy;
					by = c.ry;
					tx = c.wx;
					bx = c.rx;
				} else {
					ty = c.ry;
					by = c.wy;
					tx = c.rx;
					bx = c.wx;
				}
				for (int ii = ty; ii <= by; ii++) {
					String* s;
					if (ii < c.top || c.top + MAX_CANVASY < ii || !(s = document->GetLine(ii))) {
						continue;
					}
					int lcx;
					int rcx;
					if (ii == ty) {
						lcx = ToCanvasX(ii, tx, c.left);
						rcx = ToCanvasX(ii, s->length() + 1, c.left) + 1;
					} else if (ii == by) {
						lcx = ToCanvasX(ii, 1, c.left);
						rcx = ToCanvasX(ii, bx, c.left);
					} else {
						lcx = ToCanvasX(ii, 1, c.left);
						rcx = ToCanvasX(ii, s->length() + 1, c.left) + 1;
					}
					g->fillRect(
						(lcx - 1 + 5) * gridWidth, (ii - c.top + 1) * gridHeight,
						(rcx - lcx) * gridWidth, gridHeight);
				}
			}
		}
	}
	//
	void Canvas::DrawCaretLine(Graphics* g, const Cursol& c) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
        logprintf("g=%x cursol=%x %d\n", g, cursol, __LINE__);
		int cy = ToCanvasY(c.wy, c.top);
        logprintf("g=%x %d\n", g, __LINE__);
		g->setColor(Color::gray);
		g->drawLine(0, (cy + 1) * gridHeight, getWidth(), (cy + 1) * gridHeight);
	}
	//
	void Canvas::DrawCaret(Graphics* g, const Cursol& c) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		g->setColor(Color::gray);
		g->fillRect(
			(ToCanvasX(cursol->wy, c.wx, cursol->left) + 4) * gridWidth,
			ToCanvasY(c.wy, c.top) * gridHeight,
			3, gridHeight);
	}
	// :-)
	String Canvas::ToCanvasString(const String& s) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		String r("");
		const char* p = s.getBytes();
		int len = 0;
		while(*p) {
			if (*p == '\t') {
				for (int ii = 0; ii < 4 - len % 4; ii++) {
					r += (ii == 0) ? '^' : ' ';
				}
				len += (4 - len % 4);
			} else {
				r += *p;
				len++;
			}
			p++;
		}
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		return r;
	}
	// :-)
	int Canvas::ToCanvasX(const String& s, int wx, int l) {
        logprintf("cursol=%x %d\n", cursol, __LINE__);
		int clen = 1;
		const char* p = s.getBytes();
		for (int ii = 1; p && *p && *p != '\n' && ii < wx; ii++, p++) {
			if (*p == '\t') {
				clen += (4 - (clen - 1) % 4);
			} else {
				clen++;
			}
		}
		int result = clen - l + 1;
		return (result < 1) ? 1 : (result > Canvas::MAX_CANVASX) ? Canvas::MAX_CANVASX : result;
	}
	// :-)
	int Canvas::ToWorldX(const String& s, int cx, int l) {
		//
		int len = s.endsWith("\n") ? s.length() : s.length() + 1;
		if (ToCanvasX(s, len, l) <= cx) {
			return len;
		}
		//
		const char* p = s.getBytes();
		int result = 1;
		for (int clen = 1; *p && *p != '\n' && result <= s.length(); result++, p++) {
			if (*p == '\t') {
				clen += (4 - (clen - 1) % 4);
			} else {
				clen++;
			}
			if (clen - l + 1 > cx) {
				break;
			}
		}
		return result;
	}

}
