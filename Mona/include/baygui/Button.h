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

#ifndef __BAYGUI_BUTTON_H__
#define __BAYGUI_BUTTON_H__

namespace baygui
{
	/**
	 ボタン部品.
	 マウスを押すことで引っ込む。
	 */
	class Button : public Control
	{
	protected:
		/** ボタンが押されたかどうか */
		bool isPushed;
		/** ラベル */
		String text;
		
	public:
		virtual char* className() { return "baygui.ui.Button"; }
		Button();
		virtual ~Button();
		void setText(const char* text);
		inline char* getText() { return this->text.getBytes(); }
		virtual void onStart();
		
	protected:
		virtual void onPaint(_P<Graphics> g);
		virtual void onEvent(Event *e);
	};
}

#endif  // __BAYGUI_BUTTON_H__
