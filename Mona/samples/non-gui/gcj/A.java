import java.lang.*;
import java.io.*;

public class A {
	public int num1, num2;

	public A(int a, int b) {
		num1 = a;
		num2 = b;
	}

	public int add() {
		return num1 + num2;
	}

	public int sub() {
		return num1 - num2;
	}

	public int mul() {
		return num1 * num2;
	}

	public int div() {
		return num1 / num2;
	}

	public static void main() {
		A a = new A(8, 2);
		System.out.println(a.num1);
		System.out.println(a.num2);
		System.out.println(a.add());
		System.out.println(a.sub());
		System.out.println(a.mul());
		System.out.println(a.div());
	}
}
