package org.monaos;

public class KeyInfo {
	public int keycode;
	public int modifiers;
	public int charcode;

	public KeyInfo() {
	}

	public KeyInfo(int keycode, int modifiers, int charcode) {
		this.keycode = keycode;
		this.modifiers = modifiers;
		this.charcode = charcode;
	}
}
