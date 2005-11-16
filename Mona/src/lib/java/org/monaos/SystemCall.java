package org.monaos;

public class SystemCall {
	public static native int sleep(int tick);
	public static native int set_timer(int tick);
	public static native int kill_timer(int id);
	public static native int kill_thread(int id);
	public static native int get_cursor();
	public static native int get_io();
	public static native int get_pid();
	public static native int get_tid();
	public static native int clear_screen();
	public static native int set_cursor(int x, int y);
	public static native int set_irq_receiver(int irq);
	public static native int set_ps_dump();
	public static native int read_ps_dump(PsInfo info);
}
