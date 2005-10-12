package java.lang.Vector;

public final class Vector {
	private Object[] data;
	private int count = 0;
	private static final int BUFFER_SIZE = 16;

	public Vector() {
		data = new Object[BUFFER_SIZE];
	}

	public boolean add(Object obj) {
		if (count == data.length) {
			Object[] new_data = new Object[data.length * 2];
			for (int i = 0; i < count; i++) {
				new_data[i] = data[i];
			}
			data = new_data;
		}
		data[count++] = obj;
		return true;
	}

	public int capacity() {
		return count;
	}

	public void clear() {
		for (int i = 0; i < data.length; i++) {
			data[i] = null;
		}
		data = null;
		data = new Object[BUFFER_SIZE];
	}

	public boolean contains(Object obj) {
		for (int i = 0; i < data.length; i++) {
			if (data[i].equals(obj)) return true;
		}
		return false;
	}

	public Object get(int index) {
		if (index < 0 || index >= count) {
			return null;
		} else {
			return data[index];
		}
	}

	public int indexOf(Object obj) {
		for (int i = 0; i < data.length; i++) {
			if (data[i].equals(obj)) return i;
		}
		return -1;
	}

	public boolean isEmpty() {
		return (count > 0);
	}
}
