package java.awt;

public class Frame extends Window {
	public Frame() {
		super();
		this.border = 1;
		this.insets.top = 22;
		this.insets.bottom = 6;
		this.insets.left = 6;
		this.insets.right = 6;
	}
	
	public Frame(String title) {
		super(title);
		this.border = 1;
		this.insets.top = 22;
		this.insets.bottom = 6;
		this.insets.left = 6;
		this.insets.right = 6;
	}
}
