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
#ifndef __GNOTE_CANVAS_HPP__
#define __GNOTE_CANVAS_HPP__

#include "baygui.h"
#include "trash/teventlistener.hpp"
#include "document.hpp"
#include "cursol.hpp"

using namespace trash;

namespace gnote {
	class Canvas : public Component {
	public:
		enum {
			MAX_CANVASX = 50,
			MAX_CANVASY = 20
		};
	protected:
		TEventListener* listener;
		static const String ruler;
		Document* document;
		Cursol* cursol;
		int gridWidth;
		int gridHeight;
	public:
		Canvas();
		virtual ~Canvas() {};
		inline void SetEventListener(TEventListener* l) { listener = l; }
		virtual void processEvent(Event* event);
		virtual void paint(Graphics* g);
		virtual void repaintAt(int cy = 1, int l = 20);
		inline void SetDocument(Document* d) { document = d; }
		inline void SetCursol(Cursol* c) { cursol = c; }
		inline String ToCanvasString(int wy) { return ToCanvasString(*document, wy); }
		inline int GetGridWidth() { return gridWidth; }
		inline int GetGridHeight() { return gridHeight; }
		inline int ToCanvasX(int wy, int wx, int l = 1) { return ToCanvasX(*document, wy, wx, l); }
		inline int ToCanvasX(const Document& d, int wy, int wx, int l = 1) { return (d.GetMaxLineNumber() >= wy) ? ToCanvasX(*d.GetLine(wy), wx, l) : 1; };
		int ToCanvasX(const String& s, int wx, int l = 1);
		inline int ToCanvasY(int wy, int t = 1) { return wy - t + 1; }
		inline int ToWorldX(int wy, int cx, int l = 1) { return ToWorldX(*document, wy, cx, l); }
		inline int ToWorldX(const Document& d, int wy, int cx, int l = 1) {return (d.GetMaxLineNumber() < wy) ? 1 : ToWorldX(*d.GetLine(wy), cx, l); }
		int ToWorldX(const String& s, int cx, int l = 1);
		inline int ToWorldY(int cy, int t = 1) { return t + cy - 1; }
		String ToCanvasString(const Document& d, int wy) { return (d.GetMaxLineNumber() < wy) ? String() : ToCanvasString(*d.GetLine(wy)); }
		String ToCanvasString(const String& s);
	protected:
		void DrawRuler(Graphics* g, const Document& d, const Cursol& c);
		void DrawBr(Graphics* g, int cy, int cx);
		void DrawCaretLine(Graphics* g, const Cursol& c);
		void DrawCaret(Graphics* g, const Cursol& c);
		void DrawDocument(Graphics* g, const Document& d, const Cursol& c);
		void DrawRange(Graphics* g, const Document& d, const Cursol& c);
	};
}

#endif
