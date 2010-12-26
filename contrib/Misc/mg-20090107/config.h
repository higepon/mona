#define FKEYS
// #define REGEX
#define XKEYS
#ifndef LOGIN_NAME_MAX
#define MAXLOGNAME _POSIX_LOGIN_NAME_MAX
#endif
#ifndef MAXLOGNAME
#define MAXLOGNAME LOGIN_NAME_MAX
#endif
#define HAVE_NOSTRTONUM
#define HAVE_NOSTRLCPY
#define HAVE_NOSTRLCAT
#define HAVE_NOFGETLN
#define GNU_LS

// MONA Only
#define MG_NUM_ROWS (40)
#define MG_NUM_COLS (80)


// temp
//#define NO_STARTUP
