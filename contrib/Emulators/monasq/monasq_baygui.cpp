/**
 *	MonaSq/baygui ライブラリ定義
 */
/*
 *	Copyright (c) 2006 okayu punch
 *
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without restriction,
 *	including without limitation the rights to use, copy, modify, merge,
 *	publish, distribute, sublicense, and/or sell copies of the Software,
 *	and to permit persons to whom the Software is furnished to do so,
 *	subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *	THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "monasq.h"
#include "sqplus.h"
#include <map>

using namespace SqPlus;



//================================================================
// 定数
//================================================================

const int MONASQ_EVENT_PAINT   = 0x00100001;


// baygui 定数定義
struct BayguiConstantData {
	int value;
	const char* name;
} g_bayguiConstantTable[] = {
	{ MONASQ_EVENT_PAINT,		"PAINT" },

	{ Event::KEY_PRESSED,		"KEY_PRESSED" },
	{ Event::KEY_RELEASED,		"KEY_RELEASED" },
	{ Event::MOUSE_PRESSED,		"MOUSE_PRESSED" },
	{ Event::MOUSE_RELEASED,	"MOUSE_RELEASED" },
	{ Event::MOUSE_DRAGGED,		"MOUSE_DRAGGED" },
	{ Event::MOUSE_MOVED,		"MOUSE_MOVED" },
	{ Event::FOCUS_IN,			"FOCUS_IN" },
	{ Event::FOCUS_OUT,			"FOCUS_OUT" },
	{ Event::TIMER,				"TIMER" },
	{ Event::TEXT_CHANGED,		"TEXT_CHANGED" },
	{ Event::ITEM_SELECTED,		"ITEM_SELECTED" },
	{ Event::BLOCK_INCLEMENT,	"BLOCK_INCLEMENT" },
	{ Event::BLOCK_DECLEMENT,	"BLOCK_DECLEMENT" },
	{ Event::CUSTOM_EVENT,		"CUSTOM_EVENT" },

	{ MouseEvent::BUTTON_LEFT,	"BUTTON_LEFT" },
	{ MouseEvent::BUTTON_RIGHT,	"BUTTON_RIGHT" },
	{ MouseEvent::BUTTON_MIDDLE,"BUTTON_MIDDLE" },

	{ Color::black,				"black" },
	{ Color::maroon,			"maroon" },
	{ Color::green,				"green" },
	{ Color::olive,				"olive" },
	{ Color::navy,				"navy" },
	{ Color::purple,			"purple" },
	{ Color::teal,				"teal" },
	{ Color::lightGray,			"lightGray" },
	{ Color::gray,				"gray" },
	{ Color::red,				"red" },
	{ Color::lime,				"lime" },
	{ Color::yellow,			"yellow" },
	{ Color::blue,				"blue" },
	{ Color::magenta,			"magenta" },
	{ Color::cyan,				"cyan" },
	{ Color::white,				"white" },
};


//================================================================
// SqGraphicsProxy
//================================================================
/**
 *	SqGraphicsProxy クラス
 *	<br>
 *	baygui::Graphics のプロキシクラス
 */
class SqGraphicsProxy {
public:
	Graphics* g;		// Graphics の実体または NULL を指すポインタ

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqGraphicsProxy() : g(NULL) {}
	
	void drawPixel(int x, int y, dword color) { if (g) g->drawPixel(x, y, color); }
	void drawLine(int x0, int y0, int x1, int y1) { if (g) g->drawLine(x0, y0, x1, y1); }
	void drawRect(int x, int y, int width, int height) { if (g) g->drawLine(x, y, width, height); }
	void drawCircle(int x0, int y0, int r) { if (g) g->drawCircle(x0, y0, r); }
	void drawString(const char* str, int x, int y) { if (g) g->drawString(str, x, y); }
	void fillCircle(int x0, int y0, int r) { if (g) g->fillCircle(x0, y0, r); }
	void fillRect(int x, int y, int width, int height) { if (g) g->fillRect(x, y, width, height); }
	void translate(int x, int y) { if (g) g->translate(x, y); }
	int getFontStyle() { return g ? g->getFontStyle() : 0; }
	void setClip(int cx, int cy, int cw, int ch) { if (g) g->setClip(cx, cy, cw, ch); }
	void setFontStyle(int style) { if (g) g->setFontStyle(style); }

	void drawImage(SquirrelObject so, int x, int y) {
		if (!g) return;
		SQObjectType objectType = (SQObjectType)so.GetType();
		Image* object = (Image*)so.GetInstanceUP(0);
		TRACE("GraphicsProxy.drawImage() - p:%08x\n", object);
		if (object == NULL || objectType != OT_INSTANCE) {
			monasq_printf("GraphicsProxy.drawImage() - Invalid arguments\n");
			return;
		}
		g->drawImage(object, x, y);
	}

	int setColor(HSQUIRRELVM v) {
		StackHandler sa(v);
		switch (sa.GetParamCount()) {
		case 2:
			if (g) g->setColor( sa.GetInt(2) );
			return SQ_OK;
		case 4:
			if (g) g->setColor(sa.GetInt(2), sa.GetInt(3), sa.GetInt(4));
			return SQ_OK;
		}
		return sq_throwerror(v,_T("Invalid arguments"));
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqGraphicsProxy, GraphicsProxy)

// 共有 SqGraphicsProxy インスタンス
SqGraphicsProxy g_sqGraphicsProxy;



//================================================================
// MonaSqObjectMap
//================================================================
/**
 *	baygui オブジェクトのポインタからSquirrelオブジェクトを探すためのマップ
 */
class MonaSqObjectMap {
	map<Object*, SquirrelObject> m_map;

public:
 	bool addObject(Object* ptr, SquirrelObject hInstance);
	bool removeObject(Object* ptr);
	SquirrelObject getInstance(Object* ptr);
};

/**
 *	baygui オブジェクトの登録
 */
bool
MonaSqObjectMap::addObject(Object* ptr, SquirrelObject object) {
	// 型チェック
	SQObjectType objectType = (SQObjectType)object.GetType();
	if (objectType != OT_INSTANCE) {
		monasq_printf("illegal baygui object instance type.\n  address:%d type : %d\n", ptr, objectType);
		return false;
	}
	
	// （上書き）登録
	m_map[ptr] = object;
	return true;
}

/**
 *	baygui オブジェクトの削除
 */
bool
MonaSqObjectMap::removeObject(Object* ptr) {
	map<Object*, SquirrelObject >::iterator iter = m_map.find(ptr);
	if (iter != m_map.end()) {
		m_map.erase( iter );
		return true;
	}
	return false;
}

/**
 *	baygui オブジェクトポインタから Squirrel オブジェクトを取得
 */
SquirrelObject
MonaSqObjectMap::getInstance(Object* ptr) {
	map<Object*, SquirrelObject>::iterator iter = m_map.find(ptr);
	if (iter != m_map.end()) {
		return iter->second;
	}
	return SquirrelObject();
}


// baygui コンポーネントマップ
MonaSqObjectMap g_objectMap;


inline SquirrelObject bayguiSqObject(Object* ptr) {
	return g_objectMap.getInstance(ptr);
}


//================================================================
// MonaSqEventHandlerMap
//================================================================
/**
 *	Squirrelスクリプトのイベントハンドラマップ
 */
class MonaSqEventHandlerMap {
	map<int, SquirrelFunction<bool> > m_map;

public:
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func);
	bool removeHandler(int eventId);
	SquirrelFunction<bool>* getHandler(int eventId);

	bool dispatchPaint(Graphics* g);
	bool dispatchEvent(Event *event);
};

/**
 *	イベントハンドラの登録
 */
bool
MonaSqEventHandlerMap::setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {

	// 型チェック
	SQObjectType objectType = (SQObjectType)calee.GetType();
	if (objectType != OT_TABLE && objectType != OT_CLASS && objectType != OT_INSTANCE) {
		monasq_printf("Frame.setHandler() - illegal handler object type.\n  eventId:%d type : %d\n", eventId, objectType);
		return false;
	}
	objectType = (SQObjectType)func.GetType();
	if (objectType != OT_CLOSURE) {
		monasq_printf("Frame.setHandler() - illegal handler function type.\n  eventId:%d type : %d\n", eventId, objectType);
		return false;
	}
	
	// コールバック関数オブジェクト作成
 	SquirrelFunction<bool> eventCallback = SquirrelFunction<bool>(calee, func);
	
	// イベントマップへ（上書き）登録
	m_map[eventId] = eventCallback;
	return true;
}

/**
 *	イベントハンドラの取得
 */
SquirrelFunction<bool>*
MonaSqEventHandlerMap::getHandler(int eventId) {
	map<int, SquirrelFunction<bool> >::iterator iter = m_map.find(eventId);
	if (iter != m_map.end()) return &(iter->second);
	return NULL;
}

/**
 *	イベントハンドラの削除
 */
bool
MonaSqEventHandlerMap::removeHandler(int eventId) {
	map<int, SquirrelFunction<bool> >::iterator iter = m_map.find(eventId);
	if (iter != m_map.end()) {
		m_map.erase( iter );
		return true;
	}
	return false;
}

/**
 *	描画イベントディスパッチ
 */
bool
MonaSqEventHandlerMap::dispatchPaint(Graphics* g) {
	bool bRet = false;
	SquirrelFunction<bool>* pSqFunc = getHandler( MONASQ_EVENT_PAINT );
	if (pSqFunc) {
		// baygui._g_ オブジェクトを取得
		SquirrelObject root = SquirrelVM::GetRootTable();
		SquirrelObject bayguiTable = root.GetValue("baygui");
		SquirrelObject sqObj = bayguiTable.GetValue("_g_");
		// イベントハンドラ呼び出し
		g_sqGraphicsProxy.g = g;		// g の中身を書き換える（※ baygui._g_ は g_sqGraphicsProxy の参照オブジェクト）
		bRet = (*pSqFunc)( sqObj );
		g_sqGraphicsProxy.g = NULL;		// g を使用不可に戻す
	}
	return bRet;
}

/**
 *	イベントディスパッチ
 */
bool
MonaSqEventHandlerMap::dispatchEvent(Event *event) {

	switch (event->getType()) {
	case Event::KEY_PRESSED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::KEY_PRESSED );
		if (pSqFunc) {
			KeyEvent* keyEvent = (KeyEvent*)event;
			return (*pSqFunc)(keyEvent->getKeycode(), keyEvent->getModifiers(), bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::KEY_RELEASED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::KEY_RELEASED );
		if (pSqFunc) {
			KeyEvent* keyEvent = (KeyEvent*)event;
			return (*pSqFunc)(keyEvent->getKeycode(), keyEvent->getModifiers(), bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::MOUSE_PRESSED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::MOUSE_PRESSED );
		if (pSqFunc) {
			MouseEvent* mouseEvent = (MouseEvent*)event;
			return (*pSqFunc)(mouseEvent->getX(), mouseEvent->getY(), mouseEvent->getButton(), bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::MOUSE_RELEASED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::MOUSE_RELEASED );
		if (pSqFunc) {
			MouseEvent* mouseEvent = (MouseEvent*)event;
			return (*pSqFunc)(mouseEvent->getX(), mouseEvent->getY(), mouseEvent->getButton(), bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::MOUSE_DRAGGED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::MOUSE_DRAGGED );
		if (pSqFunc) {
			MouseEvent* mouseEvent = (MouseEvent*)event;
			return (*pSqFunc)(mouseEvent->getX(), mouseEvent->getY(), mouseEvent->getButton(), bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::MOUSE_MOVED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::MOUSE_MOVED );
		if (pSqFunc) {
			MouseEvent* mouseEvent = (MouseEvent*)event;
			return (*pSqFunc)(mouseEvent->getX(), mouseEvent->getY(), bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::FOCUS_IN: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::FOCUS_IN );
		if (pSqFunc) {
			return (*pSqFunc)(bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::FOCUS_OUT: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::FOCUS_OUT );
		if (pSqFunc) {
			return (*pSqFunc)(bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::TIMER: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::TIMER );
		if (pSqFunc) {
			return (*pSqFunc)(bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::TEXT_CHANGED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::TEXT_CHANGED );
		if (pSqFunc) {
			return (*pSqFunc)(bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::ITEM_SELECTED: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::ITEM_SELECTED );
		if (pSqFunc) {
			return (*pSqFunc)(bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::BLOCK_INCLEMENT: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::BLOCK_INCLEMENT );
		if (pSqFunc) {
			return (*pSqFunc)(bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::BLOCK_DECLEMENT: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::BLOCK_DECLEMENT );
		if (pSqFunc) {
			return (*pSqFunc)(bayguiSqObject(event->getSource()));
		}
		break; }
	case Event::CUSTOM_EVENT: {
		SquirrelFunction<bool>* pSqFunc = getHandler( Event::CUSTOM_EVENT );
		if (pSqFunc) {
			return (*pSqFunc)(event->header, event->arg1, event->arg2, event->arg3,
				event->from, string(event->str, max(0, min(event->length, 128))).c_str(),
				event->getSource());
		}
		break; }
	}
	return false;
}


//================================================================
// SqImage
//================================================================
/**
 *	SqImage クラス
 *	<br>
 *	baygui::Image のサブクラス
 */
class SqImage : public Image {
public:
	typedef Image SUPER;

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqImage() : Image("") {
		TRACE("SqImage::SqImage() - p:%08x\n", this);
	}
	SqImage(const char* path) : Image(path) {
		TRACE("SqImage::SqImage() - p:%08x\n", this);
	}
	SqImage(int width, int height) : Image(width, height) {
		TRACE("SqImage::SqImage() - p:%08x\n", this);
	}
	virtual ~SqImage() {
		TRACE("SqImage::~SqImage() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqImage* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqImage();
			break;
		case 2:
			newObj = new SqImage( sa.GetString(2) );
			break;
		case 3:
			newObj = new SqImage( sa.GetInt(2), sa.GetInt(3) );
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqImage>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqImage) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------

	// SqImage クラスメソッド
	dword _getHandle() { return SUPER::getHandle(); }
	int _getWidth() { return SUPER::getWidth(); }
	int _getHeight() { return SUPER::getHeight(); }
	dword _getPixel(int x, int y) { return SUPER::getPixel(x, y); }
	void _setPixel(int x, int y, dword color) { SUPER::setPixel(x, y, color); }
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqImage, Image)



//================================================================
// SqButton
//================================================================
/**
 *	SqButton クラス
 *	<br>
 *	baygui::Button のサブクラス
 */
class SqButton : public Button {
	MonaSqEventHandlerMap m_eventHandlerMap;

public:
	typedef Button SUPER;

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqButton() : Button() {
		TRACE("SqButton::SqButton() - p:%08x\n", this);
	}
	SqButton(const char* label) : Button(label) {
		TRACE("SqButton::SqButton(\"%s\") - p:%08x\n",label, this);
	}
	virtual ~SqButton() {
		TRACE("SqButton::~SqButton() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqButton* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqButton();
			break;
		case 2:
			newObj = new SqButton( sa.GetString(2) );
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqButton>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqButton) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	// MonaSq/baygui コンポーネント共通メソッド
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {
 		return m_eventHandlerMap.setHandler(eventId, calee, func);
 	}
	bool removeHandler(int eventId) {
 		return m_eventHandlerMap.removeHandler(eventId);
	}

	// Component クラスメソッド
	void repaint() { SUPER::repaint(); }
	void setBounds(int x, int y, int width, int height) { SUPER::setBounds(x, y, width, height); }
	bool getEnabled() { return SUPER::getEnabled(); }
	bool getFocused() { return SUPER::getFocused(); }
	bool getVisible() { return SUPER::getVisible(); }
	int getX() { return SUPER::getX(); }
	int getY() { return SUPER::getY(); }
	int getWidth() { return SUPER::getWidth(); }
	int getHeight() { return SUPER::getHeight(); }
	dword getBackground() { return SUPER::getBackground(); }
	dword getForeground() { return SUPER::getForeground(); }
	int getFontStyle() { return SUPER::getFontStyle(); }
	void setEnabled(bool enabled) { SUPER::setEnabled(enabled); }
	void setFocused(bool focused) { SUPER::setFocused(focused); }
	void setVisible(bool visible) { SUPER::setVisible(visible); }
	void setLocation(int x, int y) { SUPER::setLocation(x, y); }
	void setBackground(dword backColor) { SUPER::setBackground(backColor); }
	void setForeground(dword foreColor) { SUPER::setForeground(foreColor); }
	void setFontStyle(int style) { SUPER::setFontStyle(style); }

	int update(HSQUIRRELVM v) {
		StackHandler sa(v);
		switch (sa.GetParamCount()) {
		case 1:
			SUPER::update();
			return SQ_OK;
		case 5:
			SUPER::update(sa.GetInt(2), sa.GetInt(3), sa.GetInt(4), sa.GetInt(5));
			return SQ_OK;
		}
		return sq_throwerror(v,_T("Invalid arguments"));
	}

	// Button クラスメソッド
	void _setLabel(const char* label) { SUPER::setLabel(label); }
	const char* getLabel() { return SUPER::getLabel(); }
	bool getPushed() { return SUPER::getPushed(); }


	//-------------------------------------------------------------
	// イベントハンドラ
	//-------------------------------------------------------------
	void processEvent(Event *event) {
//		if (event->getSource() == this && m_eventHandlerMap.dispatchEvent(event)) return;
		if (m_eventHandlerMap.dispatchEvent(event)) return;
		SUPER::processEvent(event);
	}
	void paint(Graphics* g) {
		if (m_eventHandlerMap.dispatchPaint(g)) return;
		SUPER::paint(g);
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqButton, Button)



//================================================================
// SqListBox
//================================================================
/**
 *	SqListBox クラス
 *	<br>
 *	baygui::ListBox のサブクラス
 */
class SqListBox : public ListBox {
	MonaSqEventHandlerMap m_eventHandlerMap;

public:
	typedef ListBox SUPER;

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqListBox() : ListBox() {
		TRACE("SqListBox::SqListBox() - p:%08x\n", this);
	}
	virtual ~SqListBox() {
		TRACE("SqListBox::~SqListBox() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqListBox* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqListBox();
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqListBox>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqListBox) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	// MonaSq/baygui コンポーネント共通メソッド
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {
 		return m_eventHandlerMap.setHandler(eventId, calee, func);
 	}
	bool removeHandler(int eventId) {
 		return m_eventHandlerMap.removeHandler(eventId);
	}

	// Component クラスメソッド
	void repaint() { SUPER::repaint(); }
	void setBounds(int x, int y, int width, int height) { SUPER::setBounds(x, y, width, height); }
	bool getEnabled() { return SUPER::getEnabled(); }
	bool getFocused() { return SUPER::getFocused(); }
	bool getVisible() { return SUPER::getVisible(); }
	int getX() { return SUPER::getX(); }
	int getY() { return SUPER::getY(); }
	int getWidth() { return SUPER::getWidth(); }
	int getHeight() { return SUPER::getHeight(); }
	dword getBackground() { return SUPER::getBackground(); }
	dword getForeground() { return SUPER::getForeground(); }
	int getFontStyle() { return SUPER::getFontStyle(); }
	void setEnabled(bool enabled) { SUPER::setEnabled(enabled); }
	void setFocused(bool focused) { SUPER::setFocused(focused); }
	void setVisible(bool visible) { SUPER::setVisible(visible); }
	void setLocation(int x, int y) { SUPER::setLocation(x, y); }
	void setBackground(dword backColor) { SUPER::setBackground(backColor); }
	void setForeground(dword foreColor) { SUPER::setForeground(foreColor); }
	void setFontStyle(int style) { SUPER::setFontStyle(style); }

	int update(HSQUIRRELVM v) {
		StackHandler sa(v);
		switch (sa.GetParamCount()) {
		case 1:
			SUPER::update();
			return SQ_OK;
		case 5:
			SUPER::update(sa.GetInt(2), sa.GetInt(3), sa.GetInt(4), sa.GetInt(5));
			return SQ_OK;
		}
		return sq_throwerror(v,_T("Invalid arguments"));
	}

	// ListBox クラスメソッド
	int getSelectedIndex() { return SUPER::getSelectedIndex(); }
	char* getSelectedItem() { return SUPER::getSelectedItem(); }
	void select(int index) { return SUPER::select(index); }
	void add(const char* item) { return SUPER::add(item); }
	void remove(int index) { return SUPER::remove(index); }	


	//-------------------------------------------------------------
	// イベントハンドラ
	//-------------------------------------------------------------
	void processEvent(Event *event) {
//		if (event->getSource() == this && m_eventHandlerMap.dispatchEvent(event)) return;
		if (m_eventHandlerMap.dispatchEvent(event)) return;
		SUPER::processEvent(event);
	}
	void paint(Graphics* g) {
		if (m_eventHandlerMap.dispatchPaint(g)) return;
		SUPER::paint(g);
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqListBox, ListBox)



//================================================================
// SqTextField
//================================================================
/**
 *	SqTextField クラス
 *	<br>
 *	baygui::TextField のサブクラス
 */
class SqTextField : public TextField {
	MonaSqEventHandlerMap m_eventHandlerMap;

public:
	typedef TextField SUPER;

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqTextField() : TextField() {
		TRACE("SqTextField::SqTextField() - p:%08x\n", this);
	}
	virtual ~SqTextField() {
		TRACE("SqTextField::~SqTextField() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqTextField* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqTextField();
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqTextField>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqTextField) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	// MonaSq/baygui コンポーネント共通メソッド
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {
 		return m_eventHandlerMap.setHandler(eventId, calee, func);
 	}
	bool removeHandler(int eventId) {
 		return m_eventHandlerMap.removeHandler(eventId);
	}

	// Component クラスメソッド
	void repaint() { SUPER::repaint(); }
	void setBounds(int x, int y, int width, int height) { SUPER::setBounds(x, y, width, height); }
	bool getEnabled() { return SUPER::getEnabled(); }
	bool getFocused() { return SUPER::getFocused(); }
	bool getVisible() { return SUPER::getVisible(); }
	int getX() { return SUPER::getX(); }
	int getY() { return SUPER::getY(); }
	int getWidth() { return SUPER::getWidth(); }
	int getHeight() { return SUPER::getHeight(); }
	dword getBackground() { return SUPER::getBackground(); }
	dword getForeground() { return SUPER::getForeground(); }
	int getFontStyle() { return SUPER::getFontStyle(); }
	void setEnabled(bool enabled) { SUPER::setEnabled(enabled); }
	void setFocused(bool focused) { SUPER::setFocused(focused); }
	void setVisible(bool visible) { SUPER::setVisible(visible); }
	void setLocation(int x, int y) { SUPER::setLocation(x, y); }
	void setBackground(dword backColor) { SUPER::setBackground(backColor); }
	void setForeground(dword foreColor) { SUPER::setForeground(foreColor); }
	void setFontStyle(int style) { SUPER::setFontStyle(style); }

	int update(HSQUIRRELVM v) {
		StackHandler sa(v);
		switch (sa.GetParamCount()) {
		case 1:
			SUPER::update();
			return SQ_OK;
		case 5:
			SUPER::update(sa.GetInt(2), sa.GetInt(3), sa.GetInt(4), sa.GetInt(5));
			return SQ_OK;
		}
		return sq_throwerror(v,_T("Invalid arguments"));
	}

	// TextField クラスメソッド
	void setText(const char* text) { SUPER::setText(text); }
	char* getText() { return SUPER::getText(); }


	//-------------------------------------------------------------
	// イベントハンドラ
	//-------------------------------------------------------------
	void processEvent(Event *event) {
//		if (event->getSource() == this && m_eventHandlerMap.dispatchEvent(event)) return;
		if (m_eventHandlerMap.dispatchEvent(event)) return;
		SUPER::processEvent(event);
	}
	void paint(Graphics* g) {
		if (m_eventHandlerMap.dispatchPaint(g)) return;
		SUPER::paint(g);
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqTextField, TextField)



//================================================================
// SqCheckbox
//================================================================
/**
 *	SqCheckbox クラス
 *	<br>
 *	baygui::Checkbox のサブクラス
 */
class SqCheckbox : public Checkbox {
	MonaSqEventHandlerMap m_eventHandlerMap;

public:
	typedef Checkbox SUPER;

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqCheckbox() : Checkbox("") {
		TRACE("SqCheckbox::SqCheckbox() - p:%08x\n", this);
	}
	SqCheckbox(const char* label) : Checkbox(label) {
		TRACE("SqCheckbox::SqCheckbox() - p:%08x\n", this);
	}
	virtual ~SqCheckbox() {
		TRACE("SqCheckbox::~SqCheckbox() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqCheckbox* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqCheckbox();
			break;
		case 2:
			newObj = new SqCheckbox( sa.GetString(2) );
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqCheckbox>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqCheckbox) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	// MonaSq/baygui コンポーネント共通メソッド
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {
 		return m_eventHandlerMap.setHandler(eventId, calee, func);
 	}
	bool removeHandler(int eventId) {
 		return m_eventHandlerMap.removeHandler(eventId);
	}

	// Component クラスメソッド
	void repaint() { SUPER::repaint(); }
	void setBounds(int x, int y, int width, int height) { SUPER::setBounds(x, y, width, height); }
	bool getEnabled() { return SUPER::getEnabled(); }
	bool getFocused() { return SUPER::getFocused(); }
	bool getVisible() { return SUPER::getVisible(); }
	int getX() { return SUPER::getX(); }
	int getY() { return SUPER::getY(); }
	int getWidth() { return SUPER::getWidth(); }
	int getHeight() { return SUPER::getHeight(); }
	dword getBackground() { return SUPER::getBackground(); }
	dword getForeground() { return SUPER::getForeground(); }
	int getFontStyle() { return SUPER::getFontStyle(); }
	void setEnabled(bool enabled) { SUPER::setEnabled(enabled); }
	void setFocused(bool focused) { SUPER::setFocused(focused); }
	void setVisible(bool visible) { SUPER::setVisible(visible); }
	void setLocation(int x, int y) { SUPER::setLocation(x, y); }
	void setBackground(dword backColor) { SUPER::setBackground(backColor); }
	void setForeground(dword foreColor) { SUPER::setForeground(foreColor); }
	void setFontStyle(int style) { SUPER::setFontStyle(style); }

	int update(HSQUIRRELVM v) {
		StackHandler sa(v);
		switch (sa.GetParamCount()) {
		case 1:
			SUPER::update();
			return SQ_OK;
		case 5:
			SUPER::update(sa.GetInt(2), sa.GetInt(3), sa.GetInt(4), sa.GetInt(5));
			return SQ_OK;
		}
		return sq_throwerror(v,_T("Invalid arguments"));
	}

	// Checkbox クラスメソッド
	void setChecked(bool checked) { SUPER::setChecked(checked); }
	bool getChecked() { return SUPER::getChecked(); }
	SquirrelObject _getCheckboxGroup() { return bayguiSqObject( SUPER::getCheckboxGroup() ); }
	const char* _getLabel() { return SUPER::getLabel(); }
	void setLabel(const char* label) { return SUPER::setLabel(label); }
	void setCheckboxGroup(SquirrelObject so) {
		SQObjectType objectType = (SQObjectType)so.GetType();
		CheckboxGroup* object = (CheckboxGroup*)so.GetInstanceUP(0);
		TRACE("Checkbox.setCheckboxGroup() - p:%08x\n", object);
		if (object == NULL || objectType != OT_INSTANCE) {
			monasq_printf("Checkbox.setCheckboxGroup() - Invalid arguments\n");
			return;
		}
		SUPER::setCheckboxGroup(object);
	}


	//-------------------------------------------------------------
	// イベントハンドラ
	//-------------------------------------------------------------
	void processEvent(Event *event) {
//		if (event->getSource() == this && m_eventHandlerMap.dispatchEvent(event)) return;
		if (m_eventHandlerMap.dispatchEvent(event)) return;
		SUPER::processEvent(event);
	}
	void paint(Graphics* g) {
		if (m_eventHandlerMap.dispatchPaint(g)) return;
		SUPER::paint(g);
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqCheckbox, Checkbox)



//================================================================
// SqCheckboxGroup
//================================================================
/**
 *	SqCheckboxGroup クラス
 *	<br>
 *	baygui::CheckboxGroup のサブクラス
 */
class SqCheckboxGroup : public CheckboxGroup {
	MonaSqEventHandlerMap m_eventHandlerMap;

public:
	typedef CheckboxGroup SUPER;

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqCheckboxGroup() : CheckboxGroup() {
		TRACE("SqCheckboxGroup::SqCheckboxGroup() - p:%08x\n", this);
	}
	virtual ~SqCheckboxGroup() {
		TRACE("SqCheckboxGroup::~SqCheckboxGroup() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqCheckboxGroup* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqCheckboxGroup();
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqCheckboxGroup>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqCheckboxGroup) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	// MonaSq/baygui コンポーネント共通メソッド
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {
 		return m_eventHandlerMap.setHandler(eventId, calee, func);
 	}
	bool removeHandler(int eventId) {
 		return m_eventHandlerMap.removeHandler(eventId);
	}

	// CheckboxGroup クラスメソッド
	void add(SquirrelObject so) {
		SQObjectType objectType = (SQObjectType)so.GetType();
		Checkbox* object = (Checkbox*)so.GetInstanceUP(0);
		TRACE("CheckboxGroup.add() - p:%08x\n", object);
		if (object == NULL || objectType != OT_INSTANCE) {
			monasq_printf("CheckboxGroup.add() - Invalid arguments\n");
			return;
		}
		SUPER::add(object);
	}
	SquirrelObject _getSelectedCheckbox() {
		return bayguiSqObject( SUPER::getSelectedCheckbox() );
	}


	//-------------------------------------------------------------
	// イベントハンドラ
	//-------------------------------------------------------------
	void processEvent(Event *event) {
//		if (event->getSource() == this && m_eventHandlerMap.dispatchEvent(event)) return;
		if (m_eventHandlerMap.dispatchEvent(event)) return;
		SUPER::processEvent(event);
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqCheckboxGroup, CheckboxGroup)



//================================================================
// SqContainer
//================================================================
/**
 *	SqContainer クラス
 *	<br>
 *	baygui::Container のサブクラス
 */
class SqContainer : public Container {
	MonaSqEventHandlerMap m_eventHandlerMap;

public:
	typedef Container SUPER;

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqContainer() : Container() {
		TRACE("SqContainer::SqContainer() - p:%08x\n", this);
	}
	virtual ~SqContainer() {
		TRACE("SqContainer::~SqContainer() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqContainer* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqContainer();
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqContainer>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqContainer) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	// MonaSq/baygui コンポーネント共通メソッド
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {
 		return m_eventHandlerMap.setHandler(eventId, calee, func);
 	}
	bool removeHandler(int eventId) {
 		return m_eventHandlerMap.removeHandler(eventId);
	}

	// Component クラスメソッド
	void repaint() { SUPER::repaint(); }
	void setBounds(int x, int y, int width, int height) { SUPER::setBounds(x, y, width, height); }
	bool getEnabled() { return SUPER::getEnabled(); }
	bool getFocused() { return SUPER::getFocused(); }
	bool getVisible() { return SUPER::getVisible(); }
	int getX() { return SUPER::getX(); }
	int getY() { return SUPER::getY(); }
	int getWidth() { return SUPER::getWidth(); }
	int getHeight() { return SUPER::getHeight(); }
	dword getBackground() { return SUPER::getBackground(); }
	dword getForeground() { return SUPER::getForeground(); }
	int getFontStyle() { return SUPER::getFontStyle(); }
	void setEnabled(bool enabled) { SUPER::setEnabled(enabled); }
	void setFocused(bool focused) { SUPER::setFocused(focused); }
	void setVisible(bool visible) { SUPER::setVisible(visible); }
	void setLocation(int x, int y) { SUPER::setLocation(x, y); }
	void setBackground(dword backColor) { SUPER::setBackground(backColor); }
	void setForeground(dword foreColor) { SUPER::setForeground(foreColor); }
	void setFontStyle(int style) { SUPER::setFontStyle(style); }

	int update(HSQUIRRELVM v) {
		StackHandler sa(v);
		switch (sa.GetParamCount()) {
		case 1:
			SUPER::update();
			return SQ_OK;
		case 5:
			SUPER::update(sa.GetInt(2), sa.GetInt(3), sa.GetInt(4), sa.GetInt(5));
			return SQ_OK;
		}
		return sq_throwerror(v,_T("Invalid arguments"));
	}

	// Container クラスメソッド
	void add(SquirrelObject so) {
		SQObjectType objectType = (SQObjectType)so.GetType();
		Component* component = (Component*)so.GetInstanceUP(0);
		TRACE("Container.add() - p:%08x\n", component);
		if (component == NULL || objectType != OT_INSTANCE) {
			monasq_printf("Container.add() - Invalid arguments\n");
			return;
		}
		SUPER::add( component );
	}
	void remove(SquirrelObject so) {
		SQObjectType objectType = (SQObjectType)so.GetType();
		Component* component = (Component*)so.GetInstanceUP(0);
		TRACE("Container.remove() - p:%08x\n", component);
		if (component == NULL || objectType != OT_INSTANCE) {
			monasq_printf("Container.remove() - Invalid arguments\n");
			return;
		}
		SUPER::remove( component );
	}
	SquirrelObject _getComponent() {
		return bayguiSqObject( SUPER::getComponent() );
	}
	SquirrelObject _getComponentAt(int x, int y) {
		return bayguiSqObject( SUPER::getComponentAt(x, y) );
	}

	//-------------------------------------------------------------
	// イベントハンドラ
	//-------------------------------------------------------------
	void processEvent(Event *event) {
//		if (event->getSource() == this && m_eventHandlerMap.dispatchEvent(event)) return;
		if (m_eventHandlerMap.dispatchEvent(event)) return;
		SUPER::processEvent(event);
	}
	void paint(Graphics* g) {
		if (m_eventHandlerMap.dispatchPaint(g)) return;
		SUPER::paint(g);
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqContainer, Container)



//================================================================
// SqFrame
//================================================================
/**
 *	SqFrame クラス
 *	<br>
 *	baygui::Frame のサブクラス
 */
class SqFrame : public Frame {
	MonaSqEventHandlerMap m_eventHandlerMap;

public:
	typedef Frame SUPER;


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqFrame() : Frame() {
		TRACE("SqFrame::SqFrame() - p:%08x\n", this);
	}
	SqFrame(const char* title) : Frame(title) {
		TRACE("SqFrame::SqFrame() - %s p:%08x\n", title, this);
	}
	virtual ~SqFrame() {
		TRACE("SqFrame::~SqFrame() - p:%08x\n", this);
		g_objectMap.removeObject(this);
	}

	// カスタムコンストラクタ
	static int construct(HSQUIRRELVM v) {
		StackHandler sa(v);
		SqFrame* newObj;
		switch (sa.GetParamCount()) {
		case 1:
			newObj = new SqFrame();
			break;
		case 2:
			newObj = new SqFrame( sa.GetString(2) );
			break;
		default:
			return sq_throwerror(v,_T("Invalid Constructor arguments"));
		}
		g_objectMap.addObject(newObj, SquirrelObject(sa.GetObjectHandle(1)));
		return PostConstruct<SqFrame>(v, newObj, release);
	}

	SQ_DECLARE_RELEASE(SqFrame) // Required when using a custom constructor.


	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	// MonaSq/baygui コンポーネント共通メソッド
 	bool setHandler(int eventId, SquirrelObject calee, SquirrelObject func) {
 		return m_eventHandlerMap.setHandler(eventId, calee, func);
 	}
	bool removeHandler(int eventId) {
 		return m_eventHandlerMap.removeHandler(eventId);
	}

	// Component クラスメソッド
	void repaint() { SUPER::repaint(); }
	void setBounds(int x, int y, int width, int height) { SUPER::setBounds(x, y, width, height); }
	bool getEnabled() { return SUPER::getEnabled(); }
	bool getFocused() { return SUPER::getFocused(); }
	bool getVisible() { return SUPER::getVisible(); }
	int getX() { return SUPER::getX(); }
	int getY() { return SUPER::getY(); }
	int getWidth() { return SUPER::getWidth(); }
	int getHeight() { return SUPER::getHeight(); }
	dword getBackground() { return SUPER::getBackground(); }
	dword getForeground() { return SUPER::getForeground(); }
	int getFontStyle() { return SUPER::getFontStyle(); }
	void setEnabled(bool enabled) { SUPER::setEnabled(enabled); }
	void setFocused(bool focused) { SUPER::setFocused(focused); }
	void setVisible(bool visible) { SUPER::setVisible(visible); }
	void setLocation(int x, int y) { SUPER::setLocation(x, y); }
	void setBackground(dword backColor) { SUPER::setBackground(backColor); }
	void setForeground(dword foreColor) { SUPER::setForeground(foreColor); }
	void setFontStyle(int style) { SUPER::setFontStyle(style); }

	int update(HSQUIRRELVM v) {
		StackHandler sa(v);
		switch (sa.GetParamCount()) {
		case 1:
			SUPER::update();
			return SQ_OK;
		case 5:
			SUPER::update(sa.GetInt(2), sa.GetInt(3), sa.GetInt(4), sa.GetInt(5));
			return SQ_OK;
		}
		return sq_throwerror(v,_T("Invalid arguments"));
	}

	// Container クラスメソッド
	void add(SquirrelObject so) {
		SQObjectType objectType = (SQObjectType)so.GetType();
		Component* component = (Component*)so.GetInstanceUP(0);
		TRACE("Frame.add() - p:%08x\n", component);
		if (component == NULL || objectType != OT_INSTANCE) {
			monasq_printf("Frame.add() - Invalid arguments\n");
			return;
		}
		SUPER::add( component );
	}
	void remove(SquirrelObject so) {
		SQObjectType objectType = (SQObjectType)so.GetType();
		Component* component = (Component*)so.GetInstanceUP(0);
		TRACE("Frame.remove() - p:%08x\n", component);
		if (component == NULL || objectType != OT_INSTANCE) {
			monasq_printf("Frame.remove() - Invalid arguments\n");
			return;
		}
		SUPER::remove( component );
	}
	SquirrelObject _getComponent() {
		TRACE("getComponent : %08x\n", SUPER::getComponent());
		return bayguiSqObject( SUPER::getComponent() );
	}
	SquirrelObject _getComponentAt(int x, int y) {
		TRACE("getComponentAt : %08x\n", SUPER::getComponentAt(x, y));
		return bayguiSqObject( SUPER::getComponentAt(x, y) );
	}

	// Window クラスメソッド
	void setTimer(int duration) { SUPER::setTimer(duration); }
	void stop() { SUPER::stop(); }
	virtual void run();		// ウインドウのメインループ呼び出し

	// Frame クラスメソッド
	const char* getTitle() { return SUPER::getTitle(); }
	void _setTitle(const char* title) { SUPER::setTitle(title); }


	//-------------------------------------------------------------
	// イベントハンドラ
	//-------------------------------------------------------------
	void processEvent(Event *event) {
//		if (event->getSource() == this && m_eventHandlerMap.dispatchEvent(event)) return;
		if (m_eventHandlerMap.dispatchEvent(event)) return;
		SUPER::processEvent(event);
	}
	void paint(Graphics* g) {
		if (m_eventHandlerMap.dispatchPaint(g)) return;
		SUPER::paint(g);
	}
};

// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqFrame, Frame)


/**
 *	ウインドウのメインループ呼び出し
 */
void SqFrame::run() {
	getSqEnvironment()->addScriptWindow(this);
	SUPER::run();
	getSqEnvironment()->removeScriptWindow(this);
}


//================================================================
// グローバル関数
//================================================================

/**
 *	MonaSq/baygui ライブラリの初期化
 */
void monasq_init_baygui_lib(HSQUIRRELVM v)
{
	// root テーブル取得
	SquirrelObject root = SquirrelVM::GetRootTable();

	//--------------------------------------------------------------------------------
	// fake namespace baygui.
	//--------------------------------------------------------------------------------
	// root.baygui テーブルを作成
	SquirrelObject nameSpaceTable = SquirrelVM::CreateTable();
	root.SetValue(_T("baygui"), nameSpaceTable);

	// baygui._g_ 共有 SqGraphicsProxy オブジェクト
	BindVariable(nameSpaceTable, &g_sqGraphicsProxy, _T("_g_"));

	// baygui 下にイベント定数を作成
	for (size_t i=0; i<ARRAY_LENGTH(g_bayguiConstantTable); ++i) {
		BindConstant( nameSpaceTable, g_bayguiConstantTable[i].value, g_bayguiConstantTable[i].name );
	}


	//--------------------------------------------------------------------------------
	// C++/SqGraphicsProxy クラスを Squirrel/GraphicsProxy クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqGraphicsProxy>(_T("GraphicsProxy")).
		func(&SqGraphicsProxy::drawPixel, _T("drawPixel")).
		func(&SqGraphicsProxy::drawImage, _T("drawImage")).
		func(&SqGraphicsProxy::drawLine, _T("drawLine")).
		func(&SqGraphicsProxy::drawRect, _T("drawRect")).
		func(&SqGraphicsProxy::drawCircle, _T("drawCircle")).
		func(&SqGraphicsProxy::drawString, _T("drawString")).
		func(&SqGraphicsProxy::fillCircle, _T("fillCircle")).
		func(&SqGraphicsProxy::fillRect, _T("fillRect")).
		func(&SqGraphicsProxy::translate, _T("translate")).
		func(&SqGraphicsProxy::getFontStyle, _T("getFontStyle")).
		func(&SqGraphicsProxy::setClip, _T("setClip")).
		funcVarArgs(&SqGraphicsProxy::setColor, _T("setColor")).
		func(&SqGraphicsProxy::setFontStyle, _T("setFontStyle"))
	;


	//--------------------------------------------------------------------------------
	// C++/SqImage クラスを Squirrel/Image クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqImage> sqImageClass(_T("Image"));
	
	sqImageClass.staticFuncVarArgs(&SqImage::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqImageClass.
		func(&SqImage::_getHandle, _T("getHandle")).
		func(&SqImage::_getWidth, _T("getWidth")).
		func(&SqImage::_getHeight, _T("getHeight")).
		func(&SqImage::_getPixel, _T("getPixel")).
		func(&SqImage::_setPixel, _T("setPixel"))
	;


	//--------------------------------------------------------------------------------
	// C++/SqButton クラスを Squirrel/Button クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqButton> sqButtonClass(_T("Button"));
	
	sqButtonClass.staticFuncVarArgs(&SqButton::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqButtonClass.
		func(&SqButton::setHandler, _T("setHandler")).
		func(&SqButton::removeHandler, _T("removeHandler")).

		funcVarArgs(&SqButton::update, _T("update")).
		func(&SqButton::repaint, _T("repaint")).
		func(&SqButton::setBounds, _T("setBounds")).
		func(&SqButton::repaint, _T("repaint")).
		func(&SqButton::getEnabled, _T("getEnabled")).
		func(&SqButton::getFocused, _T("getFocused")).
		func(&SqButton::getVisible, _T("getVisible")).
		func(&SqButton::getX, _T("getX")).
		func(&SqButton::getY, _T("getY")).
		func(&SqButton::getWidth, _T("getWidth")).
		func(&SqButton::getHeight, _T("getHeight")).
		func(&SqButton::getBackground, _T("getBackground")).
		func(&SqButton::getForeground, _T("getForeground")).
		func(&SqButton::getFontStyle, _T("getFontStyle")).
		func(&SqButton::setEnabled, _T("setEnabled")).
		func(&SqButton::setFocused, _T("setFocused")).
		func(&SqButton::setVisible, _T("setVisible")).
		func(&SqButton::setBounds, _T("setBounds")).
		func(&SqButton::setLocation, _T("setLocation")).
		func(&SqButton::setBackground, _T("setBackground")).
		func(&SqButton::setForeground, _T("setForeground")).
		func(&SqButton::setFontStyle, _T("setFontStyle")).

		func(&SqButton::getLabel, _T("getLabel")).
		func(&SqButton::_setLabel, _T("setLabel")).
		func(&SqButton::getPushed, _T("getPushed"))
	;


	//--------------------------------------------------------------------------------
	// C++/SqListBox クラスを Squirrel/ListBox クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqListBox> sqListClass(_T("ListBox"));
	
	sqListClass.staticFuncVarArgs(&SqListBox::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqListClass.
		func(&SqListBox::setHandler, _T("setHandler")).
		func(&SqListBox::removeHandler, _T("removeHandler")).

		funcVarArgs(&SqListBox::update, _T("update")).
		func(&SqListBox::repaint, _T("repaint")).
		func(&SqListBox::setBounds, _T("setBounds")).
		func(&SqListBox::repaint, _T("repaint")).
		func(&SqListBox::getEnabled, _T("getEnabled")).
		func(&SqListBox::getFocused, _T("getFocused")).
		func(&SqListBox::getVisible, _T("getVisible")).
		func(&SqListBox::getX, _T("getX")).
		func(&SqListBox::getY, _T("getY")).
		func(&SqListBox::getWidth, _T("getWidth")).
		func(&SqListBox::getHeight, _T("getHeight")).
		func(&SqListBox::getBackground, _T("getBackground")).
		func(&SqListBox::getForeground, _T("getForeground")).
		func(&SqListBox::getFontStyle, _T("getFontStyle")).
		func(&SqListBox::setEnabled, _T("setEnabled")).
		func(&SqListBox::setFocused, _T("setFocused")).
		func(&SqListBox::setVisible, _T("setVisible")).
		func(&SqListBox::setBounds, _T("setBounds")).
		func(&SqListBox::setLocation, _T("setLocation")).
		func(&SqListBox::setBackground, _T("setBackground")).
		func(&SqListBox::setForeground, _T("setForeground")).
		func(&SqListBox::setFontStyle, _T("setFontStyle")).

		func(&SqListBox::getSelectedIndex, _T("getSelectedIndex")).
		func(&SqListBox::getSelectedItem, _T("getSelectedItem")).
		func(&SqListBox::select, _T("select")).
		func(&SqListBox::add, _T("add")).
		func(&SqListBox::remove, _T("remove"))
	;


	//--------------------------------------------------------------------------------
	// C++/SqTextField クラスを Squirrel/TextField クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqTextField> sqTextFieldClass(_T("TextField"));
	
	sqTextFieldClass.staticFuncVarArgs(&SqTextField::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqTextFieldClass.
		func(&SqTextField::setHandler, _T("setHandler")).
		func(&SqTextField::removeHandler, _T("removeHandler")).

		funcVarArgs(&SqTextField::update, _T("update")).
		func(&SqTextField::repaint, _T("repaint")).
		func(&SqTextField::setBounds, _T("setBounds")).
		func(&SqTextField::repaint, _T("repaint")).
		func(&SqTextField::getEnabled, _T("getEnabled")).
		func(&SqTextField::getFocused, _T("getFocused")).
		func(&SqTextField::getVisible, _T("getVisible")).
		func(&SqTextField::getX, _T("getX")).
		func(&SqTextField::getY, _T("getY")).
		func(&SqTextField::getWidth, _T("getWidth")).
		func(&SqTextField::getHeight, _T("getHeight")).
		func(&SqTextField::getBackground, _T("getBackground")).
		func(&SqTextField::getForeground, _T("getForeground")).
		func(&SqTextField::getFontStyle, _T("getFontStyle")).
		func(&SqTextField::setEnabled, _T("setEnabled")).
		func(&SqTextField::setFocused, _T("setFocused")).
		func(&SqTextField::setVisible, _T("setVisible")).
		func(&SqTextField::setBounds, _T("setBounds")).
		func(&SqTextField::setLocation, _T("setLocation")).
		func(&SqTextField::setBackground, _T("setBackground")).
		func(&SqTextField::setForeground, _T("setForeground")).
		func(&SqTextField::setFontStyle, _T("setFontStyle")).

		func(&SqTextField::setText, _T("setText")).
		func(&SqTextField::getText, _T("getText"))
	;


	//--------------------------------------------------------------------------------
	// C++/SqCheckbox クラスを Squirrel/Checkbox クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqCheckbox> sqCheckboxClass(_T("Checkbox"));
	
	sqCheckboxClass.staticFuncVarArgs(&SqCheckbox::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqCheckboxClass.
		func(&SqCheckbox::setHandler, _T("setHandler")).
		func(&SqCheckbox::removeHandler, _T("removeHandler")).

		funcVarArgs(&SqCheckbox::update, _T("update")).
		func(&SqCheckbox::repaint, _T("repaint")).
		func(&SqCheckbox::setBounds, _T("setBounds")).
		func(&SqCheckbox::repaint, _T("repaint")).
		func(&SqCheckbox::getEnabled, _T("getEnabled")).
		func(&SqCheckbox::getFocused, _T("getFocused")).
		func(&SqCheckbox::getVisible, _T("getVisible")).
		func(&SqCheckbox::getX, _T("getX")).
		func(&SqCheckbox::getY, _T("getY")).
		func(&SqCheckbox::getWidth, _T("getWidth")).
		func(&SqCheckbox::getHeight, _T("getHeight")).
		func(&SqCheckbox::getBackground, _T("getBackground")).
		func(&SqCheckbox::getForeground, _T("getForeground")).
		func(&SqCheckbox::getFontStyle, _T("getFontStyle")).
		func(&SqCheckbox::setEnabled, _T("setEnabled")).
		func(&SqCheckbox::setFocused, _T("setFocused")).
		func(&SqCheckbox::setVisible, _T("setVisible")).
		func(&SqCheckbox::setBounds, _T("setBounds")).
		func(&SqCheckbox::setLocation, _T("setLocation")).
		func(&SqCheckbox::setBackground, _T("setBackground")).
		func(&SqCheckbox::setForeground, _T("setForeground")).
		func(&SqCheckbox::setFontStyle, _T("setFontStyle")).

		func(&SqCheckbox::getChecked, _T("getChecked")).
		func(&SqCheckbox::setChecked, _T("setChecked")).
		func(&SqCheckbox::_getLabel, _T("getLabel")).
		func(&SqCheckbox::setLabel, _T("setLabel")).
		func(&SqCheckbox::_getCheckboxGroup, _T("getCheckboxGroup")).
		func(&SqCheckbox::setCheckboxGroup, _T("setCheckboxGroup"))
	;


	//--------------------------------------------------------------------------------
	// C++/SqCheckboxGroup クラスを Squirrel/CheckboxGroup クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqCheckboxGroup> sqCheckboxGroupClass(_T("CheckboxGroup"));
	
	sqCheckboxGroupClass.staticFuncVarArgs(&SqCheckboxGroup::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqCheckboxGroupClass.
		func(&SqCheckboxGroup::setHandler, _T("setHandler")).
		func(&SqCheckboxGroup::removeHandler, _T("removeHandler")).

		func(&SqCheckboxGroup::add, _T("add")).
		func(&SqCheckboxGroup::_getSelectedCheckbox, _T("getSelectedCheckbox"))
	;


	//--------------------------------------------------------------------------------
	// C++/SqContainer クラスを Squirrel/Container クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqContainer> sqContainerClass(_T("Container"));
	
	sqContainerClass.staticFuncVarArgs(&SqContainer::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqContainerClass.
		func(&SqContainer::setHandler, _T("setHandler")).
		func(&SqContainer::removeHandler, _T("removeHandler")).

		funcVarArgs(&SqContainer::update, _T("update")).
		func(&SqContainer::repaint, _T("repaint")).
		func(&SqContainer::setBounds, _T("setBounds")).
		func(&SqContainer::repaint, _T("repaint")).
		func(&SqContainer::getEnabled, _T("getEnabled")).
		func(&SqContainer::getFocused, _T("getFocused")).
		func(&SqContainer::getVisible, _T("getVisible")).
		func(&SqContainer::getX, _T("getX")).
		func(&SqContainer::getY, _T("getY")).
		func(&SqContainer::getWidth, _T("getWidth")).
		func(&SqContainer::getHeight, _T("getHeight")).
		func(&SqContainer::getBackground, _T("getBackground")).
		func(&SqContainer::getForeground, _T("getForeground")).
		func(&SqContainer::getFontStyle, _T("getFontStyle")).
		func(&SqContainer::setEnabled, _T("setEnabled")).
		func(&SqContainer::setFocused, _T("setFocused")).
		func(&SqContainer::setVisible, _T("setVisible")).
		func(&SqContainer::setBounds, _T("setBounds")).
		func(&SqContainer::setLocation, _T("setLocation")).
		func(&SqContainer::setBackground, _T("setBackground")).
		func(&SqContainer::setForeground, _T("setForeground")).
		func(&SqContainer::setFontStyle, _T("setFontStyle")).

		func(&SqContainer::add, _T("add")).
		func(&SqContainer::remove, _T("remove")).
		func(&SqContainer::_getComponent, _T("getComponent")).
		func(&SqContainer::_getComponentAt, _T("getComponentAt"))
	;

	//--------------------------------------------------------------------------------
	// C++/SqFrame クラスを Squirrel/Frame クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqFrame> sqFrameClass(_T("Frame"));
	
	sqFrameClass.staticFuncVarArgs(&SqFrame::construct,_T("constructor"),_T("*"));
		// MUST use this form (or no args) if CustomTestObj will be used as a base class.
		// Using the "*" form will allow a single constructor to be used for all cases.

	sqFrameClass.
		func(&SqFrame::setHandler, _T("setHandler")).
		func(&SqFrame::removeHandler, _T("removeHandler")).

		funcVarArgs(&SqFrame::update, _T("update")).
		func(&SqFrame::repaint, _T("repaint")).
		func(&SqFrame::setBounds, _T("setBounds")).
		func(&SqFrame::repaint, _T("repaint")).
		func(&SqFrame::getEnabled, _T("getEnabled")).
		func(&SqFrame::getFocused, _T("getFocused")).
		func(&SqFrame::getVisible, _T("getVisible")).
		func(&SqFrame::getX, _T("getX")).
		func(&SqFrame::getY, _T("getY")).
		func(&SqFrame::getWidth, _T("getWidth")).
		func(&SqFrame::getHeight, _T("getHeight")).
		func(&SqFrame::getBackground, _T("getBackground")).
		func(&SqFrame::getForeground, _T("getForeground")).
		func(&SqFrame::getFontStyle, _T("getFontStyle")).
		func(&SqFrame::setEnabled, _T("setEnabled")).
		func(&SqFrame::setFocused, _T("setFocused")).
		func(&SqFrame::setVisible, _T("setVisible")).
		func(&SqFrame::setBounds, _T("setBounds")).
		func(&SqFrame::setLocation, _T("setLocation")).
		func(&SqFrame::setBackground, _T("setBackground")).
		func(&SqFrame::setForeground, _T("setForeground")).
		func(&SqFrame::setFontStyle, _T("setFontStyle")).

		func(&SqFrame::add, _T("add")).
		func(&SqFrame::remove, _T("remove")).
		func(&SqFrame::_getComponent, _T("getComponent")).
		func(&SqFrame::_getComponentAt, _T("getComponentAt")).

		func(&SqFrame::run, _T("run")).
		func(&SqFrame::stop, _T("stop")).
		func(&SqFrame::setTimer, _T("setTimer")).

		func(&SqFrame::getTitle, _T("getTitle")).
		func(&SqFrame::_setTitle, _T("setTitle"))
	;
}

