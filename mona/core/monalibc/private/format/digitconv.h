#ifndef _DIGITCONV_H_
#define _DIGITCONV_H_

#include <stddef.h>
#include <stdbool.h>

struct fix_traits
{
	bool another;
	bool zero_padding;
	bool left_align;
	bool space;
	bool sign_flag;
	bool add_sign_flag;
	int field_width;
	int prec;
};

int __nc_count_digits(int n, int x);
int __nc_conv_digits(char *s, size_t size, unsigned int n, int x, const struct fix_traits *tr_);
char *__nc_fix_format_string(char *s, size_t size, const struct fix_traits *tr_);
int __nc_digitconv(char *s, size_t size, int n, int x, const struct fix_traits *tr_);
int __nc_getStringLength(unsigned int n, int x, const struct fix_traits *tr_);

#endif

