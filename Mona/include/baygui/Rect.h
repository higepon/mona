/*
Copyright (c) 2004 Tino, bayside
All rights reserved.

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

#ifndef __BAYGUI_RECT_H__
#define __BAYGUI_RECT_H__

namespace baygui
{
	/** X,Y,•,‚‚³‚ðŽ‚Â\‘¢‘Ì */
	struct Rect
	{
		int X, Y, Width, Height;
		
		Rect() : X(0), Y(0), Width(0), Height(0) {}
		Rect(int x, int y, int width, int height) : X(x), Y(y), Width(width), Height(height) {}
		Rect(const Point& p, const Dimention& sz) : X(p.X), Y(p.Y), Width(sz.Width), Height(sz.Height) {}
		
		void Intersect(const Rect& r);
		
		inline bool IntersectsWith(Rect r)
		{
			r.Intersect(*this);
			return r.Width != 0 && r.Height != 0;
		}
		
		inline int get_Left() const { return this->X; }
		
		inline int get_Top() const { return this->Y; }
		
		inline int get_Right() const { return this->X + this->Width; }
		
		inline int get_Bottom() const { return this->Y + this->Height; }
		
		inline Point get_Location() const { return Point(this->X, this->Y); }
		
		inline Dimention get_Size() const { return Dimention(this->Width, this->Height); }
		
		inline bool get_IsEmpty() const
		{
			return this->X == 0 && this->Y == 0 && this->Width == 0 && this->Height == 0;
		}
		
		inline void Offset(int dx, int dy) { this->X += dx; this->Y += dy; }
		
		inline void Offset(const Point& p) { this->X += p.X; this->Y += p.Y; }
		
		inline bool Contains(int x, int y) {
			return this->X <= x && x < this->X + this->Width && this->Y <= y && y < this->Y + this->Height;
		}
		
		inline bool Contains(Point pt) { return this->Contains(pt.X, pt.Y); }
	};
}

#endif  // __BAYGUI_RECT_H__
