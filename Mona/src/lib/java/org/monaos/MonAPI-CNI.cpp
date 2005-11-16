// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <org/monaos/MonAPI.h>
#include <gcj/cni.h>
#include <java/lang/Integer.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>

static char* get_string(jstring s) {
	int len = s->length();
	char* cstr = new char[len + 1];
	jchar* ss = JvGetStringChars(s);
	for (int i = 0; i < len; i++)
		cstr[i] = (char)ss[i];
	cstr[len] = '\0';
	return cstr;
}
#endif

jint
org::monaos::MonAPI::get_server_thread_id (jint id)
{
#ifdef MONA
	return monapi_get_server_thread_id(id);
#else
	return 0;
#endif
}


jint
org::monaos::MonAPI::register_to_server (jint id, jboolean regist)
{
#ifdef MONA
	return monapi_register_to_server(id, regist);
#else
	return 0;
#endif
}


jint
org::monaos::MonAPI::set_mouse_cursor (jboolean enabled)
{
#ifdef MONA
	return monapi_call_mouse_set_cursor(enabled);
#else
	return 0;
#endif
}


jint
org::monaos::MonAPI::process_execute_file_get_tid (jstring command_line, jboolean prompt, ::java::lang::Integer* process_id, jint stdout_id)
{
	char* s_command_line = get_string(command_line);
	dword tid;
	int result = monapi_call_process_execute_file_get_tid(s_command_line, prompt, &tid, stdout_id);
	process_id->setValue(tid);
	delete [] s_command_line;
	return result;
}


jint
org::monaos::MonAPI::process_execute_file (jstring command_line, jboolean prompt)
{
	char* s_command_line = get_string(command_line);
	int result = monapi_call_process_execute_file(s_command_line, prompt);
	delete [] s_command_line;
	return result;
}


jint
org::monaos::MonAPI::change_drive (jint drive, jboolean prompt)
{
#ifdef MONA
	return monapi_call_change_drive(drive, prompt);
#else
	return 0;
#endif
}


jint
org::monaos::MonAPI::change_directory (jstring dir)
{
#ifdef MONA
	char *s_dir = get_string(dir);
	int result = monapi_call_change_directory(s_dir);
	delete [] s_dir;
	return result;
#else
	return 0;
#endif
}


jint
org::monaos::MonAPI::get_current_drive ()
{
#ifdef MONA
	return monapi_call_get_current_drive();
#else
	return 0;
#endif
}


jint
org::monaos::MonAPI::get_current_directory (jbyteArray dir)
{
#ifdef MONA
	dword tid = monapi_get_server_thread_id(ID_FILE_SERVER);

	MessageInfo msg;
	if (::MonAPI::Message::sendReceive(&msg, tid, MSG_FILE_GET_CURRENT_DIRECTORY) != 0) {
		return MONA_FAILURE;
	}

	int result = msg.arg2;
	if (result == MONA_SUCCESS) {
		char* b_dir = (char*)elements(dir);
		strncpy(b_dir, msg.str, 128);
	}

	return result;
#else
	return 0;
#endif
}


