package guibuilder;

import java.awt.Color;
import java.awt.event.MouseEvent;

/**
 部品の共通インターフェース。
 色関係の定数もここで定義している。
 */
public interface IParts {
	//
	// 定数
	//
	
	public static int INSETS_TOP    = 22;
	public static int INSETS_LEFT   = 6;
	public static int INSETS_RIGHT  = 6;
	public static int INSETS_BOTTOM = 6;

	//
	// イベント
	//
	
	public static int KEY_PRESSED    = 0;
	public static int KEY_RELEASED   = 1;
	public static int MOUSE_PRESSED  = 2;
	public static int MOUSE_RELEASED = 3;
	public static int FOCUS_IN       = 4;
	public static int FOCUS_OUT      = 5;
	public static int TIMER          = 6;
	public static int TEXT_CHANGED   = 7;
	public static int ITEM_SELECTED  = 8;
	public static int CUSTOM_EVENT   = 9;

	/** タイトル */
	public static String title = "GuiBuilder for BayGUI";
	
	/** イベント名 */
	public static String eventName[] = {
		"KEY_PRESSED",
		"KEY_RELEASED",
		"MOUSE_PRESSED",
		"MOUSE_RELEASED",
		"FOCUS_IN",
		"FOCUS_OUT",
		"TIMER",
		"TEXT_CHANGED",
		"ITEM_SELECTED",
		"CUSTOM_EVENT"
	};
	
	/** 各イベントに対応するラベル */
	public static String eventLabel[] = {
		"キー押下",
		"キーリリース",
		"マウスクリック",
		"マウスリリース",
		"フォーカスイン",
		"フォーカスアウト",
		"タイマー",
		"テキスト確定",
		"項目選択",
		"カスタムイベント"
	};

	//
	// デフォルト色
	//
	
	public static Color DEFAULT_TRANSCOLOR = new Color(0x00000000, true);
	public static Color DEFAULT_FORECOLOR  = new Color(0xff000000, true);
	public static Color DEFAULT_BACKCOLOR  = new Color(0xffc0c0c0, true);
	public static Color WINDOWS_LIGHTGRAY  = new Color(0xffd4d0c8, true);

	//
	// システム標準16色に準拠
	//
	
	public static Color COLOR_BLACK     = new Color(0xff000000, true);
	public static Color COLOR_MAROON    = new Color(0xff800000, true);
	public static Color COLOR_GREEN     = new Color(0xff008000, true);
	public static Color COLOR_OLIVE     = new Color(0xff808000, true);
	public static Color COLOR_NAVY      = new Color(0xff000080, true);
	public static Color COLOR_PURPLE    = new Color(0xff800080, true);
	public static Color COLOR_TEAL      = new Color(0xff008080, true);
	public static Color COLOR_LIGHTGRAY = new Color(0xffC0C0C0, true);
	public static Color COLOR_GRAY      = new Color(0xff808080, true);
	public static Color COLOR_RED       = new Color(0xffff0000, true);
	public static Color COLOR_LIME      = new Color(0xff00ff00, true);
	public static Color COLOR_YELLOW    = new Color(0xffffff00, true);
	public static Color COLOR_BLUE      = new Color(0xff0000ff, true);
	public static Color COLOR_MAGENDA   = new Color(0xffff00ff, true);
	public static Color COLOR_CYAN      = new Color(0xff00ffff, true);
	public static Color COLOR_WHITE     = new Color(0xffffffff, true);

	//
	// メソッド
	//
	
	public void create();
	public void dispose();
	public void setName(String name);
	public void setCaption(String caption);
	public void setEnabled(boolean enabled);
	public void setLocation(int x, int y);
	public void setSize(int x, int y);
	public void setFontStyle(String fontStyle);
	public void setBackcolor(String color);
	public void setForecolor(String color);
	public void setEvent(int eventType, boolean flag);
	public void processMouseEvent(MouseEvent e);
	public void repaint();
	
	public String getClassName();
	public String getName();
	public String getCaption();
	public boolean isEnabled();
	public int getX();
	public int getY();
	public int getWidth();
	public int getHeight();
	public String getFontStyle();
	public String getBackcolor();
	public String getForecolor();
	public boolean getSupported(int eventType);
	public boolean getEvent(int eventType);
	public String getArgs();
}
