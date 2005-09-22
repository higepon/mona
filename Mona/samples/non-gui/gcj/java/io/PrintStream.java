package java.io;

public class PrintStream {
	public PrintStream(Object out) {}
	public native void print(int i);
	public native void println();
	public void println(int i) {
		print(i);
		println();
	}
}
