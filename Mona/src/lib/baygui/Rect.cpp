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

#include <baygui.h>

namespace baygui
{
	/** 矩形をクリッピングする */
	void Rect::Intersect(const Rect& r)
	{
		int x1 = this->X, x2 = this->get_Right();
		int y1 = this->Y, y2 = this->get_Bottom();
		int x3 = r.X, x4 = r.get_Right(), y3 = r.Y, y4 = r.get_Bottom();
		
		if (x3 <= x1 && x1 <= x4) {
			this->Width = MIN(this->Width, x4 - x1);
		} else if (x3 <= x2 && x2 <= x4) {
			int dx = x3 - x1;
			this->X += dx;
			this->Width = MIN(r.Width, this->Width - dx);
		} else if (x1 < x3 && x4 < x2) {
			int dx1 = x3 - x1;
			int dx2 = x2 - x4;
			this->X += dx1;
			this->Width -= dx1 + dx2;
		} else {
			this->X = this->Width = 0;
		}
		
		if (y3 <= y1 && y1 <= y4) {
			this->Height = MIN(this->Height, y4 - y1);
		} else if (y3 <= y2 && y2 <= y4) {
			int dy = y3 - y1;
			this->Y += dy;
			this->Height = MIN(r.Height, this->Height - dy);
		} else if (y1 < y3 && y4 < y2) {
			int dy1 = y3 - y1;
			int dy2 = y2 - y4;
			this->Y += dy1;
			this->Height -= dy1 + dy2;
		} else {
			this->Y = this->Height = 0;
		}
	}
}
