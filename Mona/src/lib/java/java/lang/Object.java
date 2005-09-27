package java.lang;

public class Object {
	protected void finalize() {
	}

	public Object() {
	}

	public native final Class getClass();
	public native int hashCode();

	public boolean equals(Object obj) {
		return (this == obj);
	}

	public native String toString();
}
