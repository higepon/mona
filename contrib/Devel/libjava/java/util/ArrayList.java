package java.util;

public class ArrayList {
	protected Object[] data;
	protected int count;
	
	public ArrayList() {
		data = new Object[10];
	}
	
	public int size() { return count; }
	public boolean isEmpty() { return count == 0; }
	public boolean contains(Object elem) { return indexOf(elem) >= 0; }
	
	public int indexOf(Object elem) {
		for (int i = 0; i < count; i++)
			if ((elem == null && data[i] == null) || elem.equals(data[i]))
				return i;
		return -1;
	}
	
	public int lastIndexOf(Object elem) {
		for (int i = count - 1; i >= 0; i--)
			if ((elem == null && data[i] == null) || elem.equals(data[i]))
				return i;
		return -1;
	}
	
	public Object[] toArray() {
		Object[] arr = new Object[count];
		for (int i = 0; i < count; i++)
			arr[i] = data[i];
		return arr;
	}
	
	public Object get(int index) { return data[index]; }
	
	public Object set(int index, Object element) {
		Object old = data[index];
		data[index] = element;
		return old;
	}
	
	public boolean add(Object o) {
		if (data.length == count) {
			Object[] newdata = new Object[count * 2];
			for (int i = 0; i < count; i++)
				newdata[i] = data[i];
			data = newdata;
		}
		data[count++] = o;
		return true;
	}
	
	public Object remove(int index) {
		Object old = data[index];
		count--;
		for (int i = index; i < count; i++)
			data[i] = data[i + 1];
		data[count] = null;
		return old;
	}
	
	public boolean remove(Object o) {
		int index = indexOf(o);
		if (index < -1) return false;
		remove(index);
		return true;
	}
	
	protected void removeRange(int fromIndex, int toIndex) {
		int d = toIndex - fromIndex;
		if (d <= 0) return;
		count -= d;
		for (int i = fromIndex; i < toIndex; i++)
			data[i] = data[i + d];
		for (int i = 0; i < d; i++)
			data[count + i] = null;
	}
}
