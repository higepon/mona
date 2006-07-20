#include <stdlib.h>

int atoi(const char *s){
	int result;
	int mflag;

	if(s == NULL) return 0;
	mflag = 1;
	result = 0;

	while(isspace(*s)) s++;

	if(*s == '+'){
		mflag = 1;
		s++;
	} else if(*s == '-'){
		mflag = -1;
		s++;
	}

	while((*s >= '0') && (*s <= '9')){
		result = result*10 + (int)(*s - '0');
		s++;
	}

	return result*mflag;
}
