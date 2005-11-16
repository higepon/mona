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
org::monaos::Message::lookupMainThread (jstring name)
{
	char* cname = get_string(name);
	int result = ::MonAPI::Message::lookupMainThread(cname);
	delete(cname);
	return result;
}


jint
org::monaos::Message::send (jint id, ::org::monaos::MessageInfo *info)
{
#ifdef MONA
	::MessageInfo minfo;
	minfo.header = info->header;
	minfo.arg1 = info->arg1;
	minfo.arg2 = info->arg2;
	minfo.arg3 = info->arg3;
	minfo.from = info->from;
	byte* bstr = (byte*)elements(info->str);
	memcpy(minfo.str, bstr, 128);
	return ::MonAPI::Message::send(id, &minfo);
#else
	return 0;
#endif
}


jint
org::monaos::Message::send (jint id, jint header, jint arg1, jint arg2, jint arg3, jbyteArray str)
{
#ifdef MONA
	char* bstr = NULL;
	if (str != NULL) bstr = (char*)elements(str);
	return ::MonAPI::Message::send(id, header, arg1, arg2, arg3, bstr);
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
	byte* bstr = (byte*)elements(info->str);
	memcpy(bstr, minfo.str, 128);
	return result;
#else
	return 0;
#endif
}


jint
org::monaos::Message::reply (::org::monaos::MessageInfo *info, jint arg2, jint arg3, jbyteArray str)
{
#ifdef MONA
	char* bstr = NULL;
	if (str != NULL) bstr = (char*)elements(info->str);
	return ::MonAPI::Message::send(info->from, MSG_RESULT_OK, info->header, arg2, arg3, bstr);
#else
	return 0;
#endif
}


jint
org::monaos::Message::peek (::org::monaos::MessageInfo *info, jint index)
{
#ifdef MONA
	::MessageInfo minfo;
	int result = ::MonAPI::Message::peek(&minfo, index, 0);
	info->header = minfo.header;
	info->arg1 = minfo.arg1;
	info->arg2 = minfo.arg2;
	info->arg3 = minfo.arg3;
	info->from = minfo.from;
	byte* bstr = (byte*)elements(info->str);
	memcpy(bstr, minfo.str, 128);
	return result;
#else
	return 0;
#endif
}


jboolean
org::monaos::Message::exist ()
{
#ifdef MONA
	return ::MonAPI::Message::exist();
#else
	return false;
#endif
}


jint
org::monaos::Message::sendReceive (::org::monaos::MessageInfo *info, jint header, jint tid, jint arg1, jint arg2, jint arg3, jbyteArray str)
{
#ifdef MONA
	if (info != NULL) {
		::MessageInfo minfo;
		char* bstr = NULL;
		if (str != NULL) bstr = (char*)elements(info->str);
		int result = ::MonAPI::Message::sendReceive(&minfo, header, tid, arg1, arg2, arg3, bstr);
		info->header = minfo.header;
		info->arg1 = minfo.arg1;
		info->arg2 = minfo.arg2;
		info->arg3 = minfo.arg3;
		info->from = minfo.from;
		bstr = (char*)elements(info->str);
		memcpy(bstr, minfo.str, 128);
		return result;
	} else {
		char* bstr = NULL;
		if (str != NULL) bstr = (char*)elements(info->str);
		return ::MonAPI::Message::sendReceive(NULL, header, tid, arg1, arg2, arg3, bstr);
	}
#else
	return 0;
#endif
}


