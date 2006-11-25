#ifndef _FORMAT_H_
#define _FORMAT_H_

#include <stddef.h>
#include <stdarg.h>

extern const char flag_char[];
extern const char conv_char[];

typedef struct _convspec
{
	int flag;
	int field_width;
	int precision;
	int length_modifier;
	int conversion;
	int field_width_arg_index;
	int precision_arg_index;
	int index;
} ConvSpec;

struct _informat
{
	int flag;
	union
	{
		ConvSpec *cs;
		char *s;
		void *p;
	} u;
};

typedef struct _format
{
	struct _informat *ins; // array
	size_t length;
} Format;

#define FLAG_LEFT	0x0001
#define FLAG_SIGNED	0x0002
#define FLAG_SPACE	0x0004
#define FLAG_SUB	0x0008
#define FLAG_ZERO	0x0010

#define LMOD_CHAR	1
#define LMOD_SHORT	2
#define LMOD_LONG	3
#define LMOD_LONGLONG	4
#define LMOD_INTMAX	5
#define LMOD_SIZE_T	6
#define LMOD_PTRDIFF_T	7
#define LMOD_LONGDOUBLE	8

#define CS_d		1
#define CS_i		2
#define CS_o		3
#define CS_u		4
#define CS_x		5
#define CS_X		6
#define CS_f		7
#define CS_F		8
#define CS_e		9
#define CS_E		10
#define CS_g		11
#define CS_G		12
#define CS_a		13
#define CS_A		14
#define CS_c		15
#define CS_s		16
#define CS_p		17
#define CS_n		18
#define CS_ESCAPE	19

#define FA_LENGTH	(CS_ESCAPE)

#define NEXT_ARG_INDEX	(-1)
#define DEFAULT_INDEX	(-2)

union _arg;
extern int (*convFuncs[FA_LENGTH+1])(char*, size_t, size_t, ConvSpec*, union _arg*, int);

int __isflag_char(char c);
int __isconv_char(char c);
int convspec_parse(ConvSpec *cs, char *str);
size_t get_next_convspec_index(const char *str, size_t index);
size_t get_next_convspec_end_index(const char *str, size_t index);
size_t count_convspecs(const char *str);
void dump_ConvSpec(ConvSpec *cs);

size_t count_format(Format *format, int flag);
int parse_format(Format *format, const char *str);
int init_format(Format **fmtp);
int free_format(Format *format);
size_t trans_format(char *dest, size_t size, const char *format, va_list ap);

#endif
