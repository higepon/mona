
typedef	void *	FILEH;
#define		FILEH_INVALID		((FILEH)0)

typedef	dword	FLISTH;
#define		FLISTH_INVALID		((FLISTH)0)

#define		MAX_PATH			32

enum {
	FSEEK_SET	= 0,
	FSEEK_CUR	= 1,
	FSEEK_END	= 2
};

enum {
	FILEATTR_READONLY	= 0x01,
	FILEATTR_HIDDEN		= 0x02,
	FILEATTR_SYSTEM		= 0x04,
	FILEATTR_VOLUME		= 0x08,
	FILEATTR_DIRECTORY	= 0x10,
	FILEATTR_ARCHIVE	= 0x20
};

typedef struct {
	char	path[MAX_PATH];
} FLINFO;


											// DOSIO:ä÷êîÇÃèÄîı
void dosio_init(void);
void dosio_term(void);
											// ÉtÉ@ÉCÉãëÄçÏ
FILEH file_open_rb(const char *path);
long file_seek(FILEH handle, long pointer, int method);
UINT file_read(FILEH handle, void *data, UINT length);
void file_close(FILEH handle);
UINT file_getsize(FILEH handle);

void file_getfullpath(char *path, const char *file, int size);
BOOL file_setcd(const char *path);

FLISTH file_list1st(const char *dir, FLINFO *fli);
BOOL file_listnext(FLISTH hdl, FLINFO *fli);
void file_listclose(FLISTH hdl);

#define	file_cpyname(a, b, c)	milank_ncpy(a, b, c)
#define	file_catname(a, b, c)	milank_ncat(a, b, c)
#define	file_cmpname(a, b)		milank_cmp(a, b)
char *file_getname(const char *path);
void file_cutname(char *path);
char *file_getext(const char *path);
void file_cutext(char *path);
void file_cutseparator(char *path);
void file_setseparator(char *path, int maxlen);

