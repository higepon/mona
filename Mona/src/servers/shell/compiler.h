
#include	<sys/types.h>
#include	<sys/error.h>
#include	"syscall.h"
#include	"libc.h"

#define	BYTESEX_LITTLE
#define	OSLANG_ANK
#define	OSLINEBREAK_LF

typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
typedef	signed int			SINT;
typedef	unsigned int		UINT;
typedef char				BOOL;

#define	FALSE	0
#define	TRUE	(!FALSE)

#define	LOADINTELDWORD(a)		(*((UINT32 *)(a)))
#define	LOADINTELWORD(a)		(*((UINT16 *)(a)))
#define	STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define	STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)

#define	SUPPORT_ANK

#include	"common.h"
#include	"milstr.h"

#define	USE_FILESVR		"FILE.BIN"

