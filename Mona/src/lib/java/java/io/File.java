package java.io;

public class File {
	private String name;
	
	private native Object[] list(String pathname);
	
	public File(String pathname) {
		this.name = pathname;
	}
	
	public String getName() {
		return this.name;
	}
	
	public String getPath() {
		return "";
	}
	
	public String[] list() {
		Object[] olist = list(getPath() + getName());
		String[] slist = new String[olist.length];
		for (int i = 0; i < olist.length; i++) {
			slist[i] = olist[i].toString();
		}
		return slist;
	}
}
