
enum {
	SUCCESS		= 0,
	FAILURE		= 1
};

#ifndef PI
#define PI 3.14159265358979
#endif

#ifndef LOADINTELDWORD
#define	LOADINTELDWORD(a)		(((UINT32)(a)[0]) |				\
								((UINT32)(a)[1] << 8) |			\
								((UINT32)(a)[2] << 16) |		\
								((UINT32)(a)[3] << 24))
#endif

#ifndef LOADINTELWORD
#define	LOADINTELWORD(a)		(((UINT16)(a)[0]) | ((UINT16)(a)[1] << 8))
#endif

#ifndef STOREINTELDWORD
#define	STOREINTELDWORD(a, b)	*((a)+0) = (BYTE)((b));			\
								*((a)+1) = (BYTE)((b)>>8);		\
								*((a)+2) = (BYTE)((b)>>16);		\
								*((a)+3) = (BYTE)((b)>>24)
#endif

#ifndef STOREINTELWORD
#define	STOREINTELWORD(a, b)	*((a)+0) = (BYTE)((b));			\
								*((a)+1) = (BYTE)((b)>>8)
#endif


// ---- Optimize Macros

#ifndef REG8
#define	REG8		BYTE
#endif
#ifndef REG16
#define	REG16		UINT16
#endif

#ifndef LOW12
#define	LOW12(a)				((a) & 0x0fff)
#endif
#ifndef LOW14
#define	LOW14(a)				((a) & 0x3fff)
#endif
#ifndef LOW15
#define	LOW15(a)				((a) & 0x7fff)
#endif
#ifndef LOW16
#define	LOW16(a)				((UINT16)(a))
#endif
#ifndef HIGH16
#define	HIGH16(a)				(((UINT32)(a)) >> 16)
#endif

#ifndef STRLITERAL
#ifdef UNICODE
#define	STRLITERAL(string)		_T(string)
#else
#define	STRLITERAL(string)		(string)
#endif
#endif

#if !defined(RGB16)
#define	RGB16		UINT16
#endif

#if !defined(RGB32)
#if defined(BYTESEX_LITTLE)
typedef union {
	UINT32	d;
	struct {
		UINT8	b;
		UINT8	g;
		UINT8	r;
		UINT8	e;
	} p;
} RGB32;
#define	RGB32D(r, g, b)		(((r) << 16) + ((g) << 8) + ((b) << 0))
#elif defined(BYTESEX_BIG)
typedef union {
	UINT32	d;
	struct {
		UINT8	e;
		UINT8	r;
		UINT8	g;
		UINT8	b;
	} p;
} RGB32;
#define	RGB32D(r, g, b)		(((r) << 16) + ((g) << 8) + ((b) << 0))
#endif
#endif


#if !defined(INLINE)
#define	INLINE
#endif
#if !defined(FASTCALL)
#define	FASTCALL
#endif

