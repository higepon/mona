package guibuilder;

import java.awt.*;
import java.awt.event.*;

/**
 ベース部品。
 部品ごとに共通化できるものは全て共通化してコード量を削減する。
 */
public class PBase extends Container implements IParts {
	/** キャプション */
	private String caption   = "";
	/** フォントスタイル */
	private String fontStyle = "";
	/** 各イベントのフラグ */
	private boolean[] eventList = null;
	
	/** コンストラクタ */
	public PBase() {
		eventList = new boolean[eventName.length];
		setLayout(null);
	}
	
	/** 部品生成時に呼ばれる */
	public void create() {
	}
	
	/** 部品削除時に呼ばれる */
	public void dispose() {
	}
	
	/** クラス名を得る */
	public String getClassName() {
		return "";
	}
	
	/** キャプションを得る */
	public String getCaption() {
		return caption;
	}
	
	/** キャプションを設定する */
	public void setCaption(String caption) {
		this.caption = caption;
	}
	
	/** フォントスタイルを得る */
	public String getFontStyle() {
		return fontStyle;
	}
	
	/** フォントスタイルを設定する */
	public void setFontStyle(String fontStyle) {
		this.fontStyle = fontStyle;
	}
	
	/** 背景色（0xffffff）を得る */
	public String getBackcolor() {
		Color  c = getBackground();
		String s = Integer.toHexString
			((c.getRed() << 16 | c.getGreen() << 8 | c.getBlue()) | 0xff000000);
		return s;
	}
	
	/** 背景色を設定する */
	public void setBackcolor(String color) {
		// Javaにはunsignedがないためintだと問題がある
		setBackground(new Color((int)(Long.parseLong(color, 16)) | 0xff000000, true));
	}
	
	/** 前景色（0xffffffff）を得る */
	public String getForecolor() {
		Color  c = getForeground();
		String s = Integer.toHexString
			((c.getRed() << 16 | c.getGreen() << 8 | c.getBlue()) | 0xff000000);
		return s;
	}
	
	/** 前景色を設定する */
	public void setForecolor(String color) {
		// Javaにはunsignedがないためintだと問題がある
		setForeground(new Color((int)(Long.parseLong(color, 16)) | 0xff000000, true));
	}
	
	/** 指定したイベントがサポートされているかどうかを得る */
	public boolean getSupported(int eventType) {
		return true;
	}
	
	/** 指定したイベントのフラグを得る */
	public boolean getEvent(int eventType) {
		return eventList[eventType];
	}
	
	/** 指定したイベントのフラグを設定する */
	public void setEvent(int eventType, boolean flag) {
		eventList[eventType] = flag;
	}
	
	/** コンストラクタに渡す値を得る（デフォルトは空文字）*/
	public String getArgs() {
		return "";
	}
	
	/** マウスイベント */
	public void processMouseEvent(MouseEvent e) {
	}
}
