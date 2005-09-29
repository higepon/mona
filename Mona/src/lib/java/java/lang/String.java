package java.lang;

public final class String {
	/*private*/ public char[] data = null;
	/*private*/ public int boffset, count = 0;

	public String() {
	}

	public String(byte[] bytes, int start, int length) {
		data = new char[length + 1];
		for (int i = 0; i < length; i++)
			data[i] = (char)bytes[start + i];
		data[length] = '\0';
		count = length;
	}

	public String(char[] chars, int start, int length) {
		data = new char[length + 1];
		for (int i = 0; i < length; i++)
			data[i] = chars[start + i];
		data[length] = '\0';
		count = length;
	}

	public final String toString() {
		return this;
	}

	public final boolean equals(Object obj) {
		String s = obj.toString();
		if (count != s.count) return false;
		for (int i = 0; i < count; i++)
			if (data[i] != s.data[i]) return false;
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
}
