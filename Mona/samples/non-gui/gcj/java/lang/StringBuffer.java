package java.lang;

public class StringBuffer {
	public StringBuffer append(int i) {
		return new StringBuffer();
	}
	
	public StringBuffer append(Object o) {
		return append(o.toString());
	}
	
	public StringBuffer append(String s) {
		return new StringBuffer();
	}
}
