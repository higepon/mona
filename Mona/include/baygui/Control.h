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

#ifndef __BAYGUI_CONTROL_H__
#define __BAYGUI_CONTROL_H__

namespace baygui
{
	/**
	 部品親クラス.
	 すべての画面部品はこのクラスを継承して作成する。
	 GUIサーバー上のウィンドウオブジェクトとの仲立ちも行う。
	 */
	class Control : public Object
	{
		/**
		 部品リスト.
		 本当はContainerクラスが管理すべきものだが、TinoGUIでは部品が持っている。
		 */
		class ControlCollection : public ArrayList<_P<Control> >
		{
			friend class Control;
			
			private:
				_P<Control> target;
			
			public:
				ControlCollection() {}
				virtual ~ControlCollection() {}
		};
		
	public:
		/** 画面中のどこをクリックしたかで変わる値 */
		enum NCState { NCState_Client, NCState_None, NCState_TitleBar, NCState_CloseButton } ncState;
		
	private:
		/** 領域 */
		Rect rect;
		/** 表示可能かどうか */
		bool visible;
		/** フォーカスがあるかどうか */
		bool focused;
		/** 前景色 */
		unsigned int foreColor;
		/** 背景色 */
		unsigned int backColor;
		/** 透過色 */
		unsigned int transColor;
		/** 親部品 */
		_P<Control> parent;
		/** 部品リスト */
		_P<ControlCollection> children;
		/** 内部領域のサイズ (ウィンドウ等では外側の縁を除いたサイズ)  */
		Dimention innerSize;
		/** 前景色が変えられたかどうか */
		bool foreColorChanged;
		/** 背景色が変えられたかどうか */
		bool backColorChanged;
		
	protected:
		/** 内部データ */
		_P<Bitmap> buffer;
		/** オフセット */
		Point offset;
		/** マウスをクリックした座標 */
		Point clickPoint;
		/** GUIサーバー上のウィンドウオブジェクト */
		guiserver_window* _object;
	
	public:
		virtual char* className() { return "baygui.ui.Control"; }
		
		Control();
		
		virtual ~Control();
		
		/** ハンドルを得る */
		inline unsigned int getHandle() {
			return this->_object != NULL ? this->_object->Handle : 0;
		}
		
		/** 部品を作成したあとに呼ぶ */
		virtual void onStart();
		
		/** 部品が破棄されるときに呼ばれる */
		virtual void onExit();
		
		/** 部品を追加する */
		void add(_P<Control> control);
		
		/** 実際に再描画する */
		void repaint();
		
		/** イベントを投げる */
		virtual void postEvent(Event *e);
		
		/** 指定した座標を相対座標に変換する */
		Point pointToClient(Point p);
		
		/** 指定した座標を絶対座標に変換する */
		Point pointToScreen(Point p);
		
		/** 一番親の部品を取得する */
		_P<Control> getTopLevelControl();
		
		/** 自分の中におかれている部品を取得する (なければ自分を返す) */
		_P<Control> findChild(int x, int y);
		
		/** グラフィックオブジェクトを得る */
		virtual _P<Graphics> getGraphics();
		
		/** 親部品を得る */
		inline _P<Control> getParent() {
			return this->parent;
		}
		
		/** 部品リストを得る */
		inline _P<ControlCollection> getChildren() {
			return this->children;
		}
		
		/** 領域を得る */
		inline Rect getRect() {
			return this->rect;
		}
		
		/** 領域を設定する */
		inline void setRect(int x, int y, int width, int height) {
			this->rect.X = x;
			this->rect.Y = y;
			this->rect.Width = width;
			this->rect.Height = height;
		}
		
		inline int getX() { return this->rect.X; }
		
		inline int getY() { return this->rect.Y; }
		
		inline int getWidth() { return this->rect.Width; }
		
		inline int getHeight() { return this->rect.Height; }
		
		inline Point getLocation() { return this->rect.get_Location(); }
		
		void setLocation(int x, int y);
		
		inline Dimention getSize() { return this->rect.get_Size(); }
		
		inline void setSize(int width, int height) {
			this->rect.Width = width;
			this->rect.Height = height;
		}
		
		void setInnerSize(int width, int height);
		
		inline bool getVisible() { return this->visible; }
		
		void setVisible(bool v);
		
		inline bool getFocused() { return this->focused; }
		
		void setFocused(bool v);
		
		Dimention getInnerSize();
		
		/** 前景色を得る */
		inline unsigned int getForeground() { return this->foreColor; }
		
		/** 前景色を設定する */
		void setForeground(unsigned int c);
		
		/** 背景色を得る */
		inline unsigned int getBackground() { return this->backColor; }
		
		/** 背景色を設定する */
		void setBackground(unsigned int c);
		
		/** 透過色を得る */
		inline unsigned int getTransColor() { return this->transColor; }
		
		/** 透過色を設定する */
		void setTransColor(unsigned int c);
	
	protected:
		/** 内部領域を実際に再描画する */
		void repaintInternal();
		
		/** 内部領域を実際に再描画する前に呼ばれる */
		virtual void drawInternal() {}
		
		/** 表示時に呼ばれる */
		virtual void onShow();
		
		/** 非表示時に呼ばれる */
		virtual void onHide();
		
		/** 部品のどこをクリックしたのか調べる */
		virtual NCState NCHitTest(int x, int y);
		
		/** 描画時に呼ばれる */
		virtual void onPaint(_P<Graphics> g) {}
		
		/** イベント発生時に呼ばれる */
		virtual void onEvent(Event *e) {}
	};
}

#endif  // __BAYGUI_CONTROL_H__
