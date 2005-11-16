package org.monaos;

public class MonAPI {
	public static native int get_server_thread_id(int id);
	public static native int register_to_server(int id, boolean regist);
	public static native int set_mouse_cursor(boolean enabled);
	public static native int process_execute_file_get_tid(String command_line, boolean prompt, Integer process_id, int stdout_id);
	public static native int process_execute_file(String command_line, boolean prompt);
	public static native int change_drive(int drive, boolean prompt);
	public static native int change_directory(String dir);
	public static native int get_current_drive();
	public static native int get_current_directory(byte[] dir);
}
