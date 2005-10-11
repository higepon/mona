package org.monaos;

public class IO {
	public static native byte inp8(int port);
	public static native void outp8(int port, byte value);
	public static native short inp16(int port);
	public static native void outp16(int port, short value);
	public static native int inp32(int port);
	public static native void outp32(int port, int value);
	public static native void setIRQ(int irq, boolean enabled, boolean auto_ir2);
}
