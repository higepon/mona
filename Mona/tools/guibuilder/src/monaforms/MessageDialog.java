package guibuilder;

import java.awt.*;
import java.awt.event.*;

/**
 メッセージダイアログ。
 ２行までのメッセージを表示可能。
 */
public class MessageDialog extends Dialog implements WindowListener, ActionListener, KeyListener {
	/** 1行目のラベル */
	Label  label1 = new Label("", Label.CENTER);
	/** 2行目のラベル */
	Label  label2 = new Label("", Label.CENTER);
	/** OKボタン */
	Button submit = new Button("OK");
	/** ボタンを載せるパネル */
	Panel  bpanel = new Panel();

	/**
	 コンストラクタ
	 @param parent  親フレーム
	 @param title   タイトル
	 @param message メッセージ（2行まで）
	*/
	public MessageDialog(Frame parent, String title, String[] message) {
		super(parent, title);
		setResizable(false);
		bpanel.add(submit);
		
		// 1行メッセージ
		if (message.length == 1) {
			setSize(200,80);
			setLocation(parent.getX() + 220, parent.getY() + 200);
			label1.setText(message[0]);
			add(label1, BorderLayout.CENTER);
		
		// 2行メッセージ
		} else {
			setSize(200,100);
			setLocation(parent.getX() + 220, parent.getY() + 190);
			label1.setText(message[0]);
			label2.setText(message[1]);
			add(label1, BorderLayout.NORTH);
			add(label2, BorderLayout.CENTER);
		}
		
		add(bpanel, BorderLayout.SOUTH);
		
		// イベント設定
		addWindowListener(this);
		addKeyListener(this);
		submit.addActionListener(this);
		submit.addKeyListener(this);
	}

	//
	// WindowListener
	//
	public void windowActivated(WindowEvent e) {}
	public void windowClosed(WindowEvent e) {}
	public void windowClosing(WindowEvent e) { dispose(); }
	public void windowDeactivated(WindowEvent e) {}
	public void windowDeiconified(WindowEvent e) {}
	public void windowIconified(WindowEvent e) {}
	public void windowOpened(WindowEvent e) {}

	//
	// ActionListener
	//
	public void actionPerformed(ActionEvent e) { dispose(); /* ボタンを押すと閉じる */ }

	//
	// KeyListener
	//
	public void keyPressed(KeyEvent e) {}
	public void keyReleased(KeyEvent e) {}
	public void keyTyped(KeyEvent e) { dispose(); /* 何かキーを押すと閉じる */ }
}
