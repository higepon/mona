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

#ifndef __BAYGUI_TIMER_H__
#define __BAYGUI_TIMER_H__

#define MSG_GUI_TIMER 0x40f0

namespace baygui
{
	/** タイマークラス */
	class Timer : public Object
	{
	private:
		/** タイマー間隔 */
		int interval;
		/** エンドレスかどうか (デフォルトはtrue) */
		bool isEndless;
		/** スリープスレッドのID */
		dword sleep_tid;
		
	public:
		virtual char* className() { return "baygui.ui.Timer"; }
		
		Timer();
		
		virtual ~Timer();
		
		/** 新しいタイマーを設定するときはいったん破棄する必要がある */
		virtual void dispose();
		
		/** タイマー開始 */
		void start();
		
		/** タイマー停止 */
		void stop();
		
		/** タイマー間隔を得る */
		inline int getInterval() { return this->interval; }
		
		/** タイマー間隔を設定する */
		inline void setInterval(int interval) { this->interval = interval; }
	};
}

#endif  // __BAYGUI_TIMER_H__
