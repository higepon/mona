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
	
	public static int INSETS_TOP    = 20;
	public static int INSETS_LEFT   = 2;
	public static int INSETS_RIGHT  = 2;
	public static int INSETS_BOTTOM = 2;

	//
	// イベント
	//
	
	public static int ForeColorChanged = 0;
	public static int BackColorChanged = 1;
	public static int MouseDown        = 2;
	public static int MouseUp          = 3;
	public static int Activated        = 4;
	public static int Deactivate       = 5;
	public static int Tick             = 6;
	public static int TextChanged      = 7;
	
	/** タイトル */
	public static String title = "GuiBuilder for MonaForms";

	/** イベント名 */
	public static String eventName[] = {
		"ForeColorChanged",
		"BackColorChanged",
		"MouseDown",
		"MouseUp",
		"MouseEnter",
		"MouseLeave",
		"Activated",
		"Deactivate",
		"Tick",
		"TextChanged"
	};

	/** 各イベントに対応するラベル */
	public static String eventLabel[] = {
		"キー押下",
		"キーリリース",
		"マウスクリック",
		"マウスリリース",
		"マウス進入",
		"マウス脱出",
		"フォーカスイン",
		"フォーカスアウト",
		"タイマー",
		"テキスト確定"
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
