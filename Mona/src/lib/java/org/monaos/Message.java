package org.monaos;

public class Message {
	public static final int ID_MOUSE_SERVER      = 0;
	public static final int ID_KEYBOARD_SERVER   = 1;
	public static final int ID_FILE_SERVER       = 2;
	public static final int ID_GUI_SERVER        = 3;
	public static final int ID_ELF_SERVER        = 4;
	public static final int ID_PROCESS_SERVER    = 5;
	public static final int ID_PE_SERVER         = 6;
	public static final int ID_MONITOR_SERVER    = 7;
	public static final int ID_NUMBER_OF_SERVERS = 8;
	public static final int THREAD_UNKNOWN       = 0xffffffff;
	public static final int MSG_NONE             = 0;
	public static final int MSG_MAP              = 1;
	public static final int MSG_MEMORY_MAP_ID    = 2;
	public static final int MSG_RESULT_OK        = 3;
	public static final int MSG_RESULT_ERROR     = 4;
	public static final int MSG_SERVER_START_OK  = 5;
	public static final int MSG_INTERRUPTED      = 6;
	public static final int MSG_THREAD_KILLED    = 0x800;
	public static final int MSG_TIMER            = 0x801;
	
	public static native int register_to_server(int id, boolean regist);
	public static native int get_server_thread_id(int id);
	public static native int send(int tid, MessageInfo info);
	public static native int send(int tid, int header, int arg1, int arg2, int arg3);
	public static native int receive(MessageInfo info);
	public static native int reply(MessageInfo info, int arg2, int arg3);
	public static native int peek(MessageInfo info, int index);
	public static native boolean exist();
	public static native int send_receive(MessageInfo dst, int tid, int header, int arg1, int arg2, int arg3);
}
