// This file was created by `gcjh -stubs'. -*- c++ -*-
//
// This file is intended to give you a head start on implementing native
// methods using CNI.
// Be aware: running `gcjh -stubs ' once more for this class may
// overwrite any edits you have made to this file.

#include <org/monaos/MessageInfo.h>
#include <org/monaos/Message.h>
#include <gcj/cni.h>
#ifdef MONA
#include <monapi.h>
#include <monapi/messages.h>
#endif

jint
org::monaos::Message::register_to_server (jint id, jboolean regist)
{
#ifdef MONA
	return monapi_register_to_server(id, regist);
#else
	return 0;
#endif
}


jint
org::monaos::Message::get_server_thread_id (jint id)
{
#ifdef MONA
	return monapi_get_server_thread_id(id);
#else
	return 0;
#endif
}


jint
org::monaos::Message::send (jint id, ::org::monaos::MessageInfo *info)
{
#ifdef MONA
	::MessageInfo minfo;
	minfo.header = info->header;
	minfo.header = info->arg1;
	minfo.header = info->arg2;
	minfo.header = info->arg3;
	minfo.header = info->from;
	return MonAPI::Message::send(id, &minfo);
#else
	return 0;
#endif
}


jint
org::monaos::Message::send (jint id, jint header, jint arg1, jint arg2, jint arg3)
{
#ifdef MONA
	return MonAPI::Message::send(id, header, arg1, arg2, arg3);
#else
	return 0;
#endif
}


jint
org::monaos::Message::receive (::org::monaos::MessageInfo *info)
{
#ifdef MONA
	::MessageInfo minfo;
	int result = MonAPI::Message::receive(&minfo);
	info->header = minfo.header;
	info->arg1 = minfo.arg1;
	info->arg2 = minfo.arg2;
	info->arg3 = minfo.arg3;
	info->from = minfo.from;
	return result;
#else
	return 0;
#endif
}


jint
org::monaos::Message::reply (::org::monaos::MessageInfo *info, jint arg2, jint arg3)
{
#ifdef MONA
	::MessageInfo minfo;
	int result = MonAPI::Message::reply(&minfo, arg2, arg3);
	info->header = minfo.header;
	info->arg1 = minfo.arg1;
	info->arg2 = minfo.arg2;
	info->arg3 = minfo.arg3;
	info->from = minfo.from;
	return result;
#else
	return 0;
#endif
}


jint
org::monaos::Message::peek (::org::monaos::MessageInfo *info, jint index)
{
#ifdef MONA
	::MessageInfo minfo;
	int result = MonAPI::Message::peek(&minfo, index, 0);
	info->header = minfo.header;
	info->arg1 = minfo.arg1;
	info->arg2 = minfo.arg2;
	info->arg3 = minfo.arg3;
	info->from = minfo.from;
	return result;
#else
	return 0;
#endif
}


jboolean
org::monaos::Message::exist ()
{
#ifdef MONA
	return MonAPI::Message::exist();
#else
	return false;
#endif
}


jint
org::monaos::Message::send_receive (::org::monaos::MessageInfo *info, jint header, jint tid, jint arg1, jint arg2, jint arg3)
{
#ifdef MONA
	::MessageInfo minfo;
	minfo.header = info->header;
	minfo.header = info->arg1;
	minfo.header = info->arg2;
	minfo.header = info->arg3;
	minfo.header = info->from;
	return MonAPI::Message::sendReceive(&minfo, header, tid, arg1, arg2, arg3);
#else
	return 0;
#endif
}


