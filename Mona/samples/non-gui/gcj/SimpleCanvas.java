public class SimpleCanvas {
	public static native boolean create(int[] buf, String title, int w, int h);
	public static native boolean doEvents();
	public static native void invalidate();
}
