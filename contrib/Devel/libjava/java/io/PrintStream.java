package java.io;

public class PrintStream {
	public PrintStream(Object out) {}
	public native void print(int i);
	public native void print(float f);
	public void print(Object obj) {
		print(obj.toString());
	}
	public native void print(String s);
	public native void println();
	public void println(int i) {
		print(i);
		println();
	}
	public void println(float f) {
		print(f);
		println();
	}
	public void println(Object obj) {
		print(obj);
		println();
	}
	public void println(String s) {
		print(s);
		println();
	}
}
