package guibuilder;

import java.awt.*;
import java.awt.event.*;

/**
 画像部品
 */
public class PImage extends PBase {
	/** 部品通し番号 */
	private static int count = 0;
	
	/** コンストラクタ */
	public PImage() {
	}
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		
		// 一度背景色でクリア
		g.setColor(COLOR_WHITE);
		g.fillRect(0, 0, w, h);
		
		// 枠を描画
		g.setColor(COLOR_BLACK);
		g.drawRect(0, 0, w - 1, h - 1);
		
		// 文字
		FontMetrics metrics = getFontMetrics(getFont());
		int fw = metrics.stringWidth(getCaption());
		int fh = - metrics.getAscent() + metrics.getDescent();
		if (isEnabled() == true) {
			g.setColor(getForeground());
		} else {
			g.setColor(COLOR_GRAY);
		}
		g.drawString(getCaption(), (w - fw) / 2, (h - fh) / 2);
	}
	
	//
	// IParts
	//
	
	/** 部品作成時に呼ばれる */
	public void create() {
		setBounds(INSETS_LEFT, INSETS_TOP, 80, 80);
		setForeground(DEFAULT_FORECOLOR);
		setBackground(DEFAULT_BACKCOLOR);
		enableEvents(AWTEvent.MOUSE_EVENT_MASK);
		setName("image" + (count + 1));
		setCaption("image" + (count + 1));
		setFontStyle("FONT_PLAIN");
		count++;
	}
	
	/** 部品削除時に呼ばれる */
	public void dispose() {
		this.count = 0;
	}
	
	/** クラス名を得る */
	public String getClassName() {
		return "Image";
	}
	
	/** 指定したイベントがサポートされているかどうかを得る */
	public boolean getSupported(int eventType) {
		return false;
	}
	
	/** コンストラクタに渡す引数を得る */
	public String getArgs() {
		return "\"" + getCaption() + "\"";
	}
	
	/** マウスイベント */
	public void processMouseEvent(MouseEvent e) {
		if (e.getID() == MouseEvent.MOUSE_PRESSED) {
			System.out.println("PImage: source = " + e.getSource());
			((ApplicationWindow)getParent().getParent()).update(this);
		}
		super.processMouseEvent(e);
	}
}
