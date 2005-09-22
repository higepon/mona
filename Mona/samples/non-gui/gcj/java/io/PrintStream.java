package java.io;

public class PrintStream {
	public PrintStream(Object out) {}
	public native void print(int i);
	public native void print(String s);
	public native void println();
	public void println(int i) {
		print(i);
		println();
	}
	public void println(String s) {
		print(s);
		println();
	}
}
