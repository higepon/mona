#include	<compiler.h>


static void memmng_initialize(dword addr, UINT size);
static void memmng_deinitialize(void);
	ATETBL	__atetbl;


// ---- crt

#ifdef __cplusplus
extern "C" {
#endif
#if defined(__GNUC__)
extern const PFV	__CTOR_LIST__[];
extern const PFV	__DTOR_LIST__[];
#elif defined(_MSC_VER)
#pragma data_seg(".CRT$XCA")
PFV		__xc_a[] = {NULL};
#pragma data_seg(".CRT$XCZ")
PFV		__xc_z[] = {NULL};
#pragma data_seg()
#else
#warning unsupport C++
#endif
#ifdef __cplusplus
}
#endif

#if defined(__GNUC__)
static void __cinitterm(const PFV *list) {

	dword	cnt;

	cnt = (dword)list[0];
	list++;
	list = (PFV *)((((dword)list) + 3) & (~3));		// align bug?
	if (cnt == (dword)-1) {
		while(*list != NULL) {
			(**list++)();
		}
	}
	else {
		while(cnt) {
			cnt--;
			(**list++)();
		}
	}
}
#elif defined(_MSC_VER)
static void __cinitterm(const PFV *from, const PFV *to) {

	while(from < to) {
		if ((*from != NULL) && (*from != (PFV)-1)) {
			(**from)();
		}
		from++;
	}
}
#endif

int user_start(void) {

	memmng_initialize(0xC0000400, 8 * 1024 * 1024 - 0x400);
#if defined(__GNUC__)
	__cinitterm(__CTOR_LIST__);
#elif defined(_MSC_VER)
	__cinitterm(__xc_a, __xc_z);
#endif
	return(user_end(svrmain()));
}

int user_end(int status) {

	ATETBL			ate;
	unsigned int	r;
	ATETBL			aten;

#if defined(__GNUC__)
	__cinitterm(__DTOR_LIST__);
#endif

	ate = __atetbl;
	while(ate) {
		r = ate->count;
		while(r) {
			ate->func[--r]();
		}
		aten = ate->next;
		free(ate);
		ate = aten;
	}
	memmng_deinitialize();
	syscall_kill();
	return(status);
}


// ---- malloc/free

struct _memhdr;

typedef struct _memhdr	_MEMHDR;
typedef struct _memhdr	*MEMHDR;

struct _memhdr {
	UINT	size;
	UINT	lock;
	MEMHDR	back;
	MEMHDR	next;
};

static	MEMHDR		used;
static	MEMHDR		unused;
#if defined(_MT)
static	int			csec;
#endif

static void memmng_initialize(dword addr, UINT size) {

	used = NULL;
	unused = (MEMHDR)addr;
	unused->size = size;
	unused->lock = 0;
	unused->back = NULL;
	unused->next = NULL;
#if defined(_MT)
	csec = syscall_mutex_create();
#endif
}

static void memmng_deinitialize(void) {

#if defined(_MT)
	syscall_mutex_destroy(csec);
#endif
}

void *malloc(dword size) {

	MEMHDR	ptr;
	size_t	remain;
	MEMHDR	fwd;
	MEMHDR	p;

#if defined(_MT)
	syscall_mutex_lock(csec);
#endif
	size = sizeof(_MEMHDR) + ((size + 15) & (~15));
	ptr = unused;
	while(1) {
		if (ptr == NULL) {
#if defined(_MT)
			syscall_mutex_unlock(csec);
#endif
			return(NULL);
		}
		if (ptr->size >= size) {
			break;
		}
		ptr = ptr->next;
	}
	remain = ptr->size - size;
	if (remain > sizeof(_MEMHDR)) {
		fwd = (MEMHDR)((dword)ptr + size);
		fwd->size = remain;
		fwd->lock = 0;
		fwd->back = ptr->back;
		fwd->next = ptr->next;
		// ƒŠƒ“ƒNC³
		p = ptr->back;
		if (p) {
			p->next = fwd;
		}
		else {
			unused = fwd;
		}
		p = ptr->next;
		if (p) {
			p->back = fwd;
		}
	}
	else {
		size = ptr->size;
		unused = NULL;
	}
	// ƒŠƒ“ƒN’Ç‰Á
	p = used;
	used = ptr;
	ptr->size = size;
	ptr->lock = 0;
	ptr->back = NULL;
	ptr->next = p;
	if (p) {
		p->back = ptr;
	}
#if defined(_MT)
	syscall_mutex_unlock(csec);
#endif
	return((void *)(ptr + 1));
}

void free(void *addr) {

	MEMHDR	cur;
	MEMHDR	ptr;
	MEMHDR	back;
	MEMHDR	next;

#if defined(_MT)
	syscall_mutex_lock(csec);
#endif
	cur = ((MEMHDR)addr) - 1;
	ptr = used;
	while(1) {
		if (ptr == NULL) {
#if defined(_MT)
			syscall_mutex_unlock(csec);
#endif
			return;
		}
		if (ptr == cur) {
			break;
		}
		ptr = ptr->next;
	}
	// ƒŠƒ“ƒNC³
	back = cur->back;
	next = cur->next;
	if (back) {
		back->next = next;
	}
	else {
		used = next;
	}
	if (next) {
		next->back = back;
	}
	// ƒŠƒ“ƒN’Ç‰Á
	back = unused;
	next = NULL;
	while(back) {
		next = back->next;
		if ((next == NULL) || (next > cur)) {
			break;
		}
		back = next;
	}
	if ((back) && (back < cur)) {
		cur->back = back;
		cur->next = next;
		back->next = cur;
		if (next) {
			next->back = cur;
		}
	}
	else {
		next = back;					// ˆê‰ñ–ß‚é
		back = NULL;
		cur->back = back;
		cur->next = next;
		if (next) {
			next->back = cur;
		}
		unused = cur;
	}
	// ‚±‚ñ‚Ï‚­‚µ‚å‚ñ
	ptr = cur->next;
	if ((ptr) && (((dword)ptr - (dword)cur) == cur->size)) {
		cur->size += ptr->size;
		cur->next = ptr->next;
	}
	if (back) {
		ptr = back->next;		// cur
		if ((ptr) && (((dword)ptr - (dword)back) == back->size)) {
			back->size += ptr->size;
			back->next = ptr->next;
		}
	}
#if defined(_MT)
	syscall_mutex_unlock(csec);
#endif
}


// ---- atexit

int atexit(void (*func)(void)) {

	ATETBL	ate;

	if (func == NULL) {
		return(0);
	}
	ate = __atetbl;
	if ((ate == NULL) || (ate->count >= (sizeof(ate->func)/sizeof(PFV)))) {
		ate = (ATETBL)malloc(sizeof(_ATETBL));
		if (ate == NULL) {
			return(-1);
		}
		memset(ate, 0, sizeof(_ATETBL));
		ate->next = __atetbl;
		__atetbl = ate;
	}
	ate->func[ate->count++] = func;
	return(0);
}


// ---- memory, string

#if !defined(_MSC_VER)
void *memset(void *buf, int c, size_t count) {

	char	*p;

	p = (char *)buf;
	while(count > 0) {
		count--;
		*p++ = (char)c;
	}
	return(buf);
}

void *memcpy(void *buf1, const void *buf2, size_t count) {

const char	*p;
	char	*q;

	p = (char *)buf2;
	q = (char *)buf1;
	while(count > 0) {
		count--;
		*q++ = *p++;
	}
	return(buf1);
}

size_t strlen(const char *string) {

const char	*p;

	p = string;
	while(*p) {
		p++;
	}
	return(p - string);
}
#endif


// ---- puts

static const char str_cr[] = "\n";

int puts(const char *string) {

	syscall_print(string);
	syscall_print(str_cr);
	return(1);
}


// ---- printf

typedef int (*PFSTO)(char c, void *hdl);

enum {
	PF_ESCAPE		= 0x0001,
	PF_UNSIGNED		= 0x0002,
	PF_CAPITAL		= 0x0004,
	PF_WIDTH		= 0x0008,
	PF_PRECISION	= 0x0010,
	PF_SPACE		= 0x0020,
	PF_ZERO			= 0x0040,
	PF_PLUS			= 0x0080,
	PF_MINUS		= 0x0100,
	PF_SHORT		= 0x0200,
	PF_LONG			= 0x0400
};

static const char nullstr[] = "(NULL)";

static int pfstos(PFSTO cb, void *hdl, const char *str, int width, int flag) {

	int		r;
	int		len;

	if (str == NULL) {
		str = nullstr;
	}
	len = strlen(str);
	width = max(width, len) - len;
	r = 0;
	if (!(flag & PF_MINUS)) {
		while(width) {
			width--;
			r += (*cb)(' ', hdl);
		}
	}
	while(*str) {
		r += (*cb)(*str++, hdl);
	}
	while(width) {
		width--;
		r += (*cb)(' ', hdl);
	}
	return(r);
}

static int pfval(PFSTO cb, void *hdl, unsigned long val, int width, int prec,
											unsigned long radix, int flag) {

	int		r;
	char	sig;
	int		size;
	int		c;
	int		len;
	char	work[40];

	r = 0;
	sig = '+';
	if (flag & (PF_PRECISION | PF_MINUS)) {
		flag &= ~PF_ZERO;
	}
	if ((!(flag & PF_UNSIGNED)) && ((signed long)val < 0)) {
		flag |= PF_PLUS;
		sig = '-';
		val = (unsigned long)(-(signed long)val);
	}
	if ((flag & (PF_SPACE | PF_PLUS)) == PF_SPACE) {
		flag |= PF_PLUS;
		sig = ' ';
	}
	size = 0;
	do {
		c = val % radix;
		val = val / radix;
		if (c >= 10) {
			c += (0x41 - 0x3a);
			if (!(flag & PF_CAPITAL)) {
				c += 0x20;
			}
		}
		work[size++] = (char)(c + 0x30);
	} while(val);
	len = max(size, prec);
	if (flag & PF_PLUS) {
		len++;
	}
	width = max(width, len) - len;

	if (!(flag & (PF_MINUS | PF_ZERO))) {
		while(width) {
			width--;
			r += (*cb)(' ', hdl);
		}
	}
	if (flag & PF_PLUS) {
		r += (*cb)(sig, hdl);
	}
	if (flag & PF_ZERO) {
		while(width) {
			width--;
			r += (*cb)('0', hdl);
		}
	}
	while(prec > size) {
		prec--;
		r += (*cb)('0', hdl);
	}
	while(size) {
		size--;
		r += (*cb)(work[size], hdl);
	}
	while(width) {
		width--;
		r += (*cb)(' ', hdl);
	}
	return(r);
}

int __output(PFSTO cb, void *hdl, const char *fmt, va_list ap) {

	int		r;
	char	c;
	int		flag;
	int		width = 0;
	int		prec = 0;
	int		radix;

	r = 0;
	flag = 0;
	while(1) {
		c = *fmt++;
		if (c == '\0') {
			break;
		}
		else if (flag) {
			radix = 0;
			switch(c) {
				case 'h':
				case 'N':
					flag |= PF_SHORT;
					break;

				case 'l':
				case 'F':
				case 'L':
					flag |= PF_LONG;
					break;

				case 's':
					r += pfstos(cb, hdl, va_arg(ap, char *), width, flag);
					flag = 0;
					break;

				case 'u':
					flag |= PF_UNSIGNED;

				case 'd':
				case 'i':
					radix = 10;
					break;

				case 'o':
					radix = 8;
					flag |= PF_UNSIGNED;
					break;

				case 'X':
					flag |= PF_CAPITAL;

				case 'x':
				case 'p':
					flag |= PF_UNSIGNED;
					radix = 16;
					break;

				case 'c':
					r += (*cb)(va_arg(ap, char), hdl);
					flag = 0;
					break;

				case '%':
					r += (*cb)('%', hdl);
					flag = 0;
					break;

				case '+':
					flag |= PF_PLUS;
					break;

				case '-':
					flag |= PF_MINUS;
					break;

				case ' ':
					flag |= PF_SPACE;
					break;

				case '0':
					if (!(flag & (PF_WIDTH | PF_PRECISION))) {
						flag |= PF_ZERO;
					}

				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					if (flag & PF_PRECISION) {
						prec = prec * 10 + (int)(c - '0');
					}
					else {
						width = width * 10 + (int)(c - '0');
						flag |= PF_WIDTH;
					}
					break;

				case '.':
					flag |= PF_PRECISION;
					break;

				case '#':
				case 'g':
				case 'G':
				case 'e':
				case 'E':
				case 'n':
				default:
					flag = 0;
					break;
			}
			if (radix) {
				r += pfval(cb, hdl, va_arg(ap, int), width, prec,
																radix, flag);
				flag = 0;
			}
		}
		else if (c == '%') {
			flag = PF_ESCAPE;
			width = 0;
			prec = 0;
		}
		else {
			r += (*cb)(c, hdl);
		}
	}
	return(r);
}

static int printfcb(char c, void *hdl) {

	char	buf[2];

	buf[0] = c;
	buf[1] = '\0';
	syscall_print(buf);
	return(1);
}

int printf(const char *format, ...) {

	va_list	ap;
	int		r;

	va_start(ap, format);
	r = __output(printfcb, NULL, format, ap);
	va_end(ap);
	return(r);
}


// ---- a2i

int atoi(const char *string) {

	int		ret;
	int		c;
	int		s = 1;

	while(*string == ' ') {
		string++;
	}
	ret = 0;
	c = *string;
	if (c == '+') {
		string++;
	}
	else if (c == '-') {
		string++;
		s = -1;
	}
	while(1) {
		c = *string++;
		c -= '0';
		if ((unsigned)c < 10) {
			ret *= 10;
			ret += c;
		}
		else {
			break;
		}
	}
	return(ret * s);
}

