package org.monaos;

public class Message {
	// unknown thread id
	public static final int THREAD_UNKNOWN = 0xffffffff;
	// server ids
	public static final int ID_MOUSE_SERVER      = 0;
	public static final int ID_KEYBOARD_SERVER   = 1;
	public static final int ID_FILE_SERVER       = 2;
	public static final int ID_GUI_SERVER        = 3;
	public static final int ID_ELF_SERVER        = 4;
	public static final int ID_PROCESS_SERVER    = 5;
	public static final int ID_PE_SERVER         = 6;
	public static final int ID_MONITOR_SERVER    = 7;
	public static final int ID_NUMBER_OF_SERVERS = 8;
	// message ids (0x0000-)
	public static final int MSG_NONE             = 0;
	public static final int MSG_MAP              = 1;
	public static final int MSG_MEMORY_MAP_ID    = 2;
	public static final int MSG_RESULT_OK        = 3;
	public static final int MSG_RESULT_ERROR     = 4;
	public static final int MSG_SERVER_START_OK  = 5;
	public static final int MSG_INTERRUPTED      = 6;
	// message ids (0x0100-)
	public static final int MSG_DISPOSE_HANDLE             = 0x0100;
	public static final int MSG_REGISTER_TO_SERVER         = 0x0110;
	public static final int MSG_UNREGISTER_FROM_SERVER     = 0x0111;
	// message ids (0x0200-)
	public static final int MSG_KEY_REGIST_TO_SERVER       = 0x0200;
	public static final int MSG_KEY_UNREGIST_FROM_SERVER   = 0x0201;
	public static final int MSG_KEY_VIRTUAL_CODE           = 0x0202;
	// message ids (0x0300-)
	public static final int MSG_MOUSE_REGIST_TO_SERVER     = 0x0300;
	public static final int MSG_MOUSE_UNREGIST_FROM_SERVER = 0x0301;
	public static final int MSG_MOUSE_INFO                 = 0x0302;
	public static final int MSG_MOUSE_ENABLE_CURSOR        = 0x0303;
	public static final int MSG_MOUSE_DISABLE_CURSOR       = 0x0304;
	public static final int MSG_MOUSE_GET_CURSOR_POSITION  = 0x0305;
	public static final int MSG_MOUSE_SET_CURSOR_POSITION  = 0x0306;
	// message ids (0x0400-)
	public static final int MSG_FILE_READ_DATA             = 0x0400;
	public static final int MSG_FILE_CHANGE_DRIVE          = 0x0401;
	public static final int MSG_FILE_GET_CURRENT_DRIVE     = 0x0402;
	public static final int MSG_FILE_CHANGE_DIRECTORY      = 0x0403;
	public static final int MSG_FILE_GET_CURRENT_DIRECTORY = 0x0404;
	public static final int MSG_FILE_DECOMPRESS_BZ2        = 0x0405;
	public static final int MSG_FILE_DECOMPRESS_BZ2_FILE   = 0x0406;
	public static final int MSG_FILE_DECOMPRESS_ST5        = 0x0407;
	public static final int MSG_FILE_DECOMPRESS_ST5_FILE   = 0x0408;
	public static final int MSG_FILE_READ_DIRECTORY        = 0x0410;
	public static final int MSG_FILE_OPEN                  = 0x0411;
	public static final int MSG_FILE_CLOSE                 = 0x0412;
	public static final int MSG_FILE_READ                  = 0x0413;
	public static final int MSG_FILE_SEEK                  = 0x0414;
	public static final int MSG_FILE_GET_SIZE              = 0x0415;
	// message ids (0x0500-)
	public static final int MSG_PROCESS_EXECUTE_FILE       = 0x0500;
	public static final int MSG_PROCESS_CREATE_IMAGE       = 0x0501;
	// message ids (0x0800-)
	public static final int MSG_THREAD_KILLED              = 0x0800;
	public static final int MSG_TIMER                      = 0x0801;
	// message ids (0x1000-)
	public static final int MSG_PROCESS_GET_PROCESS_INFO   = 0x1000;
	public static final int MSG_PROCESS_CREATED            = 0x1001;
	public static final int MSG_PROCESS_TERMINATED         = 0x1002;
	public static final int MSG_PROCESS_GET_COMMON_PARAMS  = 0x1003;
	public static final int MSG_PROCESS_GRAB_STDOUT        = 0x10f0; // temporary?
	public static final int MSG_PROCESS_UNGRAB_STDOUT      = 0x10f1; // temporary?
	public static final int MSG_PROCESS_STDOUT_DATA        = 0x10f2; // temporary?
	
	public static native int lookupMainThread(String name);
	public static native int send(int tid, MessageInfo info);
	public static native int send(int tid, int header, int arg1, int arg2, int arg3, byte[] str);
	public static native int receive(MessageInfo info);
	public static native int reply(MessageInfo info, int arg2, int arg3, byte[] str);
	public static native int peek(MessageInfo info, int index);
	public static native boolean exist();
	public static native int sendReceive(MessageInfo info, int tid, int header, int arg1, int arg2, int arg3, byte[] str);
}
