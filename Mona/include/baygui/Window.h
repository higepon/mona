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

#ifndef __BAYGUI_FORM_H__
#define __BAYGUI_FORM_H__

namespace baygui
{
	/**
	 ウィンドウ部品.
	 1プロセスで複数のウィンドウを持つことができる。
	 */
	class Window : public Control
	{
		friend class Control;
	
	protected:
		/** 閉じるボタンが押されたかどうか */
		bool isCloseButtonPushed;
		/** マウスボタンが押されたときの状態 */
		NCState ncState;
	
	private:
		/** 透過率 */
		double opacity;
		/** 移動領域のハンドル */
		dword overlap;
		/** マウス移動中の座標 */
		Point ptRevRect;
		/** 内部バッファー */
		_P<Bitmap> formBuffer;
	
	public:
		virtual char* className() { return "System.Mona.Windows.Window"; }
		
		Window();
		
		virtual ~Window();
		
		/** 部品を作成したあとに呼ぶ */
		virtual void onStart();
		
		/** 部品が破棄されるときに呼ばれる */
		virtual void onExit();
		
		/** イベントを投げる */
		virtual void postEvent(Event *e);
		
		/** タイトルを設定する */
		void setTitle(const char* title);
		
		/**
		 アプリケーションを開始する.
		 メインウィンドウのみに対して呼ぶことができる。
		*/
		void run();
	
	protected:
		/** 非表示時に呼ばれる */
		virtual void onHide();
		
		/** 削除時に呼ばれる */
		virtual void onErase();
		
		/** 部品のどこをクリックしたのか調べる */
		virtual NCState NCHitTest(int x, int y);
		
		/** 内部領域を実際に再描画する前に呼ばれる */
		virtual void drawInternal();
		
		/** イベント発生時に呼ばれる */
		virtual void onEvent(Event *e) {}
	};
}

#endif  // __BAYGUI_FORM_H__
