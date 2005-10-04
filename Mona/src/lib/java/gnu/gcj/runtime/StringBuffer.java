package gnu.gcj.runtime;

public final class StringBuffer {
	private char[] data = null;
	private int count = 0;
	private static final int BUFFER_SIZE = 16;
	
	public StringBuffer() {
		data = new char[BUFFER_SIZE];
		for (int i = 0; i < BUFFER_SIZE; i++) {
			data[i] = '\0';
		}
	}
	
	public StringBuffer(String str) {
		data = new char[BUFFER_SIZE];
		for (int i = 0; i < BUFFER_SIZE; i++) {
			data[i] = '\0';
		}
		append(str);
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
		if (count == BUFFER_SIZE - 1) {
			char[] new_data = new char[data.length + BUFFER_SIZE];
			for (int i = 0; i < new_data.length; i++) {
				new_data[i] = '\0';
			}
			for (int i = 0; i < count; i++) {
				new_data[i] = data[i];
			}
			data = new_data;
			count++;
		}
		data[count++] = c;
		return this;
	}
	
	public StringBuffer append(int i) {
		if (i < 0) {
			append('-');
			i *= -1;
		}
		while(i > 0) {
			append((char)(i % 10 + '0'));
			i /= 10;
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
