#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#if defined(HOST_LINUX2) || defined(HOST_LINUX3)
#include	<sys/io.h>
#else
#include	<io.h>
#endif
#include	<sys/stat.h>

typedef signed char			SINT8;
typedef unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
typedef	signed int			SINT;
typedef	unsigned int		UINT;

