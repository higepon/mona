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

#ifndef __BAYGUI_APPLICATION_H__
#define __BAYGUI_APPLICATION_H__

namespace baygui
{
	class Window;
	class Control;
	
	/**
	 アプリケーションの雛型.
	 アプリケーションごとに１プロセスとなる。
	 ウィンドウは１プロセスにいくつでも生成できる。
	 */
	class Application
	{
	private:
		/** ウィンドウ一覧 */
		static _P<ArrayList<_P<Window> > > forms;
		/** 直前に操作していた部品 */
		static _P<Control> prevControl;
		/** メインウィンドウ */
		static _P<Window> mainWindow;
		/** 終了したかどうか */
		static bool isExited;
		/** 押されているマウスのボタン */
		static int mouseButtons;
		
	public:
		/** デフォルトのフォントデータ */
		static unsigned char* defaultFontData;
		
		static void initialize();
		static void dispose();
		static void run();
		static void run(_P<Window> form);
		static void doEvents();
		static void processEvent(MessageInfo* m);
		static void exit();
		static void addWindow(_P<Window> f);
		static void removeWindow(_P<Window> f);
		static _P<Window> findWindow(int x, int y);
	};
}

#endif  // __BAYGUI_APPLICATION_H__
