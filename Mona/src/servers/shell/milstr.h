
// ‘å•¶š¬•¶š‚ğ“¯ˆê‹‚µ‚Ä”äŠr
// ret 0:ˆê’v
int milank_cmp(const char *str, const char *cmp);
int milsjis_cmp(const char *str, const char *cmp);
int mileuc_cmp(const char *str, const char *cmp);

// ‘å•¶š¬•¶š‚ğ “¯ˆê‹‚µ‚Äcmp‚Ìƒkƒ‹‚Ü‚Å”äŠr
// ret 0:ˆê’v
int milank_memcmp(const char *str, const char *cmp);
int milsjis_memcmp(const char *str, const char *cmp);
int mileuc_memcmp(const char *str, const char *cmp);

// str[pos]‚ªŠ¿š‚PƒoƒCƒg–Ú‚©‚Ç‚¤‚©c
int milsjis_kanji1st(const char *str, int pos);
int mileuc_kanji1st(const char *str, int pos);

// str[pos]‚ªŠ¿š‚QƒoƒCƒg–Ú‚©‚Ç‚¤‚©c
int milsjis_kanji2nd(const char *str, int pos);
int mileuc_kanji2nd(const char *str, int pos);

// maxlen•ª‚¾‚¯•¶š—ñ‚ğƒRƒs[
void milank_ncpy(char *dst, const char *src, int maxlen);
void milsjis_ncpy(char *dst, const char *src, int maxlen);
void mileuc_ncpy(char *dst, const char *src, int maxlen);

// maxlen•ª‚¾‚¯•¶š—ñ‚ğƒLƒƒƒbƒg
void milank_ncat(char *dst, const char *src, int maxlen);
void milsjis_ncat(char *dst, const char *src, int maxlen);
void mileuc_ncat(char *dst, const char *src, int maxlen);

// •¶š‚ğŒŸõ
char *milank_chr(const char *str, int c);
char *milsjis_chr(const char *str, int c);
char *mileuc_chr(const char *str, int c);


// 0~9, A~Z ‚Ì‚İ‚ğ‘å•¶š¬•¶š‚ğ“¯ˆê‹‚µ‚Ä”äŠr
// ret 0:ˆê’v
int milstr_extendcmp(const char *str, const char *cmp);

// Ÿ‚ÌŒê‚ğæ“¾
char *milstr_nextword(const char *str);

// HEX2INT
long milstr_solveHEX(const char *str);

// STRLIST
char *milstr_list(const char *lststr, UINT pos);


// ---- macros

#if defined(OSLANG_SJIS)
#define	ISKANJI1ST(c)			((((c ^ 0x20) - 0xa1) & 0xff) < 0x3c)
#define	milstr_cmp(s, c)		milsjis_cmp(s, c)
#define	milstr_memcmp(s, c)		milsjis_memcmp(s, c)
#define	milstr_kanji1st(s, p)	milsjis_kanji1st(s, p)
#define	milstr_kanji2nd(s, p)	milsjis_kanji2nd(s, p)
#define	milstr_ncpy(d, s, l)	milsjis_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	milsjis_ncat(d, s, l)
#define	milstr_chr(s, c)		milsjis_chr(s, c)
#elif defined(OSLANG_EUC)
#define	ISKANJI1ST(c)			(((c - 0xa1) & 0xff) < 0x5d)
#define	milstr_cmp(s, c)		mileuc_cmp(s, c)
#define	milstr_memcmp(s, c)		mileuc_memcmp(s, c)
#define	milstr_kanji1st(s, p)	mileuc_kanji1st(s, p)
#define	milstr_kanji2nd(s, p)	mileuc_kanji2nd(s, p)
#define	milstr_ncpy(d, s, l)	mileuc_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	mileuc_ncat(d, s, l)
#define	milstr_chr(s, c)		mileuc_chr(s, c)
#else
#define	ISKANJI1ST(c)			(0)
#define	milstr_cmp(s, c)		milank_cmp(s, c)
#define	milstr_memcmp(s, c)		milank_memcmp(s, c)
#define	milstr_kanji1st(s, p)	(0)
#define	milstr_kanji2nd(s, p)	(0)
#define	milstr_ncpy(d, s, l)	milank_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	milank_ncat(d, s, l)
#define	milstr_chr(s, c)		milank_chr(s, c)
#endif

