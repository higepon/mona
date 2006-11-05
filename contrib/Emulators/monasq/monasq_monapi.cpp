/**
 *	MonaSq/monapi ライブラリ定義
 */
/*
 *	Copyright (c) 2006 okayu punch
 *
 *	Permission is hereby granted, free of charge, to any person
 *	obtaining a copy of this software and associated documentation
 *	files (the "Software"), to deal in the Software without restriction,
 *	including without limitation the rights to use, copy, modify, merge,
 *	publish, distribute, sublicense, and/or sell copies of the Software,
 *	and to permit persons to whom the Software is furnished to do so,
 *	subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be
 *	included in all copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 *	THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "monasq.h"
#include "sqplus.h"
#include <monapi/io.h>

using namespace SqPlus;

typedef int				INT32;
typedef short			INT16;
typedef char			INT8;
typedef unsigned int	UINT32;
typedef unsigned short	UINT16;
typedef unsigned char	UINT8;
typedef float			FLOAT32;
typedef double			FLOAT64;


//================================================================
// 定数
//================================================================

// monapi 定数定義
struct MonapiConstantData {
	int value;
	const char* name;
} g_monapiConstantTable[] = {
	{ VK_SPACE,			"VK_SPACE" },
	{ VK_BACKSPACE,		"VK_BACKSPACE" },
	{ VK_ENTER,			"VK_ENTER" },
	{ VK_PERIOD,		"VK_PERIOD" },
	{ VK_SLASH,			"VK_SLASH" },
	{ VK_A,				"VK_A" },
	{ VK_B,				"VK_B" },
	{ VK_C,				"VK_C" },
	{ VK_D,				"VK_D" },
	{ VK_E,				"VK_E" },
	{ VK_F,				"VK_F" },
	{ VK_G,				"VK_G" },
	{ VK_H,				"VK_H" },
	{ VK_I,				"VK_I" },
	{ VK_J,				"VK_J" },
	{ VK_K,				"VK_K" },
	{ VK_L,				"VK_L" },
	{ VK_M,				"VK_M" },
	{ VK_N,				"VK_N" },
	{ VK_O,				"VK_O" },
	{ VK_P,				"VK_P" },
	{ VK_Q,				"VK_Q" },
	{ VK_R,				"VK_R" },
	{ VK_S,				"VK_S" },
	{ VK_T,				"VK_T" },
	{ VK_U,				"VK_U" },
	{ VK_V,				"VK_V" },
	{ VK_W,				"VK_W" },
	{ VK_X,				"VK_X" },
	{ VK_Y,				"VK_Y" },
	{ VK_Z,				"VK_Z" },
	{ VK_0,				"VK_0" },
	{ VK_1,				"VK_1" },
	{ VK_2,				"VK_2" },
	{ VK_3,				"VK_3" },
	{ VK_4,				"VK_4" },
	{ VK_5,				"VK_5" },
	{ VK_6,				"VK_6" },
	{ VK_7,				"VK_7" },
	{ VK_8,				"VK_8" },
	{ VK_9,				"VK_9" },
	{ VK_TEN_0,			"VK_TEN_0" },
	{ VK_TEN_1,			"VK_TEN_1" },
	{ VK_TEN_2,			"VK_TEN_2" },
	{ VK_TEN_3,			"VK_TEN_3" },
	{ VK_TEN_4,			"VK_TEN_4" },
	{ VK_TEN_5,			"VK_TEN_5" },
	{ VK_TEN_6,			"VK_TEN_6" },
	{ VK_TEN_7,			"VK_TEN_7" },
	{ VK_TEN_8,			"VK_TEN_8" },
	{ VK_TEN_9,			"VK_TEN_9" },
	{ VK_TEN_MINUS,		"VK_TEN_MINUS" },
	{ VK_TEN_PLUS,		"VK_TEN_PLUS" },
	{ VK_TEN_PERIOD,	"VK_TEN_PERIOD" },
	
	{ MONA_SUCCESS,			"MONA_SUCCESS" },
	{ MONA_FAILURE,			"MONA_FAILURE" },

	{ SEEK_SET,				"SEEK_SET" },
	{ SEEK_CUR,				"SEEK_CUR" },
	{ SEEK_END,				"SEEK_END" },

// TODO
//	{ FILE_OPEN_READ,			"FILE_OPEN_READ" },
//	{ FILE_OPEN_NORMAL_WRITE,	"FILE_OPEN_NORMAL_WRITE" },
//	{ FILE_OPEN_APPEND_WRITE,	"FILE_OPEN_APPEND_WRITE" },

	{ WAIT_TIMER,			"WAIT_TIMER" },
	{ WAIT_NONE,			"WAIT_NONE" },
	{ THREAD_UNKNOWN,		"THREAD_UNKNOWN" },

	{ MSG_NONE,					"MSG_NONE" },
	{ MSG_MAP,					"MSG_MAP" },
	{ MSG_MEMORY_MAP_ID,		"MSG_MEMORY_MAP_ID" },
	{ MSG_RESULT_OK,			"MSG_RESULT_OK" },
	{ MSG_RESULT_ERROR,			"MSG_RESULT_ERROR" },
	{ MSG_SERVER_START_OK,		"MSG_SERVER_START_OK" },
	{ MSG_INTERRUPTED,			"MSG_INTERRUPTED" },
	{ MSG_MONES_REGIST,			"MSG_MONES_REGIST" },
	{ MSG_MONES_IP_SEND,		"MSG_MONES_IP_SEND" },
	{ MSG_MONES_IP_NOTICE,		"MSG_MONES_IP_NOTICE" },
	{ MSG_MONES_ICMP_SEND,		"MSG_MONES_ICMP_SEND" },
	{ MSG_MONES_ICMP_NOTICE,	"MSG_MONES_ICMP_NOTICE" },
	{ MSG_MONES_FRAME_REQ,		"MSG_MONES_FRAME_REQ" },
	{ MSG_MONES_ARPTBL_REQ,		"MSG_MONES_ARPTBL_REQ" },
	{ MSG_MONES_WAKEUP_ARP_WAIT,"MSG_MONES_WAKEUP_ARP_WAIT" },
	{ MSG_THREAD_KILLED,		"MSG_THREAD_KILLED" },
	{ MSG_TIMER,				"MSG_TIMER" },

	{ ATTRIBUTE_DIRECTORY,	"ATTRIBUTE_DIRECTORY" },

	{ PEEK_REMOVE,			"PEEK_REMOVE" },
	{ DRIVE_NONE,			"DRIVE_NONE" },
	{ DRIVE_FD0,			"DRIVE_FD0" },
	{ DRIVE_CD0,			"DRIVE_CD0" },
	{ SHARED_FDC_BUFFER,	"SHARED_FDC_BUFFER" },

	{ FS_NO_ERROR,				"FS_NO_ERROR" },
	{ FS_INIT_ERROR,			"FS_INIT_ERROR" },
	{ FS_GET_ROOT_ERROR,		"FS_GET_ROOT_ERROR" },
	{ FS_GET_DIR_ERROR,			"FS_GET_DIR_ERROR" },
	{ FS_DIR_NOT_EXIST_ERROR,	"FS_DIR_NOT_EXIST_ERROR" },
	{ FS_ALREADY_OPEN_ERROR,	"FS_ALREADY_OPEN_ERROR" },
	{ FS_FILE_OPEN_ERROR,		"FS_FILE_OPEN_ERROR" },
	{ FS_FILE_NOT_FOUND,		"FS_FILE_NOT_FOUND" },
	{ FS_FILE_IS_NOT_OPEN,		"FS_FILE_IS_NOT_OPEN" },
	{ FS_FILE_EXIST,			"FS_FILE_EXIST" },
	{ FS_FILE_CREATE_ERROR,		"FS_FILE_CREATE_ERROR" },

	{ IDM_OBJECT_NOT_FOUND,		"IDM_OBJECT_NOT_FOUND" },
	{ IDM_SECURITY_ERROR,		"IDM_SECURITY_ERROR" },

	{ ID_MOUSE_SERVER,		"ID_MOUSE_SERVER" },
	{ ID_KEYBOARD_SERVER,	"ID_KEYBOARD_SERVER" },
	{ ID_FILE_SERVER,		"ID_FILE_SERVER" },
	{ ID_GUI_SERVER,		"ID_GUI_SERVER" },
	{ ID_ELF_SERVER,		"ID_ELF_SERVER" },
	{ ID_PROCESS_SERVER,	"ID_PROCESS_SERVER" },
	{ ID_PE_SERVER,			"ID_PE_SERVER" },
	{ ID_MONITOR_SERVER,	"ID_MONITOR_SERVER" },
	{ ID_NUMBER_OF_SERVERS,	"ID_NUMBER_OF_SERVERS" },

};


//================================================================
// SqSharedMemory
//================================================================
/**
 *	SqSharedMemory クラス
 *	<br>
 *	monapi_cmemoryinfo のラッパークラス
 */
class SqSharedMemory {
	monapi_cmemoryinfo* mi;		// monapi_cmemoryinfo の実体または NULL を指すポインタ
	
	static monapi_cmemoryinfo* s_creating_mi;

public:
	/**
	 *	C++ から SqSharedMemory オブジェクトを作成してスタックに積む
	 *	@param mi	割り当てる monapi_cmemoryinfo ポインタ
	 */
	static int push_memoryMap(monapi_cmemoryinfo* _mi) {
		HSQUIRRELVM v =  SquirrelVM::GetVMPtr();
		s_creating_mi = _mi;
		if (!CreateConstructNativeClassInstance(v, "SharedMemory")) {
			TRACE("createMemoryMapInstance() - push_memoryMap failed.\n");
			return 1;
		}
		return 1;
	}

	//-------------------------------------------------------------
	// Squirrel 用関数
	//-------------------------------------------------------------
	SqSharedMemory() : mi(NULL) {
		TRACE( "SqSharedMemory::SqSharedMemory() - p:%08x\n", this );
		if (s_creating_mi) {
			mi = s_creating_mi;
			s_creating_mi = NULL;
		}
	}
	virtual ~SqSharedMemory() {
		TRACE( "SqSharedMemory::~SqSharedMemory() - p:%08x\n", this );
		dispose();
	}


	bool create(dword size, int prompt) {
		dispose();
		mi = monapi_cmemoryinfo_new();
		if (!mi) return false;
		if (!monapi_cmemoryinfo_create(mi, size, prompt)) {
			monapi_cmemoryinfo_delete(mi);
			return false;
		}
		return true;
	}
	bool map(dword handle, dword size) {
		dispose();
		mi = monapi_cmemoryinfo_new();
		if (!mi) return false;
		mi->Handle = handle;
		mi->Size = size;
		if (!monapi_cmemoryinfo_map(mi)) {
			monapi_cmemoryinfo_delete(mi);
			return false;
		}
		return true;
	}
	void dispose() {
		if (mi) {
			monapi_cmemoryinfo_dispose(mi);
			monapi_cmemoryinfo_delete(mi);
			mi = NULL;
		}
	}
	
	dword getHandle() {
		return mi ? mi->Handle : 0;
	}
	dword getOwner() {
		return mi ? mi->Owner : 0;
	}
	dword getSize() {
		return mi ? mi->Size : 0;
	}
	
	SquirrelObject getString(dword pos, dword size) {
		if (!mi) return SquirrelObject();
		pos = max(0U, min(pos, mi->Size));
		size = max(0U, min(size, mi->Size - pos));

		HSQUIRRELVM v =  SquirrelVM::GetVMPtr();
		SquirrelObject so;
		sq_pushstring(v, (char*)(mi->Data + pos), size);
		so.AttachToStackObject(-1);
		sq_pop(v, 1);
		return so;
	}

	INT8 getInt8(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 1) return 0;
		return *(INT8*)(mi->Data + pos);
	}
	INT16 getInt16(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 2) return 0;
		return *(INT16*)(mi->Data + pos);
	}
	INT32 getInt32(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 4) return 0;
		return *(INT32*)(mi->Data + pos);
	}
	UINT8 getUInt8(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 1) return 0;
		return *(UINT8*)(mi->Data + pos);
	}
	UINT16 getUInt16(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 2) return 0;
		return *(UINT16*)(mi->Data + pos);
	}
	UINT32 getUInt32(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 4) return 0;
		return *(UINT32*)(mi->Data + pos);
	}
	FLOAT32 getFloat32(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 4) return 0;
		return *(FLOAT32*)(mi->Data + pos);
	}
	FLOAT64 getFloat64(dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 8) return 0;
		return *(FLOAT64*)(mi->Data + pos);
	}

	void setString(const char* str, dword pos) {
		if (!mi) return;
		pos = max(0U, min(pos, mi->Size));
		dword size = strlen(str);
		size = max(0U, min(size, mi->Size - pos));
		memcpy(mi->Data + pos, str, size);
	}

	void setInt8(INT8 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 1) return;
		*(INT8*)(mi->Data + pos) = val;
	}
	void setInt16(INT16 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 2) return;
		*(INT16*)(mi->Data + pos) = val;
	}
	void setInt32(INT32 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 4) return;
		*(INT32*)(mi->Data + pos) = val;
	}
	void setUInt8(UINT8 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 1) return;
		*(UINT8*)(mi->Data + pos) = val;
	}
	void setUInt16(UINT16 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 2) return;
		*(UINT16*)(mi->Data + pos) = val;
	}
	void setUInt32(UINT32 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 4) return;
		*(UINT32*)(mi->Data + pos) = val;
	}
	void setFloat32(FLOAT32 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 4) return;
		*(FLOAT32*)(mi->Data + pos) = val;
	}
	void setFloat64(FLOAT64 val, dword pos) {
		if (!mi || pos < 0 || pos >= mi->Size - 8) return;
		*(FLOAT64*)(mi->Data + pos) = val;
	}
};

monapi_cmemoryinfo* SqSharedMemory::s_creating_mi = NULL;


// Squirrel 内でのクラス名を宣言
DECLARE_INSTANCE_TYPE_NAME(SqSharedMemory, SharedMemory)



//================================================================
// monapiラッパー関数
//================================================================

int wrapper_syscall_send(dword id, SquirrelObject table) {
	MessageInfo info;
	info.header = table.GetValue("header").ToInteger();
	info.arg1 = table.GetValue("arg1").ToInteger();
	info.arg2 = table.GetValue("arg2").ToInteger();
	info.arg3 = table.GetValue("arg3").ToInteger();
	info.from = table.GetValue("from").ToInteger();
	const char* s = table.GetValue("str").ToString();
	int len = max(0, min((int)strlen(s), 128));
	strncpy(info.str, s, len);
	info.length = len;
	return syscall_send(id, &info);
}

SquirrelObject wrapper_syscall_receive() {
	MessageInfo info;
	int result = syscall_receive(&info);
	if (result != 0) return SquirrelObject();
	
	SquirrelObject table = SquirrelVM::CreateTable();
	table.SetValue( "header", (int)info.header );
	table.SetValue( "arg1", (int)info.arg1 );
	table.SetValue( "arg2", (int)info.arg2 );
	table.SetValue( "arg3", (int)info.arg3 );
	table.SetValue( "from", (int)info.from );
	table.SetValue( "str", string(info.str, max(0, min(info.length, 128))).c_str() );
	return table;
}

SquirrelObject wrapper_syscall_get_vram_info() {
	ScreenInfo info;
	syscall_get_vram_info(&info);
	SquirrelObject table = SquirrelVM::CreateTable();
	table.SetValue( "vram", (int)info.vram );
	table.SetValue( "bpp", (int)info.bpp );
	table.SetValue( "x", (int)info.x );
	table.SetValue( "y", (int)info.y );
	return table;
}

SquirrelObject wrapper_syscall_get_cursor() {
	int x, y;
	syscall_get_cursor(&x, &y);
	SquirrelObject table = SquirrelVM::CreateTable();
	table.SetValue( "x", x );
	table.SetValue( "y", y );
	return table;
}

SquirrelObject wrapper_syscall_get_date() {
	KDate date;
	syscall_get_date(&date);
	SquirrelObject table = SquirrelVM::CreateTable();
	table.SetValue( "year", (int)date.year );
	table.SetValue( "month", (int)date.month );
	table.SetValue( "day", (int)date.day );
	table.SetValue( "dayofweek", (int)date.dayofweek );
	table.SetValue( "min", (int)date.min );
	table.SetValue( "sec", (int)date.sec );
	return table;
}

SquirrelObject wrapper_syscall_get_arg() {
	int count = syscall_get_arg_count();
	TRACE("syscall_get_arg_count() - ret:%d\n", count);
	SquirrelObject array = SquirrelVM::CreateArray(count);

	char buf[2048];
	for (int i=0; i<count; ++i) {
		int result = syscall_get_arg(buf, i);
		TRACE("syscall_get_arg() - ret:%d\n", result);
		TRACE("   %d\n", buf);
		
		if (result != 0) break;
		array.SetValue( i, buf );
	};
	return array;
}

SquirrelObject wrapper_ps() {
	SquirrelObject array = SquirrelVM::CreateArray(0);

	int result = syscall_set_ps_dump();
	if (result != 0) return array;

	PsInfo info;
	while (true) {
		result = syscall_read_ps_dump(&info);
		if (result != 0) break;
		
		SquirrelObject table = SquirrelVM::CreateTable();
		table.SetValue( "name", info.name );
		table.SetValue( "state", (int)info.state );
		table.SetValue( "cr3", (int)info.cr3 );
		table.SetValue( "eip", (int)info.eip );
		table.SetValue( "esp", (int)info.esp );
		table.SetValue( "tid", (int)info.tid );
		array.ArrayAppend( table );
	};
	return array;
}

SquirrelObject wrapper_syscall_get_kernel_version() {
	char buf[2048];
	syscall_get_kernel_version(buf, sizeof(buf));
	return SquirrelVM::CreateString(buf);
}

SquirrelObject wrapper_syscall_peek(int index, int flags) {
	MessageInfo info;
	int result = syscall_peek(&info, index, flags);
	if (result != 0) return SquirrelObject();
	
	SquirrelObject table = SquirrelVM::CreateTable();
	table.SetValue( "header", (int)info.header );
	table.SetValue( "arg1", (int)info.arg1 );
	table.SetValue( "arg2", (int)info.arg2 );
	table.SetValue( "arg3", (int)info.arg3 );
	table.SetValue( "from", (int)info.from );
	table.SetValue( "str", string(info.str, max(0, min(info.length, 128))).c_str() );
	return table;
}

SquirrelObject wrapper_syscall_get_memory_info() {
	MemoryInfo info;
	int result = syscall_get_memory_info(&info);
	if (result != 0) return SquirrelObject();
	
	SquirrelObject table = SquirrelVM::CreateTable();
	table.SetValue( "totalMemoryH", (int)info.totalMemoryH );
	table.SetValue( "totalMemoryL", (int)info.totalMemoryL );
	table.SetValue( "freePageNum", (int)info.freePageNum );
	table.SetValue( "totalPageNum", (int)info.totalPageNum );
	table.SetValue( "pageSize", (int)info.pageSize );
	return table;
}


SquirrelObject wrapper_monapi_call_file_read_directory(const char* path, MONAPI_BOOL prompt) {
	SquirrelObject array = SquirrelVM::CreateArray(0);
// TODO
#if 0
	monapi_cmemoryinfo* mi = monapi_call_file_read_directory(path, prompt);
	int dsize = *(int*)mi->Data;
	if (mi == NULL || dsize == 0) return SquirrelObject();
	
	monapi_directoryinfo* pInfo = (monapi_directoryinfo*)&mi->Data[sizeof(int)];
	for (int i = 0; i < dsize; i++, pInfo++) {
		SquirrelObject table = SquirrelVM::CreateTable();
		table.SetValue( "name", pInfo->name );
		table.SetValue( "size", (int)pInfo->size );
		table.SetValue( "attr", (int)pInfo->attr );
		array.ArrayAppend( table );
	};
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
#endif
	return array;
}

int wrapper_monapi_call_file_read_data(HSQUIRRELVM v) {
// TODO
#if 0
	StackHandler sa(v);
	int nargs = sa.GetParamCount();
	if (nargs != 3) return 0;
	const char* file = sa.GetString(2);
	int prompt = sa.GetInt(3);
	monapi_cmemoryinfo* mi = monapi_call_file_read_data(file, prompt);
	SqSharedMemory::push_memoryMap(mi);
#endif
	return 1;
}

int wrapper_monapi_call_process_execute_file_get_tid(const char* command_line, MONAPI_BOOL prompt, dword stdout_id) {
	dword tid;
	int result = monapi_call_process_execute_file_get_tid(command_line, prompt, &tid, stdout_id);
	if (result != 0) return -1;
	return tid;
}

SquirrelObject wrapper_monapi_call_get_current_directory() {
	char buf[2048];
// TODO
#if 0
	monapi_call_get_current_directory(buf);
#endif
	return SquirrelVM::CreateString(buf);
}


//================================================================
// グローバル関数
//================================================================

/**
 *	テーブルに C 関数を追加する
 */
void RegisterVarArgs(HSQUIRRELVM v, HSQOBJECT hso, SQFUNCTION func, const SQChar* fname) {
	sq_pushobject( v, hso );
	sq_pushstring( v, fname, -1 );
	sq_newclosure( v, func, 0 );
	sq_createslot( v, -3 );
	sq_pop( v, 1 );
}


/**
 *	MonaSq/monapi ライブラリの初期化
 */
void monasq_init_monapi_lib(HSQUIRRELVM v)
{
	// root テーブル取得
	SquirrelObject root = SquirrelVM::GetRootTable();

	//--------------------------------------------------------------------------------
	// fake namespace monapi.
	//--------------------------------------------------------------------------------
	// root.monapi テーブルを作成
	SquirrelObject nameSpaceTable = SquirrelVM::CreateTable();
	root.SetValue(_T("monapi"), nameSpaceTable);

	// monapi 下に定数を作成
	for (size_t i=0; i<ARRAY_LENGTH(g_monapiConstantTable); ++i) {
		BindConstant( nameSpaceTable, g_monapiConstantTable[i].value, g_monapiConstantTable[i].name );
	}


	//--------------------------------------------------------------------------------
	// C++/SqSharedMemory クラスを Squirrel/SharedMemory クラスとして定義
	//--------------------------------------------------------------------------------
	SQClassDef<SqSharedMemory> sqMemoryMap(_T("SharedMemory"));
	
	sqMemoryMap.
		func(&SqSharedMemory::create, _T("create")).
		func(&SqSharedMemory::map, _T("map")).
		func(&SqSharedMemory::dispose, _T("dispose")).
		func(&SqSharedMemory::getHandle, _T("getHandle")).
		func(&SqSharedMemory::getOwner, _T("getOwner")).
		func(&SqSharedMemory::getSize, _T("getSize")).
		func(&SqSharedMemory::getString, _T("getString")).
		func(&SqSharedMemory::getInt8, _T("getInt8")).
		func(&SqSharedMemory::getInt16, _T("getInt16")).
		func(&SqSharedMemory::getInt32, _T("getInt32")).
		func(&SqSharedMemory::getUInt8, _T("getUInt8")).
		func(&SqSharedMemory::getUInt16, _T("getUInt16")).
		func(&SqSharedMemory::getUInt32, _T("getUInt32")).
		func(&SqSharedMemory::getFloat32, _T("getFloat32")).
		func(&SqSharedMemory::getFloat64, _T("getFloat64")).
		func(&SqSharedMemory::setString, _T("setString")).
		func(&SqSharedMemory::setInt8, _T("setInt8")).
		func(&SqSharedMemory::setInt16, _T("setInt16")).
		func(&SqSharedMemory::setInt32, _T("setInt32")).
		func(&SqSharedMemory::setUInt8, _T("setUInt8")).
		func(&SqSharedMemory::setUInt16, _T("setUInt16")).
		func(&SqSharedMemory::setUInt32, _T("setUInt32")).
		func(&SqSharedMemory::setFloat32, _T("setFloat32")).
		func(&SqSharedMemory::setFloat64, _T("setFloat64"))
	;


	//--------------------------------------------------------------------------------
	// register monapi function.
	//--------------------------------------------------------------------------------
	HSQOBJECT hNamespace = nameSpaceTable.GetObjectHandle();

	Register(v, hNamespace, sleep, _T("sleep"));
	Register(v, hNamespace, set_timer, _T("set_timer"));
	Register(v, hNamespace, kill_timer, _T("kill_timer"));
	Register(v, hNamespace, print, _T("print"));
	Register(v, hNamespace, kill, _T("kill"));
	Register(v, hNamespace, exit, _T("exit"));

	Register(v, hNamespace, syscall_test, _T("syscall_test"));
	Register(v, hNamespace, syscall_sleep, _T("syscall_sleep"));
	Register(v, hNamespace, syscall_set_timer, _T("syscall_set_timer"));
	Register(v, hNamespace, syscall_kill_timer, _T("syscall_kill_timer"));
	Register(v, hNamespace, syscall_print, _T("syscall_print"));
	Register(v, hNamespace, syscall_kill, _T("syscall_kill"));

	Register(v, hNamespace, wrapper_syscall_send, _T("syscall_send"));
	Register(v, hNamespace, wrapper_syscall_receive, _T("syscall_receive"));
	Register(v, hNamespace, syscall_exist_message, _T("syscall_exist_message"));

	Register(v, hNamespace, syscall_mutex_create, _T("syscall_mutex_create"));
	Register(v, hNamespace, syscall_mutex_trylock, _T("syscall_mutex_trylock"));
	Register(v, hNamespace, syscall_mutex_lock, _T("syscall_mutex_lock"));
	Register(v, hNamespace, syscall_mutex_trylock, _T("syscall_mutex_trylock"));
	Register(v, hNamespace, syscall_mutex_unlock, _T("syscall_mutex_unlock"));

	Register(v, hNamespace, wrapper_syscall_get_vram_info, _T("syscall_get_vram_info"));
	Register(v, hNamespace, wrapper_syscall_get_cursor, _T("syscall_get_cursor"));
	Register(v, hNamespace, syscall_set_cursor, _T("syscall_set_cursor"));

	Register(v, hNamespace, syscall_mutex_destroy, _T("syscall_mutex_destroy"));
//	Register(v, hNamespace, syscall_map, _T("syscall_map"));
//	Register(v, hNamespace, syscall_unmap2, _T("syscall_unmap2"));
	Register(v, hNamespace, syscall_lookup_main_thread, _T("syscall_lookup_main_thread"));
	Register(v, hNamespace, syscall_get_pid, _T("syscall_get_pid"));
	Register(v, hNamespace, syscall_get_tid, _T("syscall_get_tid"));
	Register(v, hNamespace, wrapper_syscall_get_arg, _T("syscall_get_arg"));
	Register(v, hNamespace, syscall_mthread_yield_message, _T("syscall_mthread_yield_message"));
	Register(v, hNamespace, wrapper_syscall_get_date, _T("syscall_get_date"));
	Register(v, hNamespace, wrapper_ps, _T("ps"));
	Register(v, hNamespace, syscall_get_io, _T("syscall_get_io"));
	Register(v, hNamespace, syscall_kill_thread, _T("syscall_kill_thread"));

	Register(v, hNamespace, syscall_lookup, _T("syscall_lookup"));
	Register(v, hNamespace, syscall_get_tick, _T("syscall_get_tick"));
	Register(v, hNamespace, wrapper_syscall_get_kernel_version, _T("syscall_get_kernel_version"));
	Register(v, hNamespace, syscall_clear_screen, _T("syscall_clear_screen"));
	Register(v, hNamespace, wrapper_syscall_peek, _T("syscall_peek"));

	Register(v, hNamespace, syscall_set_irq_receiver, _T("syscall_set_irq_receiver"));
	Register(v, hNamespace, syscall_has_irq_receiver, _T("syscall_has_irq_receiver"));
	Register(v, hNamespace, syscall_remove_irq_receiver, _T("syscall_remove_irq_receiver"));
	Register(v, hNamespace, wrapper_syscall_get_memory_info, _T("syscall_get_memory_info"));
	Register(v, hNamespace, syscall_free_pages, _T("syscall_free_pages"));
	Register(v, hNamespace, syscall_change_base_priority, _T("syscall_change_base_priority"));

	Register(v, hNamespace, syscall_set_dll_segment_writable, _T("syscall_set_dll_segment_writable"));
	Register(v, hNamespace, syscall_set_dll_segment_notshared, _T("syscall_set_dll_segment_notshared"));

	Register(v, hNamespace, inp8, _T("inp8"));
	Register(v, hNamespace, outp8, _T("outp8"));
	Register(v, hNamespace, inp16, _T("inp16"));
	Register(v, hNamespace, outp16, _T("outp16"));
	Register(v, hNamespace, inp32, _T("inp32"));
	Register(v, hNamespace, outp32, _T("outp32"));
	Register(v, hNamespace, monapi_set_irq, _T("monapi_set_irq"));
	Register(v, hNamespace, monapi_wait_interrupt, _T("monapi_wait_interrupt"));

	Register(v, hNamespace, monapi_get_server_thread_id, _T("monapi_get_server_thread_id"));
// TODO
//	Register(v, hNamespace, monapi_call_dispose_handle, _T("monapi_call_dispose_handle"));
	Register(v, hNamespace, monapi_register_to_server, _T("monapi_register_to_server"));
	Register(v, hNamespace, monapi_call_mouse_set_cursor, _T("monapi_call_mouse_set_cursor"));
// TODO
//	Register(v, hNamespace, monapi_call_file_open, _T("monapi_call_file_open"));
//	Register(v, hNamespace, monapi_call_file_get_file_size, _T("monapi_call_file_get_file_size"));
//	Register(v, hNamespace, monapi_call_file_seek, _T("monapi_call_file_seek"));
//	Register(v, hNamespace, monapi_call_file_close, _T("monapi_call_file_close"));
	
	Register(v, hNamespace, wrapper_monapi_call_file_read_directory, _T("monapi_call_file_read_directory"));
	RegisterVarArgs(v, hNamespace, wrapper_monapi_call_file_read_data, _T("monapi_call_file_read_data"));

	Register(v, hNamespace, monapi_call_process_execute_file, _T("monapi_call_process_execute_file"));
	Register(v, hNamespace, wrapper_monapi_call_process_execute_file_get_tid, _T("monapi_call_process_execute_file_get_tid"));
// TODO
//	Register(v, hNamespace, monapi_call_change_drive, _T("monapi_call_change_drive"));
//	Register(v, hNamespace, monapi_call_get_current_drive, _T("monapi_call_get_current_drive"));
	Register(v, hNamespace, wrapper_monapi_call_get_current_directory, _T("monapi_call_get_current_directory"));
// TODO
//	Register(v, hNamespace, monapi_call_change_directory, _T("monapi_call_change_directory"));

/*
extern int syscall_load_process_image(LoadProcessInfo* info);

extern monapi_cmemoryinfo* monapi_call_file_read(dword id, dword size);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_bz2_file(const char* file, MONAPI_BOOL prompt);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5(monapi_cmemoryinfo* mi);
extern monapi_cmemoryinfo* monapi_call_file_decompress_st5_file(const char* file, MONAPI_BOOL prompt);
*/



}
