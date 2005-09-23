import java.lang.*;
import java.io.*;

public class A {
	public static void main() {
		System.out.println(new Object());
		System.out.println(new System());
		System.out.println(new PrintStream(null));
		System.out.println(new A());
		int[] a = new int[] { 5, 3, 1, 6 };
		for (int i = 0; i <= a.length; i++) {
			System.out.println(a[i]);
		}
	}
}
