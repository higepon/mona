package java.lang;

import java.io.*;

public class System {
	public static java.io.PrintStream out;
	public static native void exit(int status);
	public static native void gc();
}
