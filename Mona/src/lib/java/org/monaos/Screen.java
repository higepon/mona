package org.monaos;

public class Screen {
	public static native int getWidth();
	public static native int getHeight();
	public static native int getBpp();
	public static native int getPixel(int x, int y);
	public static native void putPixel(int x, int y, int color);
	public static native void bitblt(int x, int y, int width, int height, int image[]);
}
