package guibuilder;

import java.awt.*;

/**
 画像つきボタン。
 ボタンを継承しているので押すと引っ込む。
 */
public class ImageButton extends Button {
	/** 画像 */
	private Image image = null;
	
	/**
	 コンストラクタ。
	 @param name '/resources/' + 'name' + '.gif' という名前のファイルが開かれる。
	 */
	public ImageButton(String name) {
		setName(name);
		image = Toolkit.getDefaultToolkit().getImage(this.getClass().getResource("/resources/" + name + ".gif"));
	}
	
	/** 描画ハンドラ */
	public void paint(Graphics g) {
		g.drawImage(image, 2, 2, this);
	}
}
