import java.lang.*;
import java.io.*;

public class Main {
	public static void main(String[] args) {
		System.out.println(new Object());
		System.out.println("abc");
		System.out.println("abc.length=" + "abc".length());
	}
}
/*public class Main {
	public static void main(String[] args) {
		char[] chars = new char[] { 'こ', 'ん', 'に', 'ち', 'は', '世', '界' };
		String s = "こんにちは世界";
		for (int i = 0; i < chars.length; i++) {
			char ch1 = chars[i], ch2 = s.charAt(i);
			System.out.print((int)ch1);
			System.out.print(" - ");
			System.out.print((int)ch2);
			System.out.println(ch1 == ch2 ? " : OK" : " : NG");
		}
	}
}*/
