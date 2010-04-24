/* config.h.  Generated from config.h.in by configure.  */
#ifndef CONFIG_H_SEEN
#define CONFIG_H_SEEN
/*
 * Configuration for w3m
 */
#define JA 0
#define EN 1

/* Name of package */
/* defined at command line #define PACKAGE "w3m" */

#define HELP_FILE "w3mhelp-w3m_en.html"
#define HELP_CGI     "w3mhelp"
#define W3MCONFIG    "w3mconfig"
#define IMGDISPLAY   "w3mimgdisplay"
#define XFACE2XPM    "xface2xpm"

#define BOOKMARK     "bookmark.html"
#define CONFIG_FILE  "config"
#define KEYMAP_FILE  "keymap"
#define MENU_FILE    "menu"
#define MOUSE_FILE   "mouse"
#define COOKIE_FILE  "cookie"
#define HISTORY_FILE "history"

#define PASSWD_FILE	RC_DIR "/passwd"
#define PRE_FORM_FILE	RC_DIR "/pre_form"
#define USER_MAILCAP	RC_DIR "/mailcap"
#define SYS_MAILCAP	CONF_DIR "/mailcap"
#define USER_MIMETYPES	"~/.mime.types"
#define SYS_MIMETYPES	ETC_DIR "/mime.types"
#define USER_URIMETHODMAP	RC_DIR "/urimethodmap"
#define SYS_URIMETHODMAP	CONF_DIR "/urimethodmap"

#define DEF_SAVE_FILE "index.html"

/* User Configuration */
#define DISPLAY_CHARSET WC_CES_US_ASCII
#define SYSTEM_CHARSET WC_CES_US_ASCII
#define DOCUMENT_CHARSET WC_CES_US_ASCII
/* mumurik
#define DISPLAY_CHARSET WC_CES_UTF_8
#define SYSTEM_CHARSET WC_CES_UTF_8
#define DOCUMENT_CHARSET WC_CES_UTF_8
*/
// mumurik #define USE_M17N 1
// mumurik #define USE_UNICODE 1
#define W3M_LANG EN
#define LANG W3M_LANG

/* Define to 1 if translation of program messages to the user's
   native language is requested. */
/* #undef ENABLE_NLS */

#define USE_COLOR 1
#define USE_ANSI_COLOR 1
#define USE_BG_COLOR 1
/* #undef USE_MIGEMO */
#define USE_MARK
#define USE_MOUSE 1
/* #undef USE_GPM */
/* #undef USE_SYSMOUSE */
#define USE_MENU 1
#define USE_COOKIE 1
/* #undef USE_DIGEST_AUTH */
/* #undef USE_SSL */
/* #undef USE_SSL_VERIFY */
/* #define USE_HELP_CGI 1 */
#undef USE_HELP_CGI
/* #define USE_EXTERNAL_URI_LOADER 1 */
#undef USE_EXTERNAL_URI_LOADER
/* #define USE_W3MMAILER 1 */
#undef USE_W3MMAILER
/* #define USE_NNTP 1 */
#undef USE_NNTP
/* #undef USE_GOPHER */
/* #undef USE_ALARM */
/* #define USE_IMAGE 1 */
#undef USE_IMAGE
/* #undef USE_W3MIMG_X11 */
/* #undef USE_W3MIMG_FB */
/* #undef W3MIMGDISPLAY_SETUID */
/* #undef USE_IMLIB */
/* #undef USE_GDKPIXBUF */
/* #undef USE_GTK2 */
/* #undef USE_IMLIB2 */
/* #define USE_XFACE 1 */
#undef USE_XFACE
#undef USE_DICT
/* #define USE_HISTORY 1 */
#undef USE_HISTORY
/* #undef FORMAT_NICE */
#define ID_EXT
/* #undef CLEAR_BUF */
/* #undef INET6 */
/* #undef HAVE_OLD_SS_FAMILY */
#define USE_EGD
#define ENABLE_REMOVE_TRAILINGSPACES
/* #undef MENU_THIN_FRAME */
/* #undef USE_RAW_SCROLL */
/* #undef TABLE_EXPAND */
/* #undef TABLE_NO_COMPACT */
#define NOWRAP
#define MATRIX

#define DEF_EDITOR "/usr/bin/vi"
#define DEF_MAILER "/usr/bin/mail"
#define DEF_EXT_BROWSER "/usr/bin/mozilla"

/* fallback viewer. mailcap override these configuration */
#define DEF_IMAGE_VIEWER	"display"
#define DEF_AUDIO_PLAYER	"showaudio"

/* for USE_MIGEMO */
#define DEF_MIGEMO_COMMAND "migemo -t egrep /usr/local/share/migemo/migemo-dict"

#define USE_BINMODE_STREAM 1
/* #undef HAVE_TERMIOS_H */
/* #undef HAVE_TERMIO_H */
/* #undef HAVE_SGTTY_H */
#define HAVE_DIRENT_H 1
#define HAVE_LOCALE_H 1
#define HAVE_STDINT_H 1
#define HAVE_INTTYPES_H 1
/* #define SIZEOF_LONG_LONG 8 */
#if SIZEOF_LONG_LONG > 0
typedef long long clen_t;
#else
typedef long clen_t;
#endif
/* #define HAVE_STRTOLL 1 */
#undef HAVE_STROQ
/* #define HAVE_ATOLL 1 */
#undef HAVE_ATOQ
/* #define HAVE_STRCASECMP 1 */
#undef HAVE_STRCASESTR
#define HAVE_STRCHR 1
#define HAVE_STRERROR 1
#define HAVE_SYS_ERRLIST 1
/* #undef HAVE_BCOPY */
/* #undef HAVE_WAITPID */
/* #undef HAVE_WAIT3 */
#define HAVE_STRFTIME 1
/* #define HAVE_GETCWD 1 */
#undef HAVE_GETWD
/* #undef HAVE_SYMLINK */
/* #undef HAVE_READLINK */
/* #undef HAVE_LSTAT */
/* #undef HAVE_SETENV */
#define HAVE_PUTENV 1
/* #undef HAVE_SRAND48 */
/* #undef HAVE_SRANDOM */
/* #undef HAVE_GETPASSPHRASE */
#define HAVE_CHDIR 1
/* #undef HAVE_SETPGRP */
#define HAVE_SETLOCALE 1
/* #undef HAVE_LANGINFO_CODESET */

#define SETPGRP_VOID 1
#ifdef HAVE_SETPGRP
#ifdef SETPGRP_VOID
#define SETPGRP() setpgrp()
#else
#define SETPGRP() setpgrp(0,0)
#endif
#else /* no HAVE_SETPGRP; OS/2 EMX */
#define SETPGRP() setpgid(0, 0)
#endif
#define HAVE_FLOAT_H 1
/* #undef HAVE_SYS_SELECT_H */

#define HAVE_SIGSETJMP 1

#define RETSIGTYPE void
typedef RETSIGTYPE MySignalHandler;
#define SIGNAL_ARG int _dummy	/* XXX */
#define SIGNAL_ARGLIST 0	/* XXX */
#define SIGNAL_RETURN return

#ifdef HAVE_SIGSETJMP
/* mumurik #ifdef __MINGW32_VERSION */
#if defined(__MINGW32_VERSION) || defined(MONA)
# define SETJMP(env) setjmp(env)
# define LONGJMP(env,val) longjmp(env, val)
# define JMP_BUF jmp_buf
#else
# define SETJMP(env) sigsetjmp(env,1)
# define LONGJMP(env,val) siglongjmp(env,val)
# define JMP_BUF sigjmp_buf
#endif /* __MINGW32_VERSION */
#else
# define SETJMP(env) setjmp(env)
# define LONGJMP(env,val) longjmp(env)
# define JMP_BUF jmp_buf
#endif

#ifndef HAVE_SRAND48
#ifdef HAVE_SRANDOM
#define srand48 srandom
#define lrand48 random
#else /* HAVE_SRANDOM */
#define USE_INCLUDED_SRAND48
#endif /* HAVE_SRANDOM */
#endif

#ifndef HAVE_LSTAT
#define lstat stat
#endif

#if defined( __CYGWIN32__ ) && !defined( __CYGWIN__ )
#define __CYGWIN__
#endif

#if defined( __CYGWIN__ ) || defined( __EMX__ )
#define SUPPORT_DOS_DRIVE_PREFIX 1
#endif

#if defined( __CYGWIN__ )
#define SUPPORT_NETBIOS_SHARE 1
#define SUPPORT_WIN9X_CONSOLE_MBCS 1
#endif

#if defined(__DJGPP__)
#define DEFAULT_TERM	"dosansi"
#else
#define DEFAULT_TERM	0	/* XXX */
#endif

#if defined(__EMX__) /* use $extension? */
#define GUNZIP_CMDNAME  "gzip"
#define BUNZIP2_CMDNAME "bzip2"
#define INFLATE_CMDNAME  "inflate.exe"
#define W3MBOOKMARK_CMDNAME	"w3mbookmark.exe"
#define W3MHELPERPANEL_CMDNAME	"w3mhelperpanel.exe"
#define DEV_NULL_PATH	"nul"
#define DEV_TTY_PATH	"con"
#define CGI_EXTENSION	".cmd"
#define USE_PATH_ENVVAR
#else
#define GUNZIP_CMDNAME  "gunzip"
#define BUNZIP2_CMDNAME "bunzip2"
#define INFLATE_CMDNAME	"inflate"
#define W3MBOOKMARK_CMDNAME	"w3mbookmark"
#define W3MHELPERPANEL_CMDNAME	"w3mhelperpanel"
#define DEV_NULL_PATH	"/dev/null"
#define DEV_TTY_PATH	"/dev/tty"
#define CGI_EXTENSION	".cgi"
#endif

#define PATH_SEPARATOR	':'
#define GUNZIP_NAME  "gunzip"
#define BUNZIP2_NAME "bunzip2"
#define INFLATE_NAME "inflate"

#ifdef __MINGW32_VERSION
#define SIGKILL SIGTERM
#define S_IXGRP 0
#define S_IXOTH 0
#define S_IRWXG 0
#define S_IRWXO 0
#endif /* __MINGW32_VERSION */

#endif /* CONFIG_H_SEEN */
