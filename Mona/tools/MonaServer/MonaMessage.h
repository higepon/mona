#ifndef __MONA_MESSAGE_H__
#define __MONA_MESSAGE_H__

#define MONASERVER_MESSAGE_LIMIT 4096
#define MONASERVER_MESSAGE_MUTEX TEXT("MONASERVER_MESSAGE_MUTEX")
#define MONASERVER_VRAM TEXT("MONASERVER_VRAM")
#define MONASERVER_MESSAGES TEXT("MONASERVER_MESSAGES")

enum
{
	WM_MONA_REFRESH_SCREEN = WM_USER + 0x10,
	WM_MONA_SYSTEM_CALL,
	WM_MONA_MESSAGE
};

extern HWND __mona_server;
extern HANDLE __mona_message_mutex;
extern MessageInfo* __mona_messages;

class MonaMessage
{
public:
	static void Create(MessageInfo* mi, DWORD from, dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL)
	{
		mi->from = from;
		mi->header = header;
		mi->arg1 = arg1;
		mi->arg2 = arg2;
		mi->arg3 = arg3;
		if (str == NULL)
		{
			mi->str[0] = '\0';
		}
		else
		{
			strncpy(mi->str, str, sizeof(mi->str));
		}
	}

	static bool Send(DWORD tid, DWORD from, dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL)
	{
		MessageInfo mi;
		MonaMessage::Create(&mi, from, header, arg1, arg2, arg3, str);
		return Send(tid, &mi);
	}

	static bool Send(DWORD tid, MessageInfo* mi)
	{
		bool ret = false;
		WaitForSingleObject(__mona_message_mutex, INFINITE);
		for (int i = 0; i < MONASERVER_MESSAGE_LIMIT; i++)
		{
			if (__mona_messages[i].from != THREAD_UNKNOWN) continue;

			__mona_messages[i] = *mi;
			if (mi->from == THREAD_UNKNOWN) __mona_messages[i].from = GetCurrentThreadId();
			if (PostThreadMessage(tid, WM_MONA_MESSAGE, (WPARAM)i, 0))
			{
				ret = true;
			}
			else
			{
				__mona_messages[i].from = THREAD_UNKNOWN;
			}
			break;
		}
		ReleaseMutex(__mona_message_mutex);
		return ret;
	}

	static int SystemCall(dword header, dword arg1 = 0, dword arg2 = 0, dword arg3 = 0, const char* str = NULL)
	{
		MessageInfo mi;
		MonaMessage::Create(&mi, GetCurrentThreadId(), header, arg1, arg2, arg3, str);
		return SystemCall(&mi);
	}

	static int SystemCall(MessageInfo* mi)
	{
		int id = -1;
		WaitForSingleObject(__mona_message_mutex, INFINITE);
		for (int i = 0; i < MONASERVER_MESSAGE_LIMIT; i++)
		{
			if (__mona_messages[i].from != THREAD_UNKNOWN) continue;

			__mona_messages[i] = *mi;
			id = i;
			break;
		}
		ReleaseMutex(__mona_message_mutex);
		return id == -1 ? 0 : (int)SendMessage(__mona_server, WM_MONA_SYSTEM_CALL, (WPARAM)id, 0);
	}

	static bool Receive(MessageInfo* mi, int id)
	{
		if (id < 0 || id >= MONASERVER_MESSAGE_LIMIT) return false;

		WaitForSingleObject(__mona_message_mutex, INFINITE);
		if (mi != NULL) *mi = __mona_messages[id];
		__mona_messages[id].from = THREAD_UNKNOWN;
		ReleaseMutex(__mona_message_mutex);
		return true;
	}
};

#endif
