package guibuilder;

import java.awt.*;
import java.awt.event.*;

public class PButton extends PBase {
	private static int count = 0;
	
	public PButton() {
	}
	
	public void paint(Graphics g) {
		int w = getWidth();
		int h = getHeight();
		
		// àÍìxîwåiêFÇ≈ÉNÉäÉA
		g.setColor(getBackground());
		g.fillRect(0, 0, w, h);
		
		// ògÇï`âÊ
		DrawRaised(g, 0, 0, w, h);
		/*
		g.setColor(COLOR_BLACK);
		g.drawLine(2, 0, w - 3, 0);
		g.drawLine(2, h - 1, w - 3, h - 1);
		g.drawLine(0, 2, 0, h - 3);
		g.drawLine(w - 1, 2, w - 1, h - 3);
		g.drawLine(1, 1, 1, 1);
		g.drawLine(1, h - 2 , 1, h - 2);
		g.drawLine(w - 2 , 1, w - 2, 1);
		g.drawLine(w - 2 , h - 2, w - 2, h - 2);
		
		g.setColor(COLOR_GRAY);
		g.drawLine(2, h - 2, w - 3, h - 2);
		g.drawLine(w - 2, 2, w - 2, h - 3);
		g.drawLine(w - 3 , h - 3, w - 3, h - 3);
		g.setColor(COLOR_WHITE);
		g.drawLine(1, 2, 1, h - 3);
		g.drawLine(2, 1, w - 3, 1);
		*/
		
		// ï∂éö
		FontMetrics metrics = getFontMetrics(getFont());
		int fw = metrics.stringWidth(getCaption());
		int fh = metrics.getAscent() - metrics.getDescent();
		if (isEnabled() == true) {
			g.setColor(getForeground());
		} else {
			g.setColor(Color.gray);
		}
		g.drawString(getCaption(), (w - fw) / 2, (h + fh) / 2);
	}
	
	//
	// IParts
	//
	
	public void create() {
		setBounds(INSETS_LEFT, INSETS_TOP, 80, 20);
		setForeground(DEFAULT_FORECOLOR);
		setBackground(DEFAULT_BACKCOLOR);
		enableEvents(AWTEvent.MOUSE_EVENT_MASK);
		setName("button" + (count + 1));
		setCaption("button" + (count + 1));
		setFontStyle("FONT_PLAIN");
		count++;
	}
	
	public void dispose() {
		this.count = 0;
	}
	
	public String getClassName() {
		return "Button";
	}
	
	public void processMouseEvent(MouseEvent e) {
		if (e.getID() == MouseEvent.MOUSE_PRESSED) {
			System.out.println("PButton: source = " + e.getSource());
			((ApplicationWindow)getParent().getParent()).update(this);
		}
		super.processMouseEvent(e);
	}
}
