
// prefix header for Mona

//#include	<monacapi.h>
//#include	<SDL.h>
#include	<monapi.h>
#include	<monapi/messages.h>

#define	BYTESEX_LITTLE

typedef signed char		SINT8;
typedef unsigned char	UINT8;
typedef	short			SINT16;
typedef unsigned short	UINT16;
typedef	int				SINT32;
typedef unsigned int	UINT32;
typedef char			BOOL;
typedef unsigned char	BYTE;
typedef unsigned int	UINT;

#define	ZeroMemory(a, b)		memset((a),  0 , (b))
#define	FillMemory(a, b, c)		memset((a), (c), (b))
#define	CopyMemory(a, b, c)		memcpy((a), (void *)(b), (c))

#include	"common.h"

#ifndef FALSE
#define	FALSE		0
#endif
#ifndef TRUE
#define	TRUE		1
#endif

#define	_MALLOC(a, b)	malloc(a)
#define	_MFREE(a)		free(a)
#define	TRACEOUT(a)

#define	FASTCALL

