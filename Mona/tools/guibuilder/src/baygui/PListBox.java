package guibuilder;

import java.awt.*;
import java.awt.event.*;

/**
 リスト部品
 */
public class PListBox extends PBase {
	/** 部品通し番号 */
	private static int count = 0;
	
	/** コンストラクタ */
	public PListBox() {
	}
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		
		// 一度背景色でクリア
		g.setColor(COLOR_WHITE);
		g.fillRect(0, 0, w, h);
		
		// 枠を描画
		if (isEnabled() == true) {
			g.setColor(new Color(0xff0080ff, true));
		} else {
			g.setColor(getBackground());
		}
		g.drawRect(0, 0, w - 1, h - 1);
		g.setColor(COLOR_BLACK);
		g.drawRect(1, 1, w - 3, h - 3);
		
		// 選択枠
		if (isEnabled() == true) {
			g.setColor(new Color(0xff0080ff, true));
			g.fillRect(3, 3, w - 6, 12);
		}
		
		// 文字
		FontMetrics metrics = getFontMetrics(getFont());
		int fw = metrics.stringWidth(getCaption());
		int fh = metrics.getAscent() - metrics.getDescent();
		if (isEnabled() == true) {
			g.setColor(COLOR_WHITE);
		} else {
			g.setColor(COLOR_BLACK);
		}
		g.drawString(getCaption(), 3, 3 + fh);
	}
	
	//
	// IParts
	//
	
	/** 部品生成時に呼ばれる */
	public void create() {
		setBounds(INSETS_LEFT, INSETS_TOP, 100, 100);
		setForeground(DEFAULT_FORECOLOR);
		setBackground(DEFAULT_BACKCOLOR);
		enableEvents(AWTEvent.MOUSE_EVENT_MASK);
		setName("list" + (count + 1));
		setCaption("list" + (count + 1));
		setFontStyle("FONT_PLAIN");
		count++;
	}
	
	/** 部品削除時に呼ばれる */
	public void dispose() {
		this.count = 0;
	}
	
	/** クラス名を得る */
	public String getClassName() {
		return "ListBox";
	}
	
	/** マウスイベント */
	public void processMouseEvent(MouseEvent e) {
		if (e.getID() == MouseEvent.MOUSE_PRESSED) {
			System.out.println("PListBox: source = " + e.getSource());
			((ApplicationWindow)getParent().getParent()).update(this);
		}
		super.processMouseEvent(e);
	}
}
