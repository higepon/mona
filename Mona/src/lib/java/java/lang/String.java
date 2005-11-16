package java.lang;

public final class String {
	/*private*/ protected char[] data = null;
	/*private*/ protected int boffset, count = 0;

	public String() {
	}

	public String(byte[] bytes) {
		count = bytes.length;
		data = new char[count];
		for (int i = 0; i < count; i++)
			data[i] = (char)bytes[i];
	}

	public String(byte[] bytes, int start, int length) {
		data = new char[length];
		for (int i = 0; i < length; i++)
			data[i] = (char)bytes[start + i];
		count = length;
	}

	public String(char[] chars) {
		count = chars.length;
		data = new char[count];
		for (int i = 0; i < count; i++)
			data[i] = chars[i];
	}

	public String(char[] chars, int start, int length) {
		data = new char[length];
		for (int i = 0; i < length; i++)
			data[i] = chars[start + i];
		count = length;
	}

	public final String toString() {
		return this;
	}

	public byte[] getBytes() {
		byte[] bytes = new byte[length()];
		for (int i = 0; i < length(); i++) {
			bytes[i] = (byte)(charAt(i) & 0xff);
		}
		return bytes;
	}

	public final boolean equals(Object obj) {
		String s = obj.toString();
		if (count != s.count) return false;
		for (int i = 0; i < count; i++)
			if (data[i] != s.data[i])
				return false;
		return true;
	}

	public final int length() {
		return count;
	}
	
	public final char charAt(int index) {
		return data[index];
	}

	public final String substring(int start) {
		return new String(data, start, count - start);
	}

	public final String substring(int start, int length) {
		return new String(data, start, length);
	}
	
	public final int indexOf(char ch) {
		for (int i = 0; i < count; i++)
			if (data[i] == ch) return i;
		return -1;
	}
	
	public final int lastIndexOf(char ch) {
		for (int i = count - 1; i >= 0; i--)
			if (data[i] == ch)
				return i;
		return -1;
	}
	
	public final String[] split(char ch) {
		int len = 1;
		for (int i = 0; i < count; i++)
			if (data[i] == ch) len++;
		String[] sp = new String[len];
		int n = 0, p = 0;
		for (int i = 0; i <= count; i++) {
			if (i == count || data[i] == ch) {
				sp[n++] = substring(p, i - p);
				p = i + 1;
			}
		}
		return sp;
	}
	
	public final boolean startsWith(String s) {
		int len = s.length();
		if (len > count)
			return false;
		for (int i = 0; i < len; i++)
			if (data[i] != s.data[i])
				return false;
		return true;
	}
	
	public final boolean endsWith(String s) {
		int len = s.length();
		if (len > count)
			return false;
		int p = count - len;
		for (int i = 0; i < len; i++)
			if (data[p + i] != s.data[i])
				return false;
		return true;
	}
	
	public final String toLowerCase() {
		char[] chars = new char[count];
		for (int i = 0; i < count; i++) {
			char c = data[i];
			if ((c >= 'A') && (c <= 'Z')) {
				chars[i] = (char)(c + 0x20);
			} else {
				chars[i] = c;
			}
		}
		return new String(chars);
	}
	
	public final String toUpperCase() {
		char[] chars = new char[count];
		for (int i = 0; i < count; i++) {
			char c = data[i];
			if ((c >= 'a') && (c <= 'z')) {
				chars[i] = (char)(c - 0x20);
			} else {
				chars[i] = c;
			}
		}
		return new String(chars);
	}
}
