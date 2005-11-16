package java.lang;

public final class Integer {
	public static final int MIN_VALUE = 0x80000000;
	public static final int MAX_VALUE = 0x7fffffff;
	private int value;

	public Integer(int value) {
		this.value = value;
	}

	public void setValue(int value) {
		this.value = value;
	}

	public int intValue() {
		return this.value;
	}

	public static int parseInt(String s) {
		int result = 0;
		int mflag  = 1;
		int start  = 0;

		if (s == null || s.length() == 0) return result;

		if (s.charAt(0) == '+'){
			mflag = 1;
			start++;
		} else if(s.charAt(0) == '-'){
			mflag = -1;
			start++;
		}

		for (int i = start; i < s.length(); i++) {
			char c = s.charAt(i);
			if ((c >= '0') && (c <= '9')){
				result = result * 10 + (int)(c - '0');
			}
		}

		return result * mflag;
	}
}
