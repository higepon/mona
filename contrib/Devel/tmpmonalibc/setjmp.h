#ifndef _TMPMONA_LIB_SETJMP_
#define _TMPMONA_LIB_SETJMP_

typedef void* jmp_buf;

#ifdef __cplusplus
extern "C" {
#endif

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);

#ifdef __cplusplus
}
#endif

#endif
