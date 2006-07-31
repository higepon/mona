#include <stdlib.h>
#include <limits.h>

size_t strtoi(const char *s, char **endptr, int base, int width, char flag)
{
	const char *tmp = s;
	const char *head;
	unsigned long int result = 0;
	int mflag = 1;
	unsigned long int max;

	if(s == NULL) return result;
	if(base > 36) base = 36; /* check base */
	if(base < 0) base = 0;
	while(isspace(*s)) s++; /* skip spaces */
	if(*s == '+'){ /* init Minus flag */
		s++;
	} else if(*s == '-'){
		mflag = -1;
		s++;
	}

	head = s;
	if(*head == '0'){ /* modify base using s */
		head++;
		if( ((*head == 'X') || (*head == 'x')) && ((base == 0) || (base == 16)) ){
			head++;
			if(isxdigit(*head) != 0){
				base = 16;
				s = head; /* skip 0X or 0x */
			}
		} else if(*head >= '1' && *head <= '7'){
			if(base == 0) base = 8;
		} else {
			if(base == 0) base = 10;
		}
	} else if(base == 0){
		base = 10;
	}
	head = s;

	if(width == 0) width = INT_MAX;
	width -= (int)(head - tmp);
	if(flag & S_FORMAT_UNSIGNED){
		mflag = 1;
		if(flag & S_FORMAT_LONG){
			max = ULONG_MAX;
		} else {
			max = UINT_MAX;
		}
	} else {
		if(flag & S_FORMAT_LONG){
			max = LONG_MAX;
		} else {
			max = INT_MAX;
		}
	}

	while(width-- > 0){ /* conversion */
		long int value;

		if(!isalnum(*s)) break;
		if(isdigit(*s)){
			value = *s - '0';
		} else {
			value = toupper(*s) - ('A' - 10);
		}
		if(value >= base) break;
		if(result == max){
			s++;
			continue;
		} else if(result > (max - value)/base){
			result = max;
			s++;
			continue;
		}
		result = result*base + value;
		s++;
	}

	if(endptr != NULL){ /* set endptr */
		if(s == head){
			*endptr = (char *)tmp;
		} else {
			*endptr = (char *)s;
		}
	}

	return result * (long)mflag;
}
