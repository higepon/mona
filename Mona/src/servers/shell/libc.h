
// メッセージ増やすんなら、ここも追加してね～（にっこり
enum {
	MSG_DISPOSE_HANDLE					= 0x0100,

	MSG_REGISTER_TO_SERVER				= 0x0110,
	MSG_UNREGISTER_FROM_SERVER,

	MSG_KEY_REGIST_TO_SERVER			= 0x0200,
	MSG_KEY_UNREGIST_FROM_SERVER,
	MSG_KEY_VIRTUAL_CODE,

	MSG_MOUSE_REGIST_TO_SERVER			= 0x0300,
	MSG_MOUSE_UNREGIST_FROM_SERVER,
	MSG_MOUSE_INFO,
	MSG_MOUSE_ENABLE_CURSOR,
	MSG_MOUSE_DISABLE_CURSOR,

	MSG_FILE_READ_DATA					= 0x0400,
	MSG_FILE_DECOMPRESS_BZ2,
	MSG_FILE_DECOMPRESS_BZ2_FILE,

	MSG_ELF_EXECUTE_FILE				= 0x0500,

	MSG_PROCESS_GET_PROCESS_INFO		= 0x1000,
	MSG_PROCESS_CREATED,
	MSG_PROCESS_TERMINATED
};

enum {
	MKEY_BACK			= 8,
	MKEY_TAB			= 9,
	MKEY_ENTER			= 13,
	MKEY_SHIFT			= 16,
	MKEY_CONTROL		= 17,
	MKEY_MENU			= 18,
	MKEY_PAUSE			= 19,
	MKEY_CAPS			= 20,
	MKEY_KANA			= 21,
	MKEY_KANJI			= 25,
	MKEY_ESCAPE			= 27,
	MKEY_XFER			= 28,
	MKEY_NFER			= 29,
	MKEY_IMEACEEPT		= 30,
	MKEY_IMEMODE		= 31,
	MKEY_SPACE			= 32,
	MKEY_PAGEUP			= 33,
	MKEY_PAGEDOWN		= 34,
	MKEY_END			= 35,
	MKEY_HOME			= 36,
	MKEY_LEFT			= 37,
	MKEY_UP				= 38,
	MKEY_RIGHT			= 39,
	MKEY_DOWN			= 40,
	MKEY_PRINT			= 44,
	MKEY_INSERT			= 45,
	MKEY_DELETE			= 46,
	MKEY_0				= 48,
	MKEY_1				= 49,
	MKEY_2				= 50,
	MKEY_3				= 51,
	MKEY_4				= 52,
	MKEY_5				= 53,
	MKEY_6				= 54,
	MKEY_7				= 55,
	MKEY_8				= 56,
	MKEY_9				= 57,
	MKEY_A				= 65,
	MKEY_B				= 66,
	MKEY_C				= 67,
	MKEY_D				= 68,
	MKEY_E				= 69,
	MKEY_F				= 70,
	MKEY_G				= 71,
	MKEY_H				= 72,
	MKEY_I				= 73,
	MKEY_J				= 74,
	MKEY_K				= 75,
	MKEY_L				= 76,
	MKEY_M				= 77,
	MKEY_N				= 78,
	MKEY_O				= 79,
	MKEY_P				= 80,
	MKEY_Q				= 81,
	MKEY_R				= 82,
	MKEY_S				= 83,
	MKEY_T				= 84,
	MKEY_U				= 85,
	MKEY_V				= 86,
	MKEY_W				= 87,
	MKEY_X				= 88,
	MKEY_Y				= 89,
	MKEY_Z				= 90,
	MKEY_LWIN			= 91,
	MKEY_RWIN			= 92,
	MKEY_APPS			= 93,
	MKEY_NUM0			= 96,
	MKEY_NUM1			= 97,
	MKEY_NUM2			= 98,
	MKEY_NUM3			= 99,
	MKEY_NUM4			= 100,
	MKEY_NUM5			= 101,
	MKEY_NUM6			= 102,
	MKEY_NUM7			= 103,
	MKEY_NUM8			= 104,
	MKEY_NUM9			= 105,
	MKEY_MULTIPLE		= 106,
	MKEY_ADD			= 107,
	MKEY_SUBTRACT		= 109,
	MKEY_DECIMAL		= 110,
	MKEY_DIVIDE			= 111,
	MKEY_F1				= 112,
	MKEY_F2				= 113,
	MKEY_F3				= 114,
	MKEY_F4				= 115,
	MKEY_F5				= 116,
	MKEY_F6				= 117,
	MKEY_F7				= 118,
	MKEY_F8				= 119,
	MKEY_F9				= 120,
	MKEY_F10			= 121,
	MKEY_F11			= 122,
	MKEY_F12			= 123,
	MKEY_NUMLOCK		= 144,
	MKEY_SCROLL			= 145,
	MKEY_LSHIFT			= 160,
	MKEY_RSHIFT			= 161,
	MKEY_LCONTROL		= 162,
	MKEY_RCONTROL		= 163,
	MKEY_LMENU			= 164,
	MKEY_RMENU			= 165,
	MKEY_OEM_SEMICOLON	= 186,
	MKEY_OEM_PLUS		= 187,
	MKEY_OEM_COMMA		= 188,
	MKEY_OEM_MINUS		= 189,
	MKEY_OEM_PERIOD		= 190,
	MKEY_OEM_QUESTION	= 191,
	MKEY_OEM_TILDE		= 192,
	MKEY_OEM_LBRACKETS	= 219,
	MKEY_OEM_PIPE		= 220,
	MKEY_OEM_RBRACKETS	= 221,
	MKEY_OEM_QUOTES		= 222,
	MKEY_OEM_BACKSLASH	= 226,

	MKEY_MAX			= 256
};

enum {
	MKEY_MODIFIER_DOWN	= 0x01,
	MKEY_MODIFIER_UP	= 0x02,
	MKEY_MODIFIER_SHIFT	= 0x04,
	MKEY_MODIFIER_ALT	= 0x08,
	MKEY_MODIFIER_CTRL	= 0x10,
	MKEY_MODIFIER_WIN	= 0x20,
	MKEY_MODIFIER_MENU	= 0x40,
	MKEY_MODIFIER_CHAR	= 0x80
};

#define max(a, b)	(((a)>(b))?(a):(b))
#define min(a, b)	(((a)<(b))?(a):(b))

typedef	char			*va_list;
#define va_start(ap, last)	(ap) = (va_list)&(last) + sizeof(last)
#define va_arg(ap, t)		((t *)(ap += sizeof(t)))[-1]
#define va_end(ap)			(ap) = NULL

typedef void (*PFV)(void);
typedef struct _atetbl		 _ATETBL;
typedef struct _atetbl		 *ATETBL;
struct _atetbl {
	ATETBL			next;
	unsigned int	count;
	PFV				func[64];
};


#ifdef __cplusplus
extern "C" {
#endif

extern	ATETBL	__atetbl;

int user_end(int status);
int user_start(void);

int svrmain(void);

int atexit(void (*func)(void));

int puts(const char *string);
int printf(const char *format, ...);
void *memset(void *buf, int c, size_t count);
void *memcpy(void *buf1, const void *buf2, size_t count);
size_t strlen(const char *string);

int atoi(const char *string);

void *malloc(dword size);
void free(void *addr);

#ifdef __cplusplus
}
#endif


// ---- for cxx
#if 0
inline void *operator new(unsigned int size) {

	return(::memmng_alloc(size));
}

inline void operator delete(void *ptr) {

	::memmng_free(ptr);
}

inline void *operator new[](unsigned int size) {

	return(::memmng_alloc(size));
}

inline void operator delete[](void *ptr) {

	::memmng_free(ptr);
}
#endif

