package java.io;

public class FileInputStream {
	private String name;
	private int size;
	private int offset;
	private byte[] data;
	
	private native int get_size(String name);
	private native boolean read_data(String name, byte[] data);
	
	public FileInputStream(File file) {
		this.name = file.getPath() + file.getName();
		this.size = get_size(this.name);
		this.data = new byte[this.size];
		read_data(this.name, this.data);
	}
	
	public FileInputStream(String name) {
		this.name = name;
		this.size = get_size(this.name);
		this.data = new byte[this.size];
		read_data(this.name, this.data);
	}
	
	public int available() {
		return this.size;
	}
	
	public int read() {
		if (this.offset == this.size - 1) {
			return -1;
		} else {
			return this.data[offset++];
		}
	}
	
	public int read(byte[] b) {
		int i = 0;
		for (i = 0; i < b.length; i++) {
			if (this.offset + i == this.size - 1) {
				return i;
			} else {
				b[i] = this.data[this.offset + i];
			}
		}
		this.offset += i;
		return i;
	}
	
	public void close() {
		this.name = "";
		this.size = 0;
		this.data = null;
		this.offset = 0;
	}
}
