package guibuilder;

import java.awt.*;
import java.awt.event.*;

/**
 アプリケーションツールバー。
 ボタンが押されるとウィンドウに部品が追加される。
 */
public class ApplicationToolbar extends Panel implements ActionListener {
	/** Button */
	private ImageButton b1 = null;
	/** Image */
	private ImageButton b2 = null;
	/** Label */
	private ImageButton b3 = null;
	/** ListBox */
	private ImageButton b4 = null;
	/** TextField */
	private ImageButton b5 = null;
	
	/** コンストラクタ */
	public ApplicationToolbar() {
	}
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		// 立体的に見せるための線
		g.setColor(IParts.COLOR_GRAY);
		g.drawLine(0, 0, getWidth() - 1, 0);
		g.setColor(IParts.COLOR_WHITE);
		g.drawLine(0, 1, getWidth() - 1, 1);
		g.setColor(IParts.COLOR_BLACK);
		g.drawLine(0, getHeight() - 1, getWidth() - 1, getHeight() - 1);
		
		paintComponents(g);
	}
	
	/** 部品生成時に呼ばれる */
	public void create() {
		setBounds(getParent().getInsets().left, getParent().getInsets().top, 
			640 - getParent().getInsets().right - getParent().getInsets().left, 29);
		setForeground(IParts.COLOR_BLACK);
		setBackground(IParts.WINDOWS_LIGHTGRAY);
		setLayout(null);
		
		b1 = new ImageButton("button");
		b2 = new ImageButton("image");
		b3 = new ImageButton("label");
		b4 = new ImageButton("listbox");
		b5 = new ImageButton("textfield");
		
		b1.setBounds(  1, 2, 26, 26);
		b2.setBounds( 27, 2, 26, 26);
		b3.setBounds( 53, 2, 26, 26);
		b4.setBounds( 79, 2, 26, 26);
		b5.setBounds(105, 2, 26, 26);
		
		b1.addActionListener(this);
		b2.addActionListener(this);
		b3.addActionListener(this);
		b4.addActionListener(this);
		b5.addActionListener(this);
		
		add(b1);
		add(b2);
		add(b3);
		add(b4);
		add(b5);
	}

	//
	// ActionListener
	//
	public void actionPerformed(ActionEvent e) {
		// アプリケーションウィンドウを得る
		PWindow pWindow = ((ApplicationWindow)getParent()).getPWindow();
		if (pWindow == null) return;
		if (e.getSource() == b1) {
			// ボタン部品
			PButton p = new PButton();
			pWindow.add(p, 0);
			p.create();
		} else if (e.getSource() == b2) {
			// イメージ部品
			PImage p = new PImage();
			pWindow.add(p, 0);
			p.create();
		} else if (e.getSource() == b3) {
			// ラベル部品
			PLabel p = new PLabel();
			pWindow.add(p, 0);
			p.create();
		} else if (e.getSource() == b4) {
			// リスト部品
			PListBox p = new PListBox();
			pWindow.add(p, 0);
			p.create();
		} else if (e.getSource() == b5) {
			// テキスト部品
			PTextField p = new PTextField();
			pWindow.add(p, 0);
			p.create();
		}
	}
}
