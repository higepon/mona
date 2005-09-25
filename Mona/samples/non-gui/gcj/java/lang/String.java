package java.lang;

public class String {
	public String() {}
	public String toString() {
		return this;
	}
	public final int length() {
		return count;
	}
	/*private*/ public Object data = null;
	/*private*/ public int boffset = 0, count = 0;
}
