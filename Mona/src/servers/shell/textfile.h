
typedef struct {
	long	fh;
	long	fhpos;
	UINT	buffersize;
	UINT	flag;
	UINT	pos;
	UINT	remain;
} _TEXTFILE, *TEXTFILEH;


#ifdef __cplusplus
extern "C" {
#endif

TEXTFILEH textfile_open(const char *filename, UINT buffersize = 0x400);
BOOL textfile_read(TEXTFILEH fh, char *buffer, UINT size);
void textfile_close(TEXTFILEH fh);

#ifdef __cplusplus
}
#endif

