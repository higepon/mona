package guibuilder;

import java.awt.*;
import java.awt.event.*;

/**
 ラベル部品
 */
public class PLabel extends PBase {
	/** 部品通し番号 */
	private static int count = 0;
	
	/** コンストラクタ */
	public PLabel() {
	}
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		
		// 一度背景色でクリア
		g.setColor(getBackground());
		g.fillRect(0, 0, w, h);
		
		// 文字
		FontMetrics metrics = getFontMetrics(getFont());
		int fw = metrics.stringWidth(getCaption());
		int fh = metrics.getAscent() - metrics.getDescent();
		if (isEnabled() == true) {
			g.setColor(getForeground());
		} else {
			g.setColor(COLOR_GRAY);
		}
		g.drawString(getCaption(), (w - fw) / 2, (h + fh) / 2);
	}
	
	//
	// IParts
	//
	
	/** 部品生成時に呼ばれる */
	public void create() {
		setBounds(INSETS_LEFT, INSETS_TOP, 80, 20);
		setForeground(DEFAULT_FORECOLOR);
		setBackground(DEFAULT_BACKCOLOR);
		enableEvents(AWTEvent.MOUSE_EVENT_MASK);
		setName("label" + (count + 1));
		setCaption("label" + (count + 1));
		setFontStyle("FONT_PLAIN");
		count++;
	}
	
	/** 部品削除時に呼ばれる */
	public void dispose() {
		this.count = 0;
	}
	
	/** クラス名を得る */
	public String getClassName() {
		return "Label";
	}
	
	/** コンストラクタに渡す引数を得る */
	public String getArgs() {
		return "\"" + getCaption() + "\"";
	}
	
	/** マウスイベント */
	public void processMouseEvent(MouseEvent e) {
		if (e.getID() == MouseEvent.MOUSE_PRESSED) {
			System.out.println("PLabel: source = " + e.getSource());
			((ApplicationWindow)getParent().getParent()).update(this);
		}
		super.processMouseEvent(e);
	}
}
