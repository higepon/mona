package java.lang;

public final class StringBuffer {
	private char[] data;
	private int count = 0;
	private static final int BUFFER_SIZE = 16;

	public StringBuffer() {
		data = new char[BUFFER_SIZE];
	}

	public StringBuffer(String str) {
		count = str.length();
		int size = BUFFER_SIZE;
		while (size < count) size *= 2;
		data = new char[size];
		for (int i = 0; i < count; i++)
			data[i] = str.charAt(i);
	}

	public StringBuffer append(boolean b) {
		if (b) {
			append("true");
		} else {
			append("false");
		}
		return this;
	}

	public StringBuffer append(char c) {
		if (count == data.length) {
			char[] new_data = new char[data.length * 2];
			for (int i = 0; i < count; i++) {
				new_data[i] = data[i];
			}
			data = new_data;
		}
		data[count++] = c;
		return this;
	}

	public StringBuffer append(int i) {
		if (i == 0) {
			append('0');
			return this;
		} else if (i < 0) {
			append('-');
			i = -i;
		}
		char[] digit= new char[20];
		int count = 0;
		while(i > 0) {
			digit[count++] = (char)(i % 10 + '0');
			i /= 10;
		}
		for (i = count - 1; i >= 0; i--) {
			append(digit[i]);
		}
		return this;
	}

	public StringBuffer append(long l) {
		append((int)l);
		return this;
	}

	public StringBuffer append(float f) {
		append((int)f);
		return this;
	}

	public StringBuffer append(double d) {
		append((int)d);
		return this;
	}

	public StringBuffer append(Object obj) {
		append(obj.toString());
		return this;
	}

	public StringBuffer append(String str) {
		for (int i = 0; i < str.length(); i++) {
			append(str.charAt(i));
		}
		return this;
	}

	public StringBuffer append(StringBuffer sb) {
		append(sb.toString());
		return this;
	}

	public String toString() {
		return new String(data, 0, count);
	}
}
