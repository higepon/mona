public class SimpleCanvas {
	public final static native boolean create(int[] buf, String title, int w, int h);
	public final static native void run();
	public final static native void exit();
	public final static native boolean doEvents();
	public final static native void invalidate();

	public static SimpleCanvas handler;
	public SimpleCanvas() { handler = this; }
	public void onMouseDown(int button, int x, int y) {}
	public void onMouseUp  (int button, int x, int y) {}
	public void onMouseMove(int button, int x, int y) {}
	public void onKeyDown  (int keycode, int modifiers) {}
	public void onKeyUp    (int keycode, int modifiers) {}
	public void onKeyPress (char ch) {}
}
