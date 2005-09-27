package java.lang;

public class String {
	public String() {
	}

	public String(byte[] bytes, int start, int length) {
		data = new char[length + 1];
		for (int i = 0; i < length; i++)
			data[i] = (char)bytes[start + i];
		data[length] = '\0';
	}

	public String toString() {
		return this;
	}

	public final int length() {
		return count;
	}

	/*private*/ public static int boffset;
	/*private*/ public char[] data = null;
	/*private*/ public int count = 0;
}
