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

	public static native int getServerThreadId(int id);
//	public static native int send(int tid, MessageInfo info);
	public static native int send(int tid, int header, int arg1, int arg2, int arg3);
//	public static native int receive(MessageInfo info);
//	public static native int reply(MessageInfo info, int arg2, int arg3);
//	public static native int peek(MessageInfo info, int index);
	public static native boolean exist();
//	public static native int sendReceive(MessageInfo dst, int tid, int header, int arg1, int arg2, int arg3);
}
