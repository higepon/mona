#ifndef _MONA_LIB_TIME_
#define _MONA_LIB_TIME_

typedef int time_t;

#ifdef __cplusplus
extern "C" {
#endif

time_t time(time_t *t);

#ifdef __cplusplus
}
#endif

#endif
