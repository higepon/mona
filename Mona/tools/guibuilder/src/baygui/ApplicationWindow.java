package guibuilder;

import java.awt.*;
import java.awt.event.*;

/**
 アプリケーションウィンドウ。
 メニューやツールバーなどを持っている。
 */
public class ApplicationWindow extends Frame implements WindowListener, ActionListener {
	/** 操作対象 */
	private IParts target = null;
	/** ウィンドウ部品 */
	private PWindow pWindow = null;
	/** ツールバー */
	private ApplicationToolbar applitacionToolbar = null;
	/** プロパティバー */
	private ApplicationPropertybar applitacionPropertybar = null;
	/** ファイル保存クラス */
	private FileMaker fileMaker = null;
	
	/** コンストラクタ */
	public ApplicationWindow() {
		super(IParts.title);
		setSize(640, 480);
		setResizable(false);
		setVisible(true);
		// 透過GIFをアプリケーションアイコンに設定している
		setIconImage(Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("/resources/mona.gif")));
	}
	
	/** 部品生成時に呼ばれる */
	public void create() {
		addWindowListener(this);
		setLayout(null);
		
		// メニューの定義
		MenuBar menuBar = new MenuBar();
		Menu fileMenu = new Menu("ファイル");
		Menu editMenu = new Menu("編集");
		Menu helpMenu = new Menu("ヘルプ");
		fileMenu.add(new MenuItem("新規", new MenuShortcut(KeyEvent.VK_N, false)));
		fileMenu.add(new MenuItem("保存", new MenuShortcut(KeyEvent.VK_S, false)));
		fileMenu.add(new MenuItem("終了", new MenuShortcut(KeyEvent.VK_Q, false)));
		editMenu.add(new MenuItem("削除", new MenuShortcut(KeyEvent.VK_D, false)));
		helpMenu.add(new MenuItem("バージョン", new MenuShortcut(KeyEvent.VK_V, false)));
		fileMenu.addActionListener(this);
		editMenu.addActionListener(this);
		helpMenu.addActionListener(this);
		menuBar.add(fileMenu);
		menuBar.add(editMenu);
		menuBar.add(helpMenu);
		setMenuBar(menuBar);
		
		// ツールバー
		applitacionToolbar = new ApplicationToolbar();
		add(applitacionToolbar);
		applitacionToolbar.create();
		applitacionToolbar.repaint();
		
		// プロパティバー
		applitacionPropertybar = new ApplicationPropertybar();
		add(applitacionPropertybar);
		applitacionPropertybar.create();
		applitacionPropertybar.repaint();
		
		// ファイル保存クラス
		fileMaker = new FileMaker();
	}

	/** ウィンドウ部品を得る */
	public PWindow getPWindow() {
		return pWindow;
	}
	
	/** ツールバーを得る */
	public ApplicationToolbar getApplicationToolbar() {
		return applitacionToolbar;
	}
	
	/** プロパティバーを得る */
	public ApplicationPropertybar getApplicationPropertybar() {
		return applitacionPropertybar;
	}
	
	/** 部品更新 */
	public void update(IParts parts) {
		this.target = parts;
		applitacionPropertybar.update(parts);
	}

	//
	// WindowListener
	//
	public void windowActivated(WindowEvent e) {}
	public void windowClosed(WindowEvent e) {}
	public void windowClosing(WindowEvent e) { System.exit(0); }
	public void windowDeactivated(WindowEvent e) {}
	public void windowDeiconified(WindowEvent e) {}
	public void windowIconified(WindowEvent e) {}
	public void windowOpened(WindowEvent e) {}

	//
	// ActionListener
	//
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("新規")) {
			if (pWindow == null) {
				pWindow = new PWindow();
				add(pWindow);
				pWindow.create();
				pWindow.repaint();
			} else {
				MessageDialog dialog = new MessageDialog(this, "エラー", 
					new String[] {"すでにウィンドウは", "作成されています。"});
				dialog.show();
			}
		} else if (e.getActionCommand().equals("保存") && pWindow != null) {
			if (pWindow != null) {
				fileMaker.writeFile(pWindow);
				MessageDialog dialog = new MessageDialog(this, "保存", 
					new String[] {"保存しました。"});
				dialog.show();
			}
		} else if (e.getActionCommand().equals("終了")) {
			System.exit(0);
		} else if (e.getActionCommand().equals("削除")) {
			if (pWindow != null && target != null) {
				// ウィンドウを削除する
				if (target == pWindow) {
					System.out.println("remove(" + target.getName() + ")");
					Component[] list = pWindow.getComponents();
					for (int i = 0; i < pWindow.getComponentCount(); i++) {
						IParts p = (IParts)list[i];
						p.dispose();
					}
					pWindow.dispose();
					remove(pWindow);
					pWindow = null;
				// 部品を削除する
				} else {
					System.out.println("remove(" + target.getName() + ")");
					target.dispose();
					pWindow.remove((Component)target);
				}
				target = null;
				repaint();
			}
		} else if (e.getActionCommand().equals("バージョン")) {
			MessageDialog dialog = new MessageDialog(this, "バージョン", 
				new String[] {IParts.title, "Copyright (C) 2005, bayside."});
			dialog.show();
		}
	}
}
