#include	"compiler.h"
#include	"dosio.h"
#include	"global.h"
#include	"shell2.h"

static	char	curpath[MAX_PATH] = "/";


// ----

void dosio_init(void) { }
void dosio_term(void) { }

#if defined(USE_FILESVR)
static const char filesvr[] = USE_FILESVR;
#endif

typedef struct {
#if defined(USE_FILESVR)
	dword	memid;
	GLOBALH	ghdl;
#endif
	dword	filesize;
	dword	filepos;
	char	*ptr;
} _FILEHDL, *FILEHDL;


FILEH file_open_rb(const char *filename) {

#if defined(USE_FILESVR)
	dword		tid;
    MessageInfo	msg;
	_FILEHDL	fh;
	FILEHDL		ret;

	tid = syscall_lookup_main_thread(filesvr);
	if (tid == (dword)-1) {
		goto fope_err1;
	}
	memset(&msg, 0, sizeof(msg));
	msg.header = MSG_FILE_READ_DATA;
//	msg.arg1 = 0;
	file_cpyname(msg.str, filename, sizeof(msg.str));
	syscall_send(tid, &msg);
	shell_phase |= PHASE_WAITFILESVR;
	while(1) {
		if (!syscall_receive(&msg)) {
			if ((msg.header == MSG_RESULT_OK) &&
				(msg.arg1 == MSG_FILE_READ_DATA)) {
				break;
			}
			defmsgdispatch(&msg);
		}
		else {
			syscall_mthread_yeild_message();
		}
	}
	shell_phase &= ~PHASE_WAITFILESVR;
	if (msg.arg2 == 0) {
		goto fope_err1;
	}
	fh.memid = msg.arg2;
	fh.filesize = msg.arg3;
	fh.filepos = 0;
	fh.ghdl = globalalloc(fh.memid);
	if (fh.ghdl == NULL) {
		goto fope_err2;
	}
	fh.ptr = (char *)globallock(fh.ghdl);
	if (fh.ptr == NULL) {
		goto fope_err3;
	}
	ret = (FILEHDL)malloc(sizeof(_FILEHDL));
	if (ret == NULL) {
		goto fope_err4;
	}
	*ret = fh;
	return((FILEH)ret);

fope_err4:
	globalunlock(fh.ghdl);

fope_err3:
	globalfree(fh.ghdl);

fope_err2:
	memset(&msg, 0, sizeof(msg));
	msg.header = MSG_DISPOSE_HANDLE;
	msg.arg1 = fh.memid;
	syscall_send(tid, &msg);

fope_err1:
	return(FILEH_INVALID);

#else
	char	path[MAX_PATH];
	dword	filesize;
	int		r;
	FILEHDL	ret;
	dword	readsize;

	file_cpyname(path, filename, sizeof(path));
	r = syscall_file_open(path, FILE_OPEN_READ, &filesize);
	if (r) {
		goto fope_err1;
	}
	ret = (FILEHDL)malloc(sizeof(_FILEHDL) + filesize);
	if (ret == NULL) {
		goto fope_err2;
	}
	ret->filesize = filesize;
	ret->filepos = 0;
	ret->ptr = (char *)(ret + 1);
	if (syscall_file_read((char *)(ret + 1), filesize, &readsize) != 0) {
		goto fope_err3;
	}
	syscall_file_close();
	return((FILEH)ret);

fope_err3:
	free(ret);

fope_err2:
	syscall_file_close();

fope_err1:
	return(FILEH_INVALID);
#endif
}

long file_seek(FILEH handle, long pointer, int method) {

	long	ret;

	if (handle == NULL) {
		return(-1);
	}
	switch(method) {
		case FSEEK_SET:
			ret = 0;
			break;

		case FSEEK_CUR:
			ret = ((FILEHDL)handle)->filepos;
			break;

		case FSEEK_END:
			ret = ((FILEHDL)handle)->filesize;
			break;

		default:
			return(-1);
	}
	ret += pointer;
	if (ret < 0) {
		ret = 0;
	}
	else if ((dword)ret > ((FILEHDL)handle)->filesize) {
		ret = ((FILEHDL)handle)->filesize;
	}
	((FILEHDL)handle)->filepos = ret;
	return(ret);
}

UINT file_read(FILEH handle, void *data, UINT length) {

	UINT	r;

	if (handle == NULL) {
		return(0);
	}
	r = ((FILEHDL)handle)->filesize - ((FILEHDL)handle)->filepos;
	r = min(r, length);
	if (r) {
		memcpy(data, ((FILEHDL)handle)->ptr + ((FILEHDL)handle)->filepos, r);
		((FILEHDL)handle)->filepos += r;
	}
	return(r);
}

void file_close(FILEH handle) {

	if (handle) {
#if defined(USE_FILESVR)
		dword tid;
    	MessageInfo	msg;
		globalunlock(((FILEHDL)handle)->ghdl);
		globalfree(((FILEHDL)handle)->ghdl);
		tid = syscall_lookup_main_thread(filesvr);
		if (tid != (dword)-1) {
			msg.header = MSG_DISPOSE_HANDLE;
			msg.arg1 = ((FILEHDL)handle)->memid;
			syscall_send(tid, &msg);
		}
#endif
		free(handle);
	}
}

UINT file_getsize(FILEH handle) {

	if (handle) {
		return(((FILEHDL)handle)->filesize);
	}
	return(0);
}


static BOOL isabsolutepath(const char *path) {

	if (path[0] == '/') {
		return(TRUE);
	}
	return(FALSE);
}

static char *getname(const char *path, char *buf, int size) {

const char	*p;
	int		leng;

	p = milstr_chr(path, '/');
	if (p == NULL) {
		milstr_ncpy(buf, path, size);
	}
	else {
		leng = (int)(p - path);
		leng = min(leng, size - 1);
		if (leng > 0) {
			memcpy(buf, path, leng);
		}
		if (leng >= 0) {
			buf[leng] = '\0';
		}
	}
	return((char *)p);
}

void file_getfullpath(char *path, const char *file, int size) {

	char	name[MAX_PATH];

	if ((file) && (isabsolutepath(file))) {
		milstr_ncpy(path, file, size);
	}
	else {
		milstr_ncpy(path, curpath, size);
		while(file) {
			while(file[0] == '/') {
				file++;
			}
			file = getname(file, name, sizeof(name));
			if (!milstr_cmp(name, ".")) {
			}
			else if (!milstr_cmp(name, "..")) {
				file_cutseparator(path);
				file_cutname(path);
			}
			else {
				file_setseparator(path, size);
				file_catname(path, name, size);
			}
		}
	}
}

BOOL file_setcd(const char *path) {

	char	cur[MAX_PATH];

	file_getfullpath(cur, path, sizeof(cur));
	if (syscall_cd(cur) == 0) {
		file_cpyname(curpath, cur, sizeof(curpath));
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}


FLISTH file_list1st(const char *dir, FLINFO *fli) {

	char	path[MAX_PATH];
	int		size;

	if (syscall_cd((char *)dir) != 0) {
		goto fl1_err1;
	}
	if (syscall_dir_open() != 0) {
		goto fl1_err1;
	}
	if (syscall_dir_read(path, &size) != 0) {
		goto fl1_err2;
	}
	if (fli) {
		file_cpyname(fli->path, path, sizeof(fli->path));
	}
	return((FLISTH)1);

fl1_err2:
	syscall_dir_close();

fl1_err1:
	syscall_cd(curpath);
	return(FLISTH_INVALID);
}

BOOL file_listnext(FLISTH hdl, FLINFO *fli) {

	char	path[MAX_PATH];
	int		size;

	if (syscall_dir_read(path, &size) == 0) {
		if (fli) {
			file_cpyname(fli->path, path, sizeof(fli->path));
		}
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

void file_listclose(FLISTH hdl) {

	syscall_dir_close();
	syscall_cd(curpath);
}


char *file_getname(const char *path) {

const char	*ret;

	ret = path;
	while(*path != '\0') {
		if (*path == '/') {
			ret = path + 1;
		}
		path++;
	}
	return((char *)ret);
}

void file_cutname(char *path) {

	char 	*p;

	p = file_getname(path);
	p[0] = '\0';
}

char *file_getext(const char *path) {

const char	*p;
const char	*q;

	p = file_getname(path);
	q = NULL;

	while(*p != '\0') {
		if (*p == '.') {
			q = p + 1;
		}
		p++;
	}
	if (!q) {
		q = p;
	}
	return((char *)q);
}

void file_cutext(char *path) {

	char	*p;
	char	*q;

	p = file_getname(path);
	q = NULL;

	while(*p != '\0') {
		if (*p == '.') {
			q = p;
		}
		p++;
	}
	if (q) {
		*q = '\0';
	}
}

void file_cutseparator(char *path) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos > 0) && (path[pos] == '/')) {
		path[pos] = '\0';
	}
}

void file_setseparator(char *path, int maxlen) {

	int		pos;

	pos = strlen(path) - 1;
	if ((pos < 0) ||
		(path[pos] == '/') ||
		((pos + 2) >= maxlen)) {
		return;
	}
	path[++pos] = '/';
	path[++pos] = '\0';
}

