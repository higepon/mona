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

#ifndef __BAYGUI_POINT_H__
#define __BAYGUI_POINT_H__

namespace baygui
{
	/** XとYを持つ構造体 */
	struct Point
	{
		int X, Y;
		
		Point() : X(0), Y(0) {}
		Point(int x, int y) : X(x), Y(y) {}
		
		inline Point operator +(const Point& p) { Point ret = *this; return ret += p; }
		
		inline Point operator -(const Point& p) { Point ret = *this; return ret -= p; }
		
		inline Point& operator +=(const Point& p)
		{
			this->X += p.X;
			this->Y += p.Y;
			return *this;
		}
		
		inline Point& operator -=(const Point& p)
		{
			this->X -= p.X;
			this->Y -= p.Y;
			return *this;
		}
		
		inline bool operator ==(const Point& p)
		{
			return this->X == p.X && this->Y == p.Y;
		}
		
		inline bool operator !=(const Point& p)
		{
			return this->X != p.X || this->Y != p.Y;
		}
		
		static Point get_Empty() { return Point(); }
	};
}

#endif  // __BAYGUI_POINT_H__
