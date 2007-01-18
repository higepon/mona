//	"bim2bin"
//	Copyright(C) 2004 H.Kawai
//	usage : bim2bin [malloc:#] [mmarea:#] in:(path) out:(path)
//		[-bim | -exe512 | -bin0 | -data | -restore | -osacmp] [-simple | -l2d3 | -tek0 | -tek1 | -tek2]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HOST_MINGW
  #include <Windows.h>
#endif //HOST_MINGW

#define NOWARN			1
#define TEK1_NOWARN		1

#define	get32(ptr)		*((int *) (ptr))

#define	SIZEOFBUF		(16 * 1024 * 1024)
#define	SIZEOFOVERBUF	(16 * 1024 * 1024)
#define	SIZEOFSUBBUF	(SIZEOFOVERBUF * 4 / 4)

//#define DEBUGMSG		1

/* +0x10 : 総DS/SSサイズ */
/* +0x14 : file */
/* +0x18 : reserve */
/* +0x1c : reserve */
/* +0x20 : static start */
/* +0x24 : static uint8_ts */

typedef unsigned char UCHAR;

#define TEK1_MAXLEN			31	/* 32+196=244 */

#define TEK1_BT_NODES0	4400 * 2
#define TEK1_BT_NODES1	65536 * 2
	/* maxdis:1MB用(10MBくらい必要になる) */

static unsigned char *putb_buf, *putb_overbuf;
static int putb_ptr;
static unsigned char putb_uint8_t, putb_count = 8;
static UCHAR *tek1_s7ptr;
static int complev = -1;
static FILE *hint = NULL;
static UCHAR tek2_table_tr0[256];

int lzcompress_l2d3(unsigned char *buf, int k, int i, int outlimit, int maxdis);
int lzcompress_tek0(int prm, unsigned char *buf, int k, int i, int outlimit, int maxdis);
int lzcompress_tek1(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int bsiz, int flags, int opt, int prm, int maxdis, int submaxdis);
//int lzcompress_tek3(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int bsiz, int flags, int opt, int prm, int maxdis, int submaxdis);
int lzcompress_tek5(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, UCHAR *argv0, UCHAR *eopt, int bsiz, int flags, int opt, int prm, int maxdis, int submaxdis);
int lzrestore_l2d3(unsigned char *buf, int k, int i, int outlimit);
int lzrestore_tek0(unsigned char *buf, int k, int i, int outlimit);
int tek1_lzrestore_tek1(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int flags);
void osarjc(int siz, UCHAR *p, int mode);

//int tek1_lzrestore_stk5(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf);
int tek_lzrestore_tek5(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, UCHAR *mclp);
int tek_conv_tek5(int csiz, UCHAR *src, int osiz, UCHAR *tmp, UCHAR *dst, UCHAR *str_eprm);

void tek1_puts7s(unsigned int i);
UCHAR *tek1_puts7sp(UCHAR *p, unsigned int i);
void tek1_puts7(unsigned int i);
unsigned int tek1_getnum_s7s(UCHAR **pp);

#define TEK1_BT_MAXLEV		10
#define TEK1_BT_NODESIZ		32	/* 必ず2の倍数 */

/* 登録したい数/16のノード1 */
/* 登録したい数/256+登録したい数/4096+登録したい数/65536+...のノード1 */
/* 1/256(16/15)=1/240 だから登録したい数/240のノード0 */
/* 1MBだとすると、ノード1は65536個、ノード0は4400個 */

struct STR_BT_NODE0 { /* 260バイト */
	int nodes, skiplen;
	void *pkey[TEK1_BT_NODESIZ];
	void *node[TEK1_BT_NODESIZ];
	 /* pkeyは各ノードの最低値（隣のノードの最大値よりも小さければ、最低値よりさらに小さくてもよい） */
};

struct STR_BT_NODE1 { /* 132バイト */
	int nodes, skiplen;
	void *pkey[TEK1_BT_NODESIZ];
};

struct STR_BT_HANDLE {
	int pos[TEK1_BT_MAXLEV + 1];
	void *node[TEK1_BT_MAXLEV];
	void *pkey;
};

struct STR_BTREE {
	int level;
	void *top;
	struct STR_BT_NODE0 *free0;
	struct STR_BT_NODE1 *free1;
};

void init_btree(struct STR_BTREE *btree);
void add_free0(struct STR_BTREE *btree, struct STR_BT_NODE0 *newnode);
void add_free1(struct STR_BTREE *btree, struct STR_BT_NODE1 *newnode);
int matchlen_bt(UCHAR *a, UCHAR *b, UCHAR *e);
void search0(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle, UCHAR *s, UCHAR *s1);
int search_back(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle);
int search_next(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle);
int insert_val(struct STR_BTREE *btree, UCHAR *s, UCHAR *s1);
int delete_val(struct STR_BTREE *btree, UCHAR *s, UCHAR *s1);

UCHAR *getnum0(UCHAR *p, UCHAR *p1, int *pi)
{
	int i = 0, j, base = 10;
//	p = skipspace(p);
	if (p >= p1)
		goto fin;
	if (*p == '0') {
		p++;
		if (*p == 'X' || *p == 'x') {
			base = 16;
			p++;
		} else if (*p == 'O' || *p == 'o') {
			base = 8;
			p++;
		}
	}
	p--;
	for (;;) {
		p++;
		if (p >= p1)
			goto fin;
		if (*p == '_')
			continue;
		j = 99;
		if ('0' <= *p && *p <= '9')
			j = *p - '0';
		if ('A' <= *p && *p <= 'F')
			j = *p - 'A' + 10;
		if ('a' <= *p && *p <= 'f')
			j = *p - 'a' + 10;
		if (base <= j)
			break;
		i = i * base + j;
	}
	if (p >= p1)
		goto fin;
	if (*p == 'k' || *p == 'K') {
		i *= 1024;
	//	p++;
	} else if (*p == 'm' || *p == 'M') {
		i *= 1024 * 1024;
	//	p++;
	} else if (*p == 'g' || *p == 'G') {
		i *= 1024 * 1024 * 1024;
	//	p++;
	}
fin:
	*pi = i;
	return p;
}

int getnum(UCHAR *p)
{
	int i;
	getnum0(p, p + 100, &i);
	return i;
}

UCHAR *glb_str_eprm = NULL;

int main(int argc, char **argv)
{
	int mallocsize = 32 * 1024, reserve = 0, stacksize, datasize, worksize;
	int i, j, filesize, compress = 7, outtype = 0, prm0 = 12, maxdis = -1, bsiz = -1, submaxdis = 16 * 1024;
	FILE *fp; /* maxdis = 32 * 1024, bsiz = 32 * 1024 */
	UCHAR *filepath[2], c, *buf, *overbuf, *s7ptr, *eopt = NULL, *argv0 = (UCHAR *)argv[0];
	int code_end, data_begin, entry, opt = -1, rjc = -1;
	static unsigned char signature[15] = "\xff\xff\xff\x01\x00\x00\x00OSASKCMP";

	#if (defined(NOWARN))
		j = 0;
	#endif

	filepath[0] = filepath[1] = NULL;
	buf = malloc(SIZEOFBUF);
	overbuf = malloc(SIZEOFOVERBUF);

	if (argc <= 2) {
		fprintf(stdout,
			"\"bim2bin\" executable binary maker for GUIGUI00 (OSASK API)\n"
			"\tcopyright (C) 2004 H.Kawai\n"
			"usage : \n"
			">bim2bin [malloc:#] [mmarea:#] in:(file) out:(file)\n"
			" [-bim | -exe512 | -bin0 | -data | -restore | -osacmp]\n"
			" [-simple | -l2d3 | -tek0 | -tek1 | -tek2 | -tek5]\n"
		);
		return 1;
	}

	/* パラメーター解析 */
	for (argv++, i = 1; i < argc; argv++, i++) {
		char *s = *argv;
		if (strncmp(s, "malloc:", 7) == 0)
			mallocsize = getnum((UCHAR *)s + 7);
		else if (strncmp(s, "file:", 5) == 0)
			reserve = getnum((UCHAR *)s + 5) | 0x01;
		else if (strcmp(s, "-simple") == 0)
			compress = -1;
		else if (strncmp(s, "input:", 6) == 0)
			filepath[0] = (UCHAR *)s + 6;
		else if (strncmp(s, "output:", 7) == 0)
			filepath[1] = (UCHAR *)s + 7;
		else if (strcmp(s, "-l2d3") == 0)
			compress = 1;
		else if (strcmp(s, "-tek0") == 0)
			compress = 2;
		else if (strcmp(s, "-tek1") == 0)
			compress = 3;
		else if (strcmp(s, "-tek2") == 0)
			compress = 4;
	//	else if (strcmp(s, "-tek3") == 0)
	//		compress = 5;
	//	else if (strcmp(s, "-tek4") == 0)
	//		compress = 6;
		else if (strcmp(s, "-tek5") == 0)
			compress = 7;
		else if (strcmp(s, "-bim") == 0)
			outtype = 0;
		else if (strcmp(s, "-exe512") == 0)
			outtype = 1;
		else if (strcmp(s, "-data") == 0)
			outtype = 2;
		else if (strcmp(s, "-restore") == 0)
			outtype = 3;
		else if (strncmp(s, "prm0:", 5) == 0)
			prm0 = getnum((UCHAR *)s + 5);
		else if (strncmp(s, "bsiz:", 5) == 0)
			bsiz = getnum((UCHAR *)s + 5);
		else if (strncmp(s, "BS:", 3) == 0)
			bsiz = getnum((UCHAR *)s + 3);
		else if (strncmp(s, "in:", 3) == 0)
			filepath[0] = (UCHAR *)s + 3;
		else if (strncmp(s, "out:", 4) == 0)
			filepath[1] = (UCHAR *)s + 4;
		else if (strncmp(s, "mmarea:", 7) == 0)
			reserve = getnum((UCHAR *)s + 7) | 0x01;
		else if (strcmp(s, "-bin0") == 0)
			outtype = 4;
		else if (strcmp(s, "-osacmp") == 0)
			outtype = 5;
		else if (strncmp(s, "opt:", 4) == 0)
			opt = getnum((UCHAR *)s + 4);
		else if (strncmp(s, "rjc:", 4) == 0)
			rjc = getnum((UCHAR *)s + 4);
		else if (strncmp(s, "eopt:", 5) == 0)
			eopt = (UCHAR *)s + 5;
		else if (strncmp(s, "eprm:", 5) == 0)
			glb_str_eprm = (UCHAR *)s + 5;
		else if (strncmp(s, "maxdis:", 7) == 0)
			maxdis = getnum((UCHAR *)s + 7);
		else if (strncmp(s, "MD:", 3) == 0)
			maxdis = getnum((UCHAR *)s + 3);
		else if (strncmp(s, "submaxdis:", 10) == 0)
			submaxdis = getnum((UCHAR *)s + 10);
		else if (strncmp(s, "SD:", 3) == 0)
			submaxdis = getnum((UCHAR *)s + 3);
		else if (strncmp(s, "hint:", 5) == 0)
			hint = fopen(s + 5, "rb");
		else if (strncmp(s, "clv:", 4) == 0) {
		//	static UCHAR table_clv[10] = { }; /* 10段階 */
		//	5が十分、3が中間、1がデフォルト、0がもっとも弱い、9が最強(99)
		//	10-99 -> 00-88, 90
			complev = getnum((UCHAR *)s + 4);
			if (complev > 9)
				complev = complev / 10 - 1;
		} else
			fprintf(stderr, "Commnad line error : %s ... skiped\n", s);
	}
	if (complev == -1)
		complev = 4;
	if (maxdis == -1) {
		maxdis = 0;
		if (compress <= 2)
			maxdis = 32 * 1024;
	}
	if (bsiz == -1)
		bsiz = 0;

	if (maxdis > 2 * 1024 * 1024)
		maxdis = 2 * 1024 * 1024;
	if (maxdis == 0)
		maxdis = 2 * 1024 * 1024;
	if (bsiz > 8 * 1024 * 1024)
		bsiz = 8 * 1024 * 1024;
	if (bsiz == 0)
		bsiz = 8 * 1024 * 1024;
	if (bsiz < 512)
		bsiz = 512;

	fp = NULL;
	if (filepath[0])
		fp = fopen((char *)filepath[0], "rb");
	if (fp == NULL) {
		fprintf(stderr, "Commnad line error : can't open input file\n");
		return 1;
	}
	filesize = fread(buf, 1, SIZEOFBUF, fp);
	fclose(fp);
	if (rjc == -1 || rjc == 1)
		osarjc(filesize, buf, 1);
	if (outtype == 2 /* data */) {
		putb_overbuf = overbuf;
		putb_ptr = 0;
		if (compress < 3) {
			if (compress == 1)
				lzcompress_l2d3(buf, filesize, 0, SIZEOFOVERBUF - 8, maxdis);
			if (compress == 2)
				lzcompress_tek0(prm0, buf, filesize, 0, SIZEOFOVERBUF - 8, maxdis);
			for (i = 0; i < putb_ptr; i++)
				buf[i] = overbuf[i];
			filesize = putb_ptr;
		}
		goto write;
	}

	if (outtype == 3 /* restore */) {
		c = 0;
		for (i = 0; i < 15; i++)
			c |= buf[i + 1] ^ signature[i];
		if (c) {
			for (i = 0; i < filesize; i++)
				overbuf[i] = buf[i];
		} else {
			if (buf[0] == 0x83)
				compress = 3;
			if (buf[0] == 0x85)
				compress = 4;
			if (buf[0] == 0x86)
				compress = 5;
			if (buf[0] == 0x89)
				compress = 7;
			if (buf[0] == 0x82)
				compress = 2;
			if (buf[0] == 0x81)
				compress = 1;
			j = 20;
			if (compress >= 3) {
				s7ptr = &buf[16];
				putb_ptr = tek1_getnum_s7s(&s7ptr);
				j = s7ptr - buf;
			}
			filesize -= j;
			for (i = 0; i < filesize; i++)
				overbuf[i] = buf[i + j];
			j = putb_ptr;
		}
		putb_overbuf = overbuf;
		putb_ptr = filesize;
		if (compress == 1)
			filesize = lzrestore_l2d3(buf, filesize, 0, SIZEOFOVERBUF - 8);
		if (compress == 2)
			filesize = lzrestore_tek0(buf, filesize, 0, SIZEOFOVERBUF - 8);
		if (3 <= compress && compress <= 7) {
			UCHAR *work = malloc(512 * sizeof (int));
			if (compress == 3)
				tek1_lzrestore_tek1(filesize, overbuf, j, buf, 512 * sizeof (int), work, 0);
			if (compress == 4)
				tek1_lzrestore_tek1(filesize, overbuf, j, buf, 512 * sizeof (int), work, 1);
			if (compress == 5)
				tek1_lzrestore_tek1(filesize, overbuf, j, buf, 512 * sizeof (int), work, 2);
			if (compress == 6)
				tek1_lzrestore_tek1(filesize, overbuf, j, buf, 512 * sizeof (int), work, 3);
			if (compress == 7)
				tek_lzrestore_tek5(filesize, overbuf, j, buf, NULL);
			filesize = j;
			free(work);
		}
		goto write;
	}

	if (outtype == 5) { /* osacmp */
		for (i = filesize - 1; i >= 0; i--)
			buf[i + 20] = buf[i];
		for (i = 0; i < 15; i++)
			buf[i + 1] = signature[i];
		if (compress == 1)
			buf[0] = 0x81;
		if (compress == 2)
			buf[0] = 0x82;
		if (compress == 3)
			buf[0] = 0x83;
		if (compress == 4)
			buf[0] = 0x85;
		if (compress == 5)
			buf[0] = 0x86;
		if (compress == 6)
			buf[0] = 0x87;
		if (compress == 7)
			buf[0] = 0x89;
		if (compress < 3) {
			buf[16] = filesize         & 0xff;
			buf[17] = (filesize >>  8) & 0xff;
			buf[18] = (filesize >> 16) & 0xff;
			buf[19] = (filesize >> 24) & 0xff;
			buf[filesize + 20] = 0x14;
			buf[filesize + 21] = 0x00;
			buf[filesize + 22] = 0x00;
			buf[filesize + 23] = 0x00;
			filesize += 24;
		}
		if (3 <= compress && compress <= 7) {
			tek1_s7ptr = &buf[16];
			tek1_puts7s(filesize);
			j = &buf[20] - tek1_s7ptr;
			if (j != 0) {
				for (i = 0; i < filesize; i++)
					buf[i + 20 - j] = buf[i + 20];
			}
			buf[filesize + 20 - j] = 0x14 - j;
			buf[filesize + 21 - j] = 0x00;
			buf[filesize + 22 - j] = 0x00;
			buf[filesize + 23 - j] = 0x00;
			filesize += 24 - j;
		}
		outtype = 4; /* bin0 */
	}
	if (outtype == 1 || outtype == 4) { /* exe512 | bin0 */
		if (outtype == 1 /* exe512 */) {
			/* ヘッダー(512バイト)のカット */
			filesize -= 512;
			for (i = 0; i < filesize; i++)
				buf[i] = buf[i + 512];
		}
		if (compress == -1) /* 単なるヘッダカット */
			goto write;
		/* スタティックデーターイメージ圧縮 */
		data_begin = get32(&buf[filesize - 4]);
		datasize = filesize - 4 - data_begin;
		if (compress < 3) {
			putb_overbuf = overbuf;
			putb_ptr = 0;
			if (compress == 1)
				lzcompress_l2d3(buf + data_begin, datasize, 0, SIZEOFOVERBUF - 8, maxdis);
			if (compress == 2)
				lzcompress_tek0(prm0, buf + data_begin, datasize, 0, SIZEOFOVERBUF - 8, maxdis);
			for (i = 0; i < putb_ptr; i++)
				buf[data_begin + i] = overbuf[i];
			filesize = data_begin + putb_ptr;
		} else {
			UCHAR *work = malloc(i = 257 * 1024 + (bsiz + 272) * 8);
			if (compress == 3)
				j = lzcompress_tek1(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 0, opt, prm0, maxdis, submaxdis);
			if (compress == 4)
				j = lzcompress_tek1(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 1, opt, prm0, maxdis, submaxdis);
		//	if (compress == 5)
		//		j = lzcompress_tek3(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 0, opt, prm0, maxdis, submaxdis);
		//	if (compress == 6)
		//		j = lzcompress_tek3(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 1, opt, prm0, maxdis, submaxdis);
			if (compress == 7)
				j = lzcompress_tek5(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, argv0, eopt, bsiz, 0, opt, prm0, maxdis, submaxdis);
			for (i = 0; i < j; i++)
				buf[data_begin + i] = overbuf[i];
			filesize = data_begin + j;
			free(work);
		}
		goto write;
	}

	if (reserve == 1)
		reserve = 0;
	code_end = get32(&buf[0x00]) + get32(&buf[0x04]);
	datasize = get32(&buf[0x0c]);
	data_begin = get32(&buf[0x10]);
	stacksize = get32(&buf[0x14]);
	entry = get32(&buf[0x18]);
	worksize = (stacksize + datasize + mallocsize + 0xfff) & ~0xfff;
	if (compress >= 0) {
		static unsigned char header1[0x48] = {
			"\x2e\x8b\x62\x20"
			"\x8b\xfc"
			"\xeb\x20"
			"GUIGUI00"
			"\0\0\0\0\0\0\0\0"
			"\0\0\0\0\0\0\0\0"
			"\0\0\0\0\0\0\0\0"
			"\x6a\xff"
			"\x0e"
			"\x68\0\0\0\0"		/* +0x2c : data begin */
			"\x2e\xff\x72\x24"
			"\x6a\x81"
			"\x6a\x04"
			"\x8b\xdc"
			"\x57"
			"\x9a\x00\x00\x00\x00\xc7\x00"
			"\x5c"
			"\xe9\0\0\0\0"		/* +0x44 : entry */
		};
		for (i = 0; i < 0x48; i++)
			buf[i] = header1[i];
		get32(&buf[0x10]) = worksize;
		get32(&buf[0x14]) = reserve;
		get32(&buf[0x20]) = stacksize;
		get32(&buf[0x24]) = datasize;
		get32(&buf[0x2c]) = code_end;
		get32(&buf[0x44]) = entry - 0x48;
		if (compress < 3) {
			putb_overbuf = overbuf;
			putb_ptr = 0;
			if (compress == 1)
				lzcompress_l2d3(buf + data_begin, datasize, 0, SIZEOFOVERBUF - 8, maxdis);
			if (compress == 2) {
				buf[0x35] = 0x82; /* tek0圧縮データー展開 */
				lzcompress_tek0(prm0, buf + data_begin, datasize, 0, SIZEOFOVERBUF - 8, maxdis);
			}
			if (putb_ptr < datasize) {
				for (i = 0; i < putb_ptr; i++)
					buf[code_end + i] = overbuf[i];
				filesize = code_end + putb_ptr;
				goto write;
			}
		} else {
			UCHAR *work = malloc(i = 257 * 1024 + (bsiz + 272) * 8);
			if (compress == 3) {
				buf[0x35] = 0x83; /* tek1圧縮データー展開 */
				j = lzcompress_tek1(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 0, opt, prm0, maxdis, submaxdis);
			}
			if (compress == 4) {
				buf[0x35] = 0x85; /* tek2圧縮データー展開 */
				j = lzcompress_tek1(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 1, opt, prm0, maxdis, submaxdis);
			}
		//	if (compress == 5) {
		//		buf[0x35] = 0x86; /* tek3圧縮データー展開 */
		//		j = lzcompress_tek3(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 0, opt, prm0, maxdis, submaxdis);
		//	}
		//	if (compress == 6) {
		//		buf[0x35] = 0x87; /* tek3圧縮データー展開 */
		//		j = lzcompress_tek3(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, bsiz, 1, opt, prm0, maxdis, submaxdis);
		//	}
			if (compress == 7) {
				buf[0x35] = 0x89; /* tek5圧縮データー展開 */
				j = lzcompress_tek5(datasize, buf + data_begin, SIZEOFOVERBUF - 8, overbuf, i, work, argv0, eopt, bsiz, 1, opt, prm0, maxdis, submaxdis);
			}
			free(work);
			if (j < datasize) {
				for (i = 0; i < j; i++)
					buf[code_end + i] = overbuf[i];
				filesize = code_end + j;
				goto write;
			}
		}
	}
	{
		static unsigned char header0[0x40] = {
			"\x2e\x8b\x62\x20"
			"\x8b\xfc"
			"\xeb\x20"
			"GUIGUI00"
			"\0\0\0\0\0\0\0\0"
			"\0\0\0\0\0\0\0\0"
			"\0\0\0\0\0\0\0\0"
			"\x2e\x8b\x4a\x24"
			"\xbe\0\0\0\0"		/* +0x2d : data begin */
			"\x2e\x8a\x06"
			"\x46"
			"\x88\x07"
			"\x47"
			"\x49"
			"\x75\xf6"
			"\xe9\0\0\0\0"		/* +0x3c : entry */
		};
		for (i = 0; i < 0x40; i++)
			buf[i] = header0[i];
		get32(&buf[0x10]) = worksize;
		get32(&buf[0x14]) = reserve;
		get32(&buf[0x20]) = stacksize;
		get32(&buf[0x24]) = datasize;
		buf[0x2d] = code_end         & 0xff;
		buf[0x2e] = (code_end >>  8) & 0xff;
		buf[0x2f] = (code_end >> 16) & 0xff;
		buf[0x30] = (code_end >> 24) & 0xff;
		get32(&buf[0x3c]) = entry - 0x40;
		for (i = 0; i < datasize; i++)
			buf[code_end + i] = buf[data_begin + i];
		filesize = code_end + datasize;
	}

write:
	fp = NULL;
	if (filepath[1])
		fp = fopen((char *)filepath[1], "wb");
	if (fp == NULL) {
		fprintf(stderr, "Commnad line error : can't open output file\n");
		return 1;
	}
	if (rjc == -1 || rjc == 1)
		osarjc(filesize, buf, 0);
	fwrite(buf, 1, filesize, fp);
	fclose(fp);

	return 0;
}

//const int search0a(int prm0, unsigned char *buf, unsigned char *buf0, const int max0, int *p, const int *table)
int search0a(int prm0, unsigned char *buf, unsigned char *buf0, const int max0, int *p, const int *table)
{
	int l, max = 0, d, ld, ld0 = 30;
	unsigned char *s;
	unsigned char c = buf[0];

	/* アクセスしてはいけないアドレス : buf + max0 */
	for (s = buf - 1; s >= buf0; s--) {
		if (*s == c) {
			for (l = max; l > 0; l--) { /* 高速化のため、後ろから比較 */
				if (buf[l] != s[l])
					goto nextloop;
			}
			/* 一致長を算出, ただしmax0になったら打ち止め */
			for (l = max; buf[l] == s[l]; ) {
				if (++l >= max0) {
					*p = s - buf;
					return l;
				}
			}

/* distanceの増加量がlの増加量のp倍を超えているようなら、損であるので採用しない */

			d = s - buf;
			if (d == -1)
				ld = 0;
			else
				for (ld = 30; d & (1 << ld); ld--);
			if ((max - l) * prm0 + ld - ld0 < 0) {
				if (l > 10) {
					max = l;
					*p = d;
					ld0 = ld;
				} else if (d >= table[l - 1]) {
					max = l;
					*p = d;
					ld0 = ld;
				}
			}
		}
nextloop:
		;
	}
	return max;
}

int search(unsigned char *buf, unsigned char *buf0, const int max0, int *p)
{
	static int table[10] = {
		-8, -512, -32 * 1024, -2 * 1024 * 1024, -0x7fffffff, 
		-0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff
	};
	return search0a(12, buf, buf0, max0, p, table);
}

int search0b_sub(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle, int *lenhis, UCHAR *s, UCHAR *s1, int len)
/* 0:該当なし */
/* 一致長lenの中で一番近いものを返す */
/* handleは勝手に動かす */
{
	int max = -0x7fffffff;
	int i;
	for (i = 0; i < 2; i++) {
		while (lenhis[i] >= len) {
			if (max < (UCHAR *) handle[i].pkey - s)
				max = (UCHAR *) handle[i].pkey - s;
			if (i == 0) {
				if (search_back(btree, &handle[i]))
					break;
			} else {
				if (search_next(btree, &handle[i]))
					break;
			}
			lenhis[i] = matchlen_bt(s, handle[i].pkey, s1);
		}
	}
	if (max == -0x7fffffff)
		max = 0;
	return max;
}

int search0b_calcld(int d)
{
	int ld = 0;
	if (d < -1)
		for (ld = 30; d & (1 << ld); ld--);
	return ld;	
}

int search0c(int prm0, UCHAR *buf, UCHAR *buf0, int max0, int *p, const int *table, int trees, struct STR_BTREE *btree, UCHAR **l1table)
{
	int l, max = 0, d, ld, ld0 = 30, lenhis[2], l0, dd, tree;
	unsigned char *s1 = buf + max0, *t;
	struct STR_BT_HANDLE handle[2];

	t = l1table[*buf];
	if (t == NULL)
		 goto fin; /* 1文字一致すらもはやどこにも存在しない */
	if (t < buf0) {
		l1table[*buf] = NULL; /* 今後面倒にしないため */
		goto fin; /* 一番近くてもそれでもmaxdisより遠いらしいのでこれもおしまい */
	}
	dd = t - buf;
	if (dd >= table[0]) {
		max = 1;
		*p = dd;
	}
	for (tree = 0; tree < trees; tree++) {
		struct STR_BT_NODE1 *node1 = btree[tree].top;
		if (node1->nodes <= 0)
			continue;
		search0(&btree[tree], &handle[0], buf, s1);
		l = matchlen_bt(buf, handle[0].pkey, s1);
		handle[1] = handle[0];
		search_next(&btree[tree], &handle[1]); /* 隣も見る */
		d = matchlen_bt(buf, handle[1].pkey, s1);
		if (l >= d)
			handle[1] = handle[0];
		else {
			handle[0] = handle[1];
			l = d;
		}
		lenhis[1] = lenhis[0] = l;
		if (l <= 1)
			continue;
		d = search0b_sub(&btree[tree], handle, lenhis, buf, s1, l);
		ld = search0b_calcld(d) - search0b_calcld(dd); /* distanceによってどのくらいビット数が変わるか */
		l0 = l - (ld + prm0 - 1) / prm0;
		while ((l - l0) * prm0 - ld > 0)
			l0++;

		if (l0 < 2)
			l0 = 2; /* 最小長1は既に処理済み */
		ld = search0b_calcld(d);
		if ((max - l) * prm0 + ld - ld0 < 0) { /* maxより長くなれるか？ */
			if (l > 10) {
				max = l;
				*p = d;
				ld0 = ld;
			} else if (d >= table[l - 1]) {
				max = l;
				*p = d;
				ld0 = ld;
			}
		}
		for (l--; l >= l0; l--) {
			d = search0b_sub(&btree[tree], handle, lenhis, buf, s1, l);
			if (d >= 0)
				continue;
			ld = search0b_calcld(d);
			if ((max - l) * prm0 + ld - ld0 < 0) { /* maxより長くなれるか？ */
				if (l > 10) {
					max = l;
					*p = d;
					ld0 = ld;
				} else if (d >= table[l - 1]) {
					max = l;
					*p = d;
					ld0 = ld;
				}
			}
		}
	}
fin:

#if 0
	/* バグによる虚偽報告がないか確認 */
//	if (max > max0) { puts("len-max err!"); exit(1); } /* これはいつもOK */
//	for (l = 0; l < max; l++)
//		if (buf[*p+l] != buf[l]) { puts("string mismatch err!"); exit(1); } /* これもいつもOK */
	if (buf + *p < buf0 && max > 0) { printf("distance err!(%d:%d) ", *p, max); max = 0; }
#endif

	return max;

/* distanceの増加量がlの増加量のp倍を超えているようなら、損であるので採用しない */
/* これを増加量ではなく減少量から逆推定 */
/* distanceに基づき、lenはどこまで減らせるかを推定 */
/* distanceの最小値(l1のとき)は一瞬で出せる */

/* 1.まずdistanceの最小値を求める */
/* 2.次にdistanceの最大値を求める */
/* 3. */

}

void putbc(const int bits, int mask)
{
	do {
	//	putb((bits & mask) != 0);
		putb_uint8_t = (putb_uint8_t << 1) + ((bits & mask) != 0);
		if (--putb_count == 0) {
			putb_count = 8;
			if (putb_ptr < 0)
				putb_buf[putb_ptr] = putb_uint8_t;
			else
				putb_overbuf[putb_ptr] = putb_uint8_t;
			putb_ptr++;
		}
	} while (mask >>= 1);
	return;
}

void flushb()
{
	if (putb_count != 8) {
		putb_uint8_t = putb_uint8_t << 1 | 1; /* "1"を送る */
		if (--putb_count)
			putb_uint8_t <<= putb_count;
		if (putb_ptr < 0)
			putb_buf[putb_ptr] = putb_uint8_t;
		else
			putb_overbuf[putb_ptr] = putb_uint8_t;
		putb_ptr++;
	}
	putb_count = 8;
	return;
}

void flushb0()
{
	if (putb_count != 8) {
		putb_uint8_t = putb_uint8_t << 1 | 0; /* "0"を送る */
		if (--putb_count)
			putb_uint8_t <<= putb_count;
		if (putb_ptr < 0)
			putb_buf[putb_ptr] = putb_uint8_t;
		else
			putb_overbuf[putb_ptr] = putb_uint8_t;
		putb_ptr++;
	}
	putb_count = 8;
	return;
}

int getbc(int bits)
//const int getbc(int bits)
{
	int ret = 0;
	do {
		if (putb_count == 8) {
			if (--putb_ptr < 0)
				return -1;
			putb_uint8_t = *putb_overbuf++;
		}
		if (--putb_count == 0)
			putb_count = 8;
		ret <<= 1;
		if (putb_uint8_t & 0x80)
			ret |= 0x01;
		putb_uint8_t <<= 1;
	} while (--bits);
	return ret;
}

//const int getbc0(int bits, int ret)
int getbc0(int bits, int ret)
/* 初期値付き */
{
	do {
		if (putb_count == 8) {
			if (--putb_ptr < 0)
				return -1;
			putb_uint8_t = *putb_overbuf++;
		}
		if (--putb_count == 0)
			putb_count = 8;
		ret <<= 1;
		if (putb_uint8_t & 0x80)
			ret |= 0x01;
		putb_uint8_t <<= 1;
	} while (--bits);
	return ret;
}

UCHAR *get_subbuf(UCHAR *subbuf, int *t, int *d, int *l)
{
	*t = *subbuf++;
	if (*t == 0x00) {
		*l = 0;
		*d = 0;
		return subbuf - 1;
	}
	if (*t == 0x01) {
		*l = 1;
		*d = 0;
		return subbuf;
	}
	*l = *subbuf++;
	if (*t & 0x20) {
		*l |= subbuf[0] <<  8;
		*l |= subbuf[1] << 16;
		*l |= subbuf[2] << 24;
		subbuf += 3;
	}
	*d = *subbuf++ | 0xffffff00;
	if (*t & 0x40) {
		*d = subbuf[-1];
		*d |= subbuf[0] <<  8;
		*d |= subbuf[1] << 16;
		*d |= subbuf[2] << 24;
		subbuf += 3;
	}
	return subbuf;
}

//const int get_subbuflen(unsigned char *subbuf, int *pt, int j)
int get_subbuflen(unsigned char *subbuf, int *pt, int j)
/* リピート長を検出 */
{
	int l, d, t, len = 0;

	get_subbuf(subbuf, &t, &d, &l);
	if (t == 0x00) {
		*pt = 0x00;
		return 0;
	}
	if (t == 0x01 || (t >= 0x02 && l < j + 1)) {
		*pt = 0x01;
		for (;;) {
			subbuf = get_subbuf(subbuf, &t, &d, &l);
			if (t == 0x00)
				break;
			if (t == 0x01)
				len++;
			else {
				if (l < j + 1)
					len += l;
				else
					break;
			}
		}
	} else {
		*pt = 0x02;
		for (;;) {
			subbuf = get_subbuf(subbuf, &t, &d, &l);
			if (t <= 0x01)
				break;
			if (l < j + 1)
				break;
			len++;
		}
	}
	return len;
}

void lzcmp_putnum1(int i)
/* 19 <= i <= 273 だと14bitコードになる */
/* 2-4-8-16形式で出力 */
{
	if (i <= 4 - 1)
		putbc(i - 1 + 1, 0x2); /* 2bit */
	else if (i <= 19 - 1)
		putbc(i - 4 + 1, 0x20); /* 6bit(4bit) */
	else if (i <= 274 - 1)
		putbc(i - 19 + 1, 0x2000); /* 14bit(8bit) */
	else if (i <= 65535)
		putbc(i, 0x20000000); /* 30bit(16bit) */
	else {
		unsigned int limit = 0xffff, mask = 0x4000, lenlen = 15;
		do {
			limit = limit * 2 + 1;
			mask <<= 1;
			lenlen++;
		} while (i > limit);
		putbc(lenlen, 0x20000000); /* 30bit */
		putbc(i, mask); /* nbit */
	}
	return;
}

void putnum_l1a(unsigned int i)
/* must i >= 1 */
/* sxsxsxsxs形式で出力 */
{
	int j;
	if (i == 1) {
		putbc(0x1, 0x1);
		return;
	}
	j = 31;
	while ((i & 0x80000000) == 0) {
		i <<= 1;
		j--;
	}
	do {
		i <<= 1; /* 最初の1は捨てる */
		if (i & 0x80000000)
			putbc(0x1, 0x2); /* sx */
		else
			putbc(0x0, 0x2); /* sx */
	} while (--j);
	putbc(0x1, 0x1); /* s("1") */
	return;
}

void putnum_l1b(unsigned int i)
/* must i >= 1 */
/* おそらく、l2aやl2bよりも優れている */
{
	if (i <= 2) {
		putbc(i + 1, 0x2); /* "10" or "11" */
		return;
	}
	putnum_l1a(i - 1); /* 3を2にする */
	return;
}

void putnum_df(int d, unsigned int s)
/* sのbitが1だと、そこまで出力したあとに、sビットを出力 */
{
	int len;
	unsigned int i = 1;
//	if (d == 0) { /* リピートマーク出力 */
//		for (i = 1; (i & s) == 0; i <<= 1)
//			putbc(1, 1);
//		putbc(0x2, 0x2); /* "10" */
//		return;
//	}

	i = 31;
	while (i > 0 && (d & (1 << i)) != 0)
		i--;
	/* i = dの0が見付かったビット位置(0～31) */

	len = -1;
	do {
		do {
			len++;
		} while ((s & (1 << len)) == 0);
	} while (i > len);

	for (;;) {
		i = s & 1;
		s >>= 1;
		if (d & (1 << len))
			putbc(1, 1);
		else
			putbc(0, 1);
		len--;
		if (i == 0)
			continue;
		if (len < 0)
			break;
		putbc(0, 1); /* 継続bit */
	}
	if (s)
		putbc(1, 1); /* 非継続bit */
	return;
}

void putnum_s8(unsigned int s)
{
	int j = 4;
	while ((s & 0xff000000) == 0 && j > 1) {
		s <<= 8;
		j--;
	}
	for (;;) {
		putbc(s >> 24, 0x80);
		if (j == 1)
			break;
		putbc(0x0, 0x1);
		s <<= 8;
		j--;
	}
	putbc(0x1, 0x1);
	return;
}

void putnum_l0a(int i, int z)
{
	static int l[4] = { 0x7fffffff, 4, 8, 16 };
	int j;
	z = l[z];
	if (i <= z) {
		while (--i)
			putbc(0x0, 0x1); /* (i - 1)個の"0" */
		putbc(0x1, 0x1);
		return;
	}
	j = z;
	do {
		putbc(0x0, 0x1); /* z個の"0" */
	} while (--j);
	putnum_l1b(i - z);
	return;
}

int getnum_l1a()
{
	int i = 1, j;
	for (;;) {
		j = getbc(1);
		if (j < 0)
			return j;
		if (j)
			break;
		i = getbc0(1, i);
		if (i < 0)
			break;
	}
	return i;
}

int getnum_l1b()
{
	int i = getnum_l1a();
	if (i < 0)
		return i;
	if (i == 1) {
		i = getbc(1);
		if (i < 0)
			return i;
	}
	return i + 1;
}

int getnum_df(unsigned int s)
{
	int d = -1, t;
	for (;;) {
		do {
			d = getbc0(1, d);
			t = s & 1;
			s >>= 1;
		} while (t == 0);
		if (s == 0)
			break;
		if (getbc(1))
			break;
	//	if (d == -1)
	//		return 0;
	}
	return d;
}

int getnum_s8()
{
	int s;
	s = getbc(8);
	while (getbc(1) == 0)
		s = getbc0(8, s);
	return s;
}

int getnum_l0a(int z)
//const int getnum_l0a(int z)
{
	static int l[4] = { 0x7fffffff, 4, 8, 16 };
	int i = 1, j;
	z = l[z];
	while (i < z) {
		j = getbc(1);
		if (j < 0)
			return j;
		if (j)
			return i;
		i++;
	}
	j = getbc(1);
	if (j < 0)
		return j;
	if (j)
		return i;
	j = getnum_l1b();
	if (j < 0)
		return j;
	return j + i;
}

//const int calclen_l1a(unsigned int i)
int calclen_l1a(unsigned int i)
/* must i >= 1 */
/* sxsxsxsxs形式で出力 */
{
	int j, l = 0;
	if (i == 1)
		return 1;
	j = 31;
	while ((i & 0x80000000) == 0) {
		i <<= 1;
		j--;
	}
	do {
		i <<= 1; /* 最初の1は捨てる */
		l += 2;
	} while (--j);
	return l + 1;
}

int calclen_l1b(unsigned int i)
//const int calclen_l1b(unsigned int i)
/* must i >= 1 */
/* sxsxsxsxs形式で出力 */
{
	if (i <= 2)
		return 2;
	return calclen_l1a(i - 1); /* 3を2にする */
}

//const int calclen_df(int d, unsigned int s)
int calclen_df(int d, unsigned int s)
/* sのbitが1だと、そこまで出力したあとに、sビットを出力 */
{
	int len, l = 0;
	unsigned int i = 1;
//	if (d == 0) { /* リピートマーク出力 */
//		for (i = 1; (i & s) == 0; i <<= 1)
//			l++;
//		l += 2;
//		return l;
//	}

	i = 31;
	while (i > 0 && (d & (1 << i)) != 0)
		i--;
	/* i = dの0が見付かったビット位置(0～31) */

	len = -1;
	do {
		do {
			len++;
		} while ((s & (1 << len)) == 0);
	} while (i > len);

	for (;;) {
		i = s & 1;
		s >>= 1;
		l++;
		len--;
		if (i == 0)
			continue;
		if (len < 0)
			break;
		l++;/* 継続bit */
	}
	if (s)
		l++; /* 非継続bit */
	return l;
}

int calclen_l0a(unsigned int i, int z)
/* must i >= 1 */
{
	static int l[4] = { 0x7fffffff, 4, 8, 16 };
	z = l[z];
	if (i <= z)
		return i;
	return z + calclen_l1b(i - z);
}

/* l2d3エンコード */

int lzcompress_l2d3(unsigned char *buf, int k, int i, int outlimit, int maxdis)
{
	int len, maxlen, srchloglen = -1, srchlogdis = 0;
	int range, distance;
	int j, i0;
//	int ptr0 = putb_ptr;

	#if (defined(NOWARN))
		i0 = 0;
	#endif

	while (i < k) {
		if (outlimit >= putb_ptr + (putb_count != 8))
			i0 = i;
		else
			return i0;

		if (i == 0)
			len = 0;
		else {
			range = i - maxdis;
			if (range < 0)
				range = 0;
			maxlen = k - i;
			distance = srchlogdis;
			if ((len = srchloglen) < 0)
				len = search(buf + i,  buf + range, maxlen, &distance);
			srchloglen = -1;
			if (len >= 2) {
				range = i + 1 - maxdis;
				if (range < 0)
					range = 0;
				srchloglen = search(buf + i + 1,  buf + range, maxlen - 1, &srchlogdis);
				if (len < srchloglen)
					len = 0;
			}
		}

		if (len < 1) {
			putbc(0x100 | buf[i], 0x100); /* "1" + buf[i] */
			i++;
		} else {
			i += len;
			if (len >= 2)
				srchloglen = -1;
			putbc(0, 0x1);
			lzcmp_putnum1(len);

			/* 上位から出力せよ */

#if 1
#define	DLEN	3
			/* 2GB以上には対応していない */
			for (j = 31 / DLEN; j >= 1 && (distance >> (j * DLEN)) == -1; j--);
			if (j >= 0) {
			//	putbc(1, 0x1); /* 1bit */
				while (j) {
					putbc((distance >> (DLEN * j - 1)) | 1, 1 << DLEN); /* DLEN bit */
					j--;
				}
				putbc(distance << 1, 1 << DLEN); /* DLEN bit */
			} else
				putbc(0, 0x1); /* 1bit */

#endif

		}
	}

	flushb();

//	printf("%d -> %d (%f%%)\n", k, putb_ptr - ptr0, (double) (putb_ptr - ptr0) * 100 / k);

	return k;
}

/* l2d3デコード */

int lzrestore_l2d3(unsigned char *buf, int k, int i, int outlimit)
{
	int len, distance, j;

	i = 0;
	for (;;) {
		j = getbc(1);
		if (j < 0)
			return i;
		if (j > 0) {
			j = getbc(8);
			if (j < 0)
				return i;
			buf[i++] = j;
			continue;
		}
		/* len */
		j = getbc(2);
//		if (j < 0)
//			return i;
		len = j;
		if (j == 0) {
			j = getbc(4);
			len = j + 3;
			if (j == 0) {
				j = getbc(8);
				len = j + 18;
				if (j == 0) {
					j = getbc(16);
					len = j;
					if (j <= 127)
						len = getbc0(j, 1); /* 最初のbitは1に決まっているから */
				}
			}
		}
		distance = -1;
		do {
			distance = getbc0(3, distance);
			j = getbc(1);
//			if (j < 0)
//				return i;
		} while (j);
		do {
			buf[i] = buf[i + distance];
			i++;
		} while (--len);
	}
}

/* tek0関係 */

struct STR_STATISTICS {
	unsigned int count, code;
};

#define STAT_TABLE_SIZE		1024 * 1024	* 2 /* 4MB(8MB) */


int setstatistics0(int siz, struct STR_STATISTICS *stat, unsigned int len, unsigned int *dat, int tablesize)
/* codeでソートされる */
{
	unsigned int *count0, i = 0, k;
	int j, l, min, max, middle;
	count0 = malloc(tablesize * sizeof (int));
	for (j = 0; j < tablesize; j++)
		count0[j] = 0;
	for (j = 0; j < len; j++) {
		if (dat[j] < tablesize)
			count0[dat[j]]++;
		else {
			k = 0;
			if (i == 0)
				goto find;
			k = i;
			if (stat[i - 1].code < dat[j])
				goto find;
			min = 0;
			max = i - 1;
			for (;;) {
				middle = (min + max) / 2;
				if (min == middle)
					break;
				if (stat[middle].code <= dat[j])
					min = middle;
				else
					max = middle;
			}
			k = min;

			for (; k < i && stat[k].code < dat[j]; k++);
find:
			if (k < i && stat[k].code == dat[j]) {
				stat[k].count++;
				goto skip;
			}
			if (i >= siz)
				goto err;
			for (l = i; l > k; l--) {
				stat[l].count = stat[l - 1].count;
				stat[l].code  = stat[l - 1].code;
			}
			stat[k].count = 1;
			stat[k].code  = dat[j];
			i++;
		}
skip:
		;
	}
	k = 0;
	for (j = 0; j < tablesize; j++) {
		if (count0[j])
			k++;
	}
	if (k + i >= siz) {
err:
		free(count0);
		return -1;
	}
	/* 押し下げて、さらに転送 */
	stat[k + i].count = 0; /* ターミネータ */
	for (j = i - 1; j >= 0; j--) {
		stat[k + j].count = stat[j].count;
		stat[k + j].code  = stat[j].code;
	}
	k = 0;
	for (j = 0; j < tablesize; j++) {
		if (count0[j]) {
			stat[k].count = count0[j];
			stat[k].code  = j;
			k++;
		}
	}
	free(count0);
	return k + i;
}

int calc_totalbits(const unsigned int *bit, const unsigned int stops)
{
	int i, t = 0;
	for (i = 0; i < 32; i++) {
		if (bit[i])
			t += bit[i] * calclen_df(-2 << i, stops);
	}
	return t;
}

#if 0

const int calc_stopbits0(const unsigned int *bit)
/* 下位bitから入れていく方法 */
{
	int l;
	unsigned int t, s, maxlen, t0;
	t = 0;
	for (l = 0; l < 32; l++) {
		t += bit[l];
	}
	if (t == 0)
		return 0;
	for (maxlen = 31; bit[maxlen] == 0; maxlen--);
	s = 1 << maxlen;
	t0 = calc_totalbits(bit, s);
	for (l = 0; l < maxlen; l++) { /* 下から入れている */
		t = calc_totalbits(bit, s | 1 << l);
		if (t0 > t) {
			s |= 1 << l;
			t0 = t;
		}
	}
	return s;
}

const int calc_stopbits1(const unsigned int *bit)
/* 上位bitから入れていく方法 */
{
	int l;
	unsigned int t, s, maxlen, t0;
	t = 0;
	for (l = 0; l < 32; l++) {
		t += bit[l];
	}
	if (t == 0)
		return 0;
	for (maxlen = 31; bit[maxlen] == 0; maxlen--);
	s = 1 << maxlen;
	t0 = calc_totalbits(bit, s);
	for (l = maxlen - 1; l >= 0; l--) { /* 上から入れている */
		t = calc_totalbits(bit, s | 1 << l);
		if (t0 > t) {
			s |= 1 << l;
			t0 = t;
		}
	}
	return s;
}

#endif

int calc_stopbits2(const unsigned int *bit)
/* 利益の多いところから入れていく方法 */
{
	int l, l0;
	unsigned int t, s, maxlen, t0, min;
	t = 0;
	#if (defined(NOWARN))
		l0 = 0;
	#endif
	for (l = 0; l < 32; l++) {
		t += bit[l];
	}
	if (t == 0)
		return 0;
	for (maxlen = 31; bit[maxlen] == 0; maxlen--);
	s = 1 << maxlen;
	min = t0 = calc_totalbits(bit, s);
	for (;;) {
		for (l = 0; l < maxlen; l++) {
			if ((s & (1 << l)) == 0) {
				t = calc_totalbits(bit, s | 1 << l);
				if (min > t) {
					min = t;
					l0 = l;
				}
			}
		}
		if (t0 <= min)
			break;
		t0 = min;
		s |= 1 << l0;
	}
	return s;
}

#define calc_stopbits	calc_stopbits2

#if 0

int calc_stopbits(const unsigned int *bit)
{
	unsigned int min, s0, method, t, s;
	s0 = calc_stopbits2(bit);
	min = calc_totalbits(bit, s0);
	method = 2;
#if 0
	s = calc_stopbits1(bit);
	t = calc_totalbits(bit, s);
	if (min > t) {
		min = t;
		s0 = s;
		method = 1;
	}
	s = calc_stopbits0(bit);
	t = calc_totalbits(bit, s);
	if (min > t) {
		min = t;
		s0 = s;
		method = 0;
	}
	printf("[%d] ", method);
#endif
	return s0;
}

#endif

int calcdis_s(UCHAR *subbuf, const int i)
/* disのエンコード方式の自動選択 */
{
	unsigned int *distbl = malloc(32 * sizeof (int)), dis_s;
	int j, l, d, t;
	for (j = 0; j < 32; j++)
		distbl[j] = 0;
	for (;;) {
		subbuf = get_subbuf(subbuf, &t, &d, &l);
		if (t == 0x00)
			break;
		if (t == 0x01)
			continue;
		if (l < i + 1)
			continue;
		if (d == -1)
			j = 0;
		else {
			j = 31;
			while (d & 0x80000000) {
				d <<= 1;
				j--;
			}
		}
		distbl[j]++;
	}
	dis_s = calc_stopbits(distbl);
	free(distbl);
	return dis_s;
}

int lzcompress_tek0(int prm, unsigned char *buf, int k, int i, int outlimit, int maxdis)
{
	unsigned char *subbuf, *subbuf0, *subbuf1;
	int len, maxlen, srchloglen = -1, srchlogdis = 0;
	int range, distance, dis_s;
	int i0, j, z, z0;
	static int table[10] = {
		-8, -2 * 1024, -128 * 1024, -16 * 1024 * 1024, -2 * 1024 * 1024 * 1024, 
		-0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff
	};
	int t, l, d, ii, ii0;

	#if (defined(NOWARN))
		z0 = ii0 = 0;
	#endif

	subbuf0 = malloc(SIZEOFSUBBUF);
	subbuf = subbuf0;

	while (i < k) {
	//	if (outlimit >= putb_ptr + (putb_count != 8))
	//		i0 = i;
	//	else
	//		return i0;

		if (i == 0)
			len = 0;
		else {
			range = i - maxdis;
			if (range < 0)
				range = 0;
			maxlen = k - i;
			len = srchloglen;
			distance = srchlogdis;
			if ((len = srchloglen) < 0)
				len = search0a(prm, buf + i,  buf + range, maxlen, &distance, table);
			srchloglen = -1;
			if (len >= 2) {
				range = i + 1 - maxdis;
				if (range < 0)
					range = 0;
				srchloglen = search0a(prm, buf + i + 1,  buf + range, maxlen - 1, &srchlogdis, table);
				if (len < srchloglen) {
					len = 0;
					if (i >= 8) {
						for (distance = -1; distance <= -8; distance--) {
							if (buf[i] == buf[i + distance]) {
								len = 1;
								break;
							}
						}
					}
				}
			}
		}

		if (len < 1) {
			subbuf[0] = 0x01;
			i++;
			subbuf++;
		} else {
			i += len;
			if (len >= 2)
				srchloglen = -1;
			subbuf[0] = 0x13;
			subbuf1 = subbuf;
			subbuf[1] = len & 0xff;
			subbuf += 2;
			if (len > 255) {
				subbuf[0] = (len >>  8) & 0xff;
				subbuf[1] = (len >> 16) & 0xff;
				subbuf[2] = (len >> 24) & 0xff;
				subbuf += 3;
				*subbuf1 = 0x36;
			}
			*subbuf++ = distance & 0xff;
			if (distance < -256) {
				subbuf[0] = (distance >>  8) & 0xff;
				subbuf[1] = (distance >> 16) & 0xff;
				subbuf[2] = (distance >> 24) & 0xff;
				subbuf += 3;
				*subbuf1 += 0x43;
			}

		}
	}
	*subbuf = 0x00;

	/* lenのエンコード方式の自動選択 */
	i0 = 0x7fffffff;
	for (z = 0; z < 16; z++) {
		for (ii = 0; ii < 2; ii++) {
			for (j = 0; j < 4; j++) {
				/* 最初にdis_sを算出し、l1bを正確に計算する */
				dis_s = calcdis_s(subbuf0, j);
				subbuf = subbuf0;
				len = 0;
				for (;;) {
					/* "0"-phase (非圧縮フェーズ) */
					srchloglen = get_subbuflen(subbuf, &t, j);
					if (srchloglen == 0)
						break;
					len += calclen_l0a(srchloglen, z & 0x03);
					do {
						subbuf = get_subbuf(subbuf, &t, &d, &l);
						if (t == 0x01) {
							len += 8;
							srchloglen--;
						} else {
							len += 8 * l;
							srchloglen -= l;
						}
					} while (srchloglen);

					/* "1"-phase (圧縮フェーズ) */
					srchloglen = get_subbuflen(subbuf, &t, j);
					if (srchloglen == 0)
						break;
					len += calclen_l0a(srchloglen, z >> 2);
					do {
						subbuf = get_subbuf(subbuf, &t, &d, &l);
						len += calclen_df(d, dis_s);
						if (ii == 0)
							len += calclen_l1a(l - j);
						else
							len += calclen_l1b(l - j);
					} while (--srchloglen);
				}
				if (i0 > len) {
					i0 = len;
					i = j;
					ii0 = ii;
					z0 = z;
				}
			}
		}
	}

	/* disのエンコード方式の自動選択 */
	dis_s = calcdis_s(subbuf0, i);
//	printf("method:l1%c(+%d) = %8d rep-mode:%x ", 'a' + ii0, i, i0, z0);
//	printf("dis_s = %08x\n", dis_s);

	/* エンコード */
	putnum_s8(dis_s);
	putbc(i, 0x2);
	putbc(ii0, 0x1); /* l1a/l1b */
	putbc(z0, 0x2); /* "0"-phase */
	putbc(z0 >> 2, 0x2); /* "1"-phase */

	subbuf = subbuf0;
	j = 0;
	for (;;) {
		/* "0"-phase (非圧縮フェーズ) */
		srchloglen = get_subbuflen(subbuf, &t, i);
		if (srchloglen == 0)
			break;
		putnum_l0a(srchloglen, z0 & 0x03);
		do {
			subbuf = get_subbuf(subbuf, &t, &d, &l);
			if (t == 0x01)
				l = 1;
			do {
				putbc(buf[j], 0x80);
				j++;
				srchloglen--;
				l--;
			} while (l);
		} while (srchloglen);

		/* "1"-phase (圧縮フェーズ) */
		srchloglen = get_subbuflen(subbuf, &t, i);
		if (srchloglen == 0)
			break;
		putnum_l0a(srchloglen, z0 >> 2);
		do {
			subbuf = get_subbuf(subbuf, &t, &d, &l);
			putnum_df(d, dis_s);
			if (ii0 == 0)
				putnum_l1a(l - i);
			else
				putnum_l1b(l - i);
			j += l;
		} while (--srchloglen);
	}

	flushb0();
	free(subbuf0);

	return k;
}

int lzrestore_tek0(unsigned char *buf, int k, int i, int outlimit)
{
	int len, distance, j, z0, z1;
	unsigned int dis_s, l_ofs, method;

//int dbc_by = 0, dbc_lz = 0, dbc_cp = 0, dbc_ds = 0;
//int ddd = 100;

	/* ヘッダ読み込み */
	dis_s = getnum_s8();
	l_ofs = getbc(2);
	method = getbc(1); /* l1a/l1b */
	z0 = getbc(2);
	z1 = getbc(2);

	#if (defined(DEBUGMSG))
		printf("method:l1%x(+%d) dis_s = %08x z0 = %d z1 = %d\n", 0xa + method, l_ofs, dis_s, z0, z1);
	#endif

	i = 0;
	for (;;) {
		/* "0"-phase (非圧縮フェーズ) */
		j = getnum_l0a(z0);
		if (j < 0)
			break;
//dbc_by += calclen_l0a(j, z0);
		do {
			len = getbc(8);
			if (len < 0)
				break;
			buf[i++] = len;
		} while (--j);

		/* "1"-phase (圧縮フェーズ) */
		j = getnum_l0a(z1);
		if (j < 0)
			break;
//dbc_lz += calclen_l0a(j, z1);
		do {
			distance = getnum_df(dis_s);
//dbc_ds += calclen_df(distance, dis_s);
			if (method == 0) {
				len = getnum_l1a();
//dbc_cp += calclen_l1a(len);
			} else {
				len = getnum_l1b();
//dbc_cp += calclen_l1b(len);
//printf("%d ", calclen_l1b(len));
//if (--ddd == 0) exit(1);
			}
			if (len < 0)
				break;
			len += l_ofs;
			do {
				buf[i] = buf[i + distance];
				i++;
			} while (--len);
		} while (--j);
	}
//printf("%d %d %d %d\n", dbc_by, dbc_lz, dbc_cp, dbc_ds);
	return i;
}

int tek1_intlog2p(int i);

void tek1_puts7(unsigned int i)
{
	if (i < 0x80)
		goto len1;
	i -= 0x80;
	if (i < 0x4000)
		goto len2;
	i -= 0x4000;
	if (i < 0x200000)
		goto len3;
	i -= 0x200000;
	if (i < 0x10000000)
		goto len4;
	i -= 0x10000000;
	*tek1_s7ptr++ = ((i >> 28) & 0x7f) << 1;
len4:
	*tek1_s7ptr++ = ((i >> 21) & 0x7f) << 1;
len3:
	*tek1_s7ptr++ = ((i >> 14) & 0x7f) << 1;
len2:
	*tek1_s7ptr++ = ((i >>  7) & 0x7f) << 1;
len1:
	*tek1_s7ptr++ = (i & 0x7f) << 1 | 1;
	return;
}

UCHAR *tek1_puts7sp(UCHAR *p, unsigned int i)
{
	if (i < 0x80)
		goto len1;
	if (i < 0x4000)
		goto len2;
	if (i < 0x200000)
		goto len3;
	if (i < 0x10000000)
		goto len4;
	*p++ = ((i >> 28) & 0x7f) << 1;
len4:
	*p++ = ((i >> 21) & 0x7f) << 1;
len3:
	*p++ = ((i >> 14) & 0x7f) << 1;
len2:
	*p++ = ((i >>  7) & 0x7f) << 1;
len1:
	*p++ = (i & 0x7f) << 1 | 1;
	return p;
}

void tek1_puts7s(unsigned int i)
{
	tek1_s7ptr = tek1_puts7sp(tek1_s7ptr, i);
	return;
}

int lzcompress_tek3h(int srcsiz, int *src, int outsiz, UCHAR *outbuf, UCHAR *work, int modecode);
int lzcompress_tek3s(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int flags, int prm, int maxdis);

void debugtest();

int lzcompress_tek3(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int bsiz, int flags, int opt, int prm, int maxdis, int submaxdis)
/* workは8バイトアラインしておくこと、srcも8バイトアラインが望ましい */
/* レベル1までしかないので、これだと256KBまでしか圧縮できない */
/* レベル2も書けば16MB。これなら悪くはない */
/* レベル3も書けば1GB。ますますよろしい（bim2binとしてはこれが限界でもいいだろう） */
/* レベル1ヘッダは、66x5。レベル2ヘッダは65x5、レベル3ヘッダは65x5 */
/* (1 + 65 + 1 + 65 * 64 + 1 + 66 * 64 * 64 + 1) * 5 = 1341KB */
/* これはやってられないので、レベル2までのサポートに変更 */
/* (1 + 65 + 1 + 65 * 64 + 1) * 5 = 21KB */
/*
	64個というのはいいのか？
	tek1hで圧縮するべきではないのか。
	tek1hはレングス圧縮はなく、タイプやベースアドレスはs7で格納。次に継続ビットをs7で（limit=0)。4bit単位でlenbit総数。
	256個くらいはあったほうがいいだろう。これだと4KBブロックで、レベル1で1MB、レベル2で256MBになる。
	モード、ベースアドレス、継続ビットはs7、最初のサイズと差分サイズはuc
*/
/* workはlzcompress_tek1hが3KB使う */
{
	int i, j, k, l, m, *s256;
	UCHAR *p, *q, *r, *s, *t, *u;
	tek1_s7ptr = outbuf;
	if (bsiz > srcsiz) {
		while ((bsiz >> 1) >= srcsiz)
			bsiz >>= 1;
	}
	s256 = (int *) work;
	work += 256 * 257 * sizeof (int); /* 257KB */
	wrksiz -= 256 * 257 * sizeof (int);
//	if ((flags & 1) == 0)
//		tek1_puts7(srcsiz);
	for (i = 0, j = bsiz >> 8; j > 1; i++, j >>= 1);
	tek1_puts7s(i << 1 | 1); /* 上位3bitはバージョン(0はtek1sのみを意味する、さらにレベル0サイズ情報なし) */
	/* 下位1bitについて：0（スーパーショート） */
	/* 001:ノーマルロング、011:マルチモードロング、その他はリザーブ */
//	tek1_puts7(3); /* 3:tek1, 2:無圧縮 */
//	tek1_puts7(1); /* ターミネータ */
	p = tek1_s7ptr;
	r = q = &p[322 * 1024]; /* レベル0出力用 */
	i = (srcsiz + bsiz - 1) / bsiz;
	s = src;
	for (j = 0; j < i; j++, s += bsiz) {
		t = src;
		if (&src[srcsiz] >= &s[bsiz]) {
			for (k = 0; k < j; k++, t += bsiz) {
				for (l = 0; l < bsiz; l++) {
					if (s[l] != t[l])
						goto skip0;
				}
				/* 一致ブロック発見 */
				s256[j] = s256[k]; /* k番目のブロックの内容をそのまま使え */
				goto skip1;
	skip0:
				;
			}
		}
		k = &src[srcsiz] - s;
		if (k > bsiz)
			k = bsiz;
		s256[j] = q - r; /* ブロック開始位置 */
	//	k = lzcompress_tek3s(k, s, &outbuf[outsiz] - q, q, wrksiz, work,
	//		(flags & 1) | (i == 1 && (flags & 1) != 0) << 1 | opt << 2, prm, maxdis);
		q += k;
skip1:
		;
	}
	t = s = &p[(1 + 256) * 5];
	/* i:ブロック総数 */
	/* j:レベル1ブロック総数 */
	j = (i + 255) >> 8;
	for (k = 0; k < j; k++) {
		if (((k + 1) << 8) <= i) {
			for (l = 0; l < k; l++) {
				for (m = 0; m < 256; m++) {
					if (s256[(k << 8) + m] != s256[(l << 8) + m])
						goto skip2;
				}
				/* 一致ブロック発見 */
				s256[256 * 256 + k] = s256[256 * 256 + l];
				goto skip3;
	skip2:
				;
			}
		}
		l = i - (k << 8);
		if (l > 256)
			l = 256;
		s256[256 * 256 + k] = s - t; /* ブロック開始位置 */

		s += lzcompress_tek3h(l, &s256[k << 8], r - s, s, work, 0-1 /* mode */);
		/* フォーマットバージョン0ではモードフィールドはない */
skip3:
		;
	}
	if (j > 1) {
		/* レベル2出力 */
		u = &p[5];
		m = lzcompress_tek3h(j, &s256[256 * 256], t - u, u, work, -1 /* mode */);
		tek1_s7ptr = p;
		tek1_puts7s(m - 2); /* レベル2ゾーンサイズ */
		p = tek1_s7ptr;
		for (l = 0; l < m; l++)
			p[l] = u[l];
		p += m;
	}
	/* レベル1出力(s, t) */
	if (i > 1) {
		tek1_s7ptr = p;
		tek1_puts7s((m = s - t) - 2); /* レベル1ゾーンサイズ */
		p = tek1_s7ptr;
		for (l = 0; l < m; l++)
			p[l] = t[l];
		p += m;
	}
	/* レベル0出力(q, r) */
	if (i == 1 && (flags & 1) != 0)
		p--; /* tek2ショート */
	else {
		/* verによっては以下をやる */
	//	tek1_s7ptr = p;
	//	tek1_puts7((m = q - r) - 2); /* レベル0ゾーンサイズ */
	//	p = tek1_s7ptr;
	}
	m = q - r;
	for (l = 0; l < m; l++)
		p[l] = r[l];
	return &p[m] - outbuf;
}

struct STR_BITBUF {
	UCHAR *p0, *p2, *p, *p1;
	int bit, bit1;
};

void initbitbuf(struct STR_BITBUF *str, unsigned int len, UCHAR *p0)
{
	str->p0 = p0;
	str->p2 = p0 + len;
	str->p1 = p0;
	str->bit1 = 0;
//	rewindbitbuf(str);
	str->p = str->p0;
	str->bit = 0;
	return;
}

void putbitbuf(struct STR_BITBUF *str, unsigned int len, unsigned int dat)
/* 最大32bit出力 */
{
	do {
		if (dat & 1)
			*(str->p) |= 1 << (str->bit);
		else
			*(str->p) &= ~(1 << (str->bit));
		if ((str->bit = (str->bit + 1) & 0x07) == 0)
			str->p++;
		dat >>= 1;
	} while (--len);
	if ((str->p1 < str->p) || (str->p1 == str->p && str->bit1 < str->bit)) {
		str->p1 = str->p;
		str->bit1 = str->bit;
	}
	return;
}

struct TEK1_STR_UC { /* ユニバーサルコード */
	UCHAR lentbl[TEK1_MAXLEN + 1], maxlen, dummy[3];
	int limit, base[TEK1_MAXLEN + 2];
};

#define TEK1_DEFINED_UC		1

#define STR_UC	TEK1_STR_UC

static UCHAR tek1_ucprm_s4[] = { 0x0c, 0x22, 0x22, 0x62 };
//	/* 0_0_011101 11011101 11011101 111_0_1011	24, 444444 */
	/* 0_1_100010 00100010 00100010 000_0_1100	24, 444444 */
//static UCHAR tek1_ucprm_l0a4[] = { 0x0b, 0x02, 0x00, 0x00, 0x00 };
//	/* 00000_0_00 00000000 00000000 00000010 000_0_1011	24, 1111111111111111111111110000 */
static UCHAR tek1_ucprm_l1a[] = { 0xac, 0xff, 0xff, 0xff };
//	/* 0_0000000 00000000 00000000 010_0_1011			24, 1111111111111111111111110 */
	/* 1_1111111 11111111 11111111 101_0_1100			24, 1111111111111111111111110 */
static UCHAR tek1_ucprm_l1b[] = { 0xdc, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x6a };
//	/* 0_0_010101 01010101 01010101 01010101 01010101 01010101 001_1_1011	24, 1111111111111111111111101 */
	/* 0_1_101010 10101010 10101010 10101010 10101010 10101010 110_1_1100	24, 1111111111111111111111101 */
static UCHAR tek1_ucprm_l1c[] = { 0xcc, 0xff, 0xff, 0x7f };
//	/* 0_0_000000 00000000 00000000 001_0_1011			24, 111111111111111111111111 */
	/* 0_1_111111 11111111 11111111 110_0_1100			24, 111111111111111111111111 */
static UCHAR tek1_ucprm_ds1[] = { 0x2c, 0x44, 0x52, 0xd5 };
//	/* 0_0101010 10101010 10111011 110_0_1011 			24, 222222 22440 */
	/* 1_1010101 01010101 01010100 001_0_1100 			24, 222222 22440 */
	/* 1_1010101 01010010 01000100 001_0_1100 			24, 222222 33440 */
//static UCHAR tek2_ucprm_stbyds[] = { 0x82, 0x77, 0x00 };
//	/* 0000000_0 _01110111 10000_0_10	8, 440000 */
static UCHAR tek1_ucprm_s41[] = { 0x2c, 0x44, 0x44, 0xc4 };
//	/* 0_0111011 10111011 10111011 110_0_1011			24, 4444440 */
	/* 1_1000100 01000100 01000100 001_0_1100			24, 4444440 */
//static UCHAR tek2_ucprm_tr8[] = { 0x03, 0x18 };
//	/* 000_0_0111 11111_0_10							8, 8 */
	/* 000_1_1000 00000_0_11							8, 8 */
//static UCHAR tek1_ucprm_ds0[] = { 0x0c, 0x22, 0xa9, 0x6a };
//	/* 0_1_101010 10101010 10101010 000_0_1100 			24, 222222 2244 */
	/* 0_1_101010 10101001 00100010 000_0_1100 			24, 222222 3344 */

//staic UCHAR pad[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//void tek1_inituc0(struct TEK1_STR_BTBUF *btbuf, struct TEK1_STR_UC *ucstr);
void tek1_inituc1(struct TEK1_STR_UC *ucstr, UCHAR *prm);
unsigned int calclen_uc(struct STR_UC *ucstr, struct STR_STATISTICS *stat);
void putnum_uc0(struct STR_BITBUF *bitbuf, struct STR_UC *ucstr, unsigned int i);

void initucstr(struct TEK1_STR_UC *ucstr, UCHAR flag, UCHAR *param0)
{
//	if (flag)
//		tek1_inituc0(ucstr);
//	else
		tek1_inituc1(ucstr, param0);
	return;
}

UCHAR calclen_ucstr_sub(struct STR_UC *ucstr, int i0)
/* bit3-7:max(4, 8, 12, 16, 20, 24), bit0-2:mode(0, 1, 7) */
{
	UCHAR m = 0, max;
	int i;
	i = ucstr->maxlen;
	max = ucstr->lentbl[i];
	for (i--; i >= i0; i--) {
		if (max < ucstr->lentbl[i])
			max = ucstr->lentbl[i];
		if (ucstr->lentbl[i] > ucstr->lentbl[i + 1])
			m = 7;
	}
	max = (max + 3) & ~3;
	if (max == 0)
		max = 4;
	if (m < 7) {
		i = ucstr->maxlen;
		if (i > 0 && ucstr->lentbl[i - 1] >= max)
			max += 4; /* maxが2度以上出現するのを回避するため */
		for (i = i0; i < ucstr->maxlen && ucstr->lentbl[i] == 0; i++);
		for (; i < ucstr->maxlen; i++) {
			if (ucstr->lentbl[i] >= ucstr->lentbl[i + 1]) {
				m = 1;
				break;
			}
		}
	}
	return m | max << 3;
}

int calclen_uc0str(struct STR_UC *ucstr, int i, int n)
{
	UCHAR m, l = 2 + 1 + 1, max;
	int d;
	max = m = calclen_ucstr_sub(ucstr, i);
	m &= 0x07;
	max = (max >> 3) & 0x1f;
	if (m == 7)
		l = (3 + 2 + 2) + ucstr->maxlen * 2; /* 符号bit出力回数をあらかじめ足しておく(maxlen + 1)個 */
	if (m == 1)
		l = (2 + 1 + 1) + ucstr->maxlen; /* m == 0のときに比べて1長いことを足しておく */
	for (i = 0; i <= ucstr->maxlen; i++) {
		d = ucstr->lentbl[i] - n;
		if (d < 0)
			d ^= -1;
		l += d;
		if (m == 0 && d == 0)
			l++;
		n = ucstr->lentbl[i];
	}
	if (m < 7) {
		if (n == max)
			goto fin;
		l += max - n + m; /* (max + m - n)個の1を出す */
	} else {
		if (n > max - n)
			l += max - n + 2; /* プラス側への追い出し */
		else {
			l += n + 2; /* マイナス側への追い出し */
			if (n == 0)
				l--; /* 符号bitだけでいいので */
		}
	}
fin:
	if (max >= 12)
		l++;
	if (max >= 20)
		l++;
	if (m < 7 && n > 0 && ucstr->maxlen > 0)
		l++;
	return l;
}

int calclen_ucstr(struct STR_UC *ucstr)
{
	return calclen_uc0str(ucstr, 0, 0);
}

void output_uc0str(struct STR_BITBUF *bitbuf, struct STR_UC *ucstr, int i, int n)
/* 最後だけ小さいというやつにはうまく対応できず、m=7でやってしまう */
{
	UCHAR m, max;
	int d;
	max = m = calclen_ucstr_sub(ucstr, i);
	m &= 0x07;
	max = (max >> 3) & 0x1f;
	if (m == 7)
		putbitbuf(bitbuf, 3, 0); /* "000" */
	if (max <= 8)
		putbitbuf(bitbuf, 1, 1); /* "1" */
	else if (max <= 16)
		putbitbuf(bitbuf, 2, 2); /* "10" */
	else
		putbitbuf(bitbuf, 3, 4); /* "100" */
	putbitbuf(bitbuf, 1, (max >> 2) - 1);
	if (m < 7)
		putbitbuf(bitbuf, 1, m);
//	n = 0;
	for ( /* i = 0 */; i <= ucstr->maxlen; i++) {
		d = ucstr->lentbl[i] - n;
		if (m == 7) {
			if (d >= 0)
				putbitbuf(bitbuf, 1, 0);
			else {
				putbitbuf(bitbuf, 1, 1);
				d ^= -1;
			}
		}
		if (m == 0 && n > 0)
			d--;
		if (d)
			putbitbuf(bitbuf, d, 0);
		putbitbuf(bitbuf, 1, 1);
		n = ucstr->lentbl[i];
	}
	if (m < 7) {
		if (n == max)
			goto fin;
		d = max - n + 1; /* (max + 1 - n)個の"0"を出す */
		if (m == 0 && n > 0)
			d--;
	} else {
		if (n >= max - n) {
			putbitbuf(bitbuf, 1, 0);
			d = max - n + 1; /* プラス側への追い出し */
		} else {
			putbitbuf(bitbuf, 1, 1);
			if (n == 0)
				goto fin;
			d = n + 1; /* マイナス側への追い出し */
		}
	}
	putbitbuf(bitbuf, d, 0);
fin:
	if (m < 7 && n > 0 && ucstr->maxlen > 0)
		putbitbuf(bitbuf, 1, 1); /* 最終パラメータ補正値 */
	return;
}

void output_ucstr(struct STR_BITBUF *bitbuf, struct STR_UC *ucstr)
{
	output_uc0str(bitbuf, ucstr, 0, 0);
	return;
}


void putnum_uc0(struct STR_BITBUF *bitbuf, struct STR_UC *ucstr, unsigned int i)
{
	int j;
	for (j = 0; j <= TEK1_MAXLEN; j++) {
		if (ucstr->base[j] <= i && i < ucstr->base[j + 1]) {
			if (j > 0)
				putbitbuf(bitbuf, j, 0);
			if (j < ucstr->maxlen)
				putbitbuf(bitbuf, 1, 1);
			if (ucstr->lentbl[j])
				putbitbuf(bitbuf, ucstr->lentbl[j], i - ucstr->base[j]);
			break;
		}
	}
	return;
}


unsigned int calclen_uc(struct STR_UC *ucstr, struct STR_STATISTICS *stat)
{
	unsigned int i, j, k;
	k = 0;
	j = 0;
	for (i = 0; stat[i].count != 0; i++) {
		for (;;) {
			if (stat[i].code < ucstr->base[j + 1]) {
				k += (j + ucstr->lentbl[j]) * stat[i].count;
				if (j < ucstr->maxlen)
					k += stat[i].count;
				break;
			}
			j++;
		}
	}
	return k;
}

void optsub_inituc(struct STR_UC *ucstr, UCHAR *prm)
{
	int i;
	ucstr->maxlen = prm[TEK1_MAXLEN + 1];
	ucstr->base[0] = 0;
	for (i = 0; i <= ucstr->maxlen; i++) {
		ucstr->lentbl[i] = prm[i];
		ucstr->base[i + 1] = ucstr->base[i] + (1 << prm[i]);
	}
	return;
}

void optsub_limcbit(struct STR_UC *ucstr, int lim, unsigned int cbits)
/* maxlenもいじるので注意 */
{
	int i, j;
	for (i = 0; i < lim; i++)
		ucstr->lentbl[i] = 0;
	j = 0;
	do {
		for (;;) {
			j++;
			if ((cbits & 1) == 0)
				break;
			cbits >>= 1;
		}
		cbits >>= 1;
		ucstr->lentbl[i] = j;
		i++;
	} while (j < 24);
	ucstr->base[0] = 0;
	ucstr->maxlen = i - 1;
	for (j = 0; j < i; j++)
		ucstr->base[j + 1] = ucstr->base[j] + (1 << ucstr->lentbl[j]);
	return;
}

void optsub_saveprm(UCHAR *prm, struct STR_UC *ucstr)
{
	int i;
	prm[TEK1_MAXLEN + 1] = ucstr->maxlen;
	for (i = 0; i <= TEK1_MAXLEN; i++)
		prm[i] = ucstr->lentbl[i];
	return;
}

#if 1

static struct STR_UC *optsub_ucstr;
static int optsub_min, optsub_max;
static UCHAR *optsub_minprm;
static int (*optsub_func)(struct STR_UC *);

void optsub_fullopt1(int i, int rest, struct STR_STATISTICS *stat0, int total0)
{
	int base0 = optsub_ucstr->base[i], j, tmp, total1, base1;
	struct STR_STATISTICS *stat;
	optsub_ucstr->lentbl[i] = j = tek1_intlog2p(optsub_max - base0);
	optsub_ucstr->maxlen = i;
	tmp = total0 + (j + i) * rest;
	if (optsub_min > tmp) {
		tmp += (*optsub_func)(optsub_ucstr);
		if (optsub_min > tmp) {
			optsub_min = tmp;
			optsub_saveprm(optsub_minprm, optsub_ucstr);
		}
	}
	if (i == TEK1_MAXLEN)
		return;
	i++;
	for (j--; j >= 0; j--) {
		if (i > 1 && optsub_ucstr->lentbl[i - 2] > j)
			continue; /* 時間がかかるのでやらない */
		base1 = base0 + (1 << j);
		tmp = 0;
		for (stat = stat0; stat->code < base1; stat++)
			tmp += stat->count;
		total1 = total0 + (i + j) * tmp;
		optsub_ucstr->maxlen = i - 1; /* ここでいったん打ち切ってみる */
		optsub_ucstr->base[i] = base1;
		optsub_ucstr->lentbl[i - 1] = j;
		if (total1 + i * (rest - tmp) + (*optsub_func)(optsub_ucstr) < optsub_min)
			optsub_fullopt1(i, rest - tmp, stat, total1);
	}
	return;
}

void optsub_fullopt2(int i, int rest, struct STR_STATISTICS *stat0, int total0)
{
	int base0 = optsub_ucstr->base[i], j, tmp, total1, base1;
	struct STR_STATISTICS *stat;
	optsub_ucstr->lentbl[i] = j = tek1_intlog2p(optsub_max - base0);
	optsub_ucstr->maxlen = i;
	tmp = total0 + (j + i) * rest;
	if (optsub_min > tmp) {
		tmp += (*optsub_func)(optsub_ucstr);
		if (optsub_min > tmp) {
			optsub_min = tmp;
			optsub_saveprm(optsub_minprm, optsub_ucstr);
		}
	}
	if (i == TEK1_MAXLEN)
		return;
	i++;
	for (j--; j >= 0; j--) {
		base1 = base0 + (1 << j);
		tmp = 0;
		for (stat = stat0; stat->code < base1; stat++)
			tmp += stat->count;
		total1 = total0 + (i + j) * tmp;
		optsub_ucstr->maxlen = i - 1; /* ここでいったん打ち切ってみる */
		optsub_ucstr->base[i] = base1;
		optsub_ucstr->lentbl[i - 1] = j;
		if (total1 + i * (rest - tmp) + (*optsub_func)(optsub_ucstr) < optsub_min)
			optsub_fullopt2(i, rest - tmp, stat, total1);
	}
	return;
}

int optsub_evaluate1(int rest, struct STR_STATISTICS *st, int base0, int *pj, int k0, int mode);

int optsub_evaluate0(int rest, struct STR_STATISTICS *st, int base0, int *pj)
/* 結構それっぽい評価関数 */
/* ゼロ無しモードだけど、現在0行進中用 */
{
	int base1, k, eval = 0, tmp, j = 0;
	do {
		k = -1;
		tmp = 0;
		do {
			k++;
			base1 = base0 + (1 << k);
			while (st->count != 0 && st->code < base1) {
				tmp += st->count;
				st++;
			}
		} while (rest > tmp * 3);
		eval += tmp * k;
		base0 = base1;
		if (rest > tmp)
			eval += rest;
		j++;
		rest -= tmp;
	} while (rest > 0 && k == 0);
	if (rest > 0) {
		eval += optsub_evaluate1(rest, st, base0, &tmp, k, 0);
		j += tmp;
	}
	*pj = j;
	return eval;
}

int optsub_evaluate1(int rest, struct STR_STATISTICS *st, int base0, int *pj, int k0, int mode)
/* 結構それっぽい評価関数 */
/* ゼロ無しモードで、現在非0行進中用(mode=0) */
/* もしくはゼロありモード(mode=-1) */
{
	int base1, k, eval = 0, tmp, j = 0;
	do {
		k = k0 + mode;
		tmp = 0;
		do {
			k++;
			base1 = base0 + (1 << k);
			while (st->count != 0 && st->code < base1) {
				tmp += st->count;
				st++;
			}
		} while (rest > tmp * 3);
		eval += tmp * k;
		base0 = base1;
		if (rest > tmp)
			eval += rest;
		j++;
		rest -= tmp;
	} while (rest > 0);
	*pj = j;
	return eval;
}

int optsub_evaluate2(int rest, struct STR_STATISTICS *st, int base0, int *pj)
/* 結構それっぽい評価関数 */
/* マイナスもありモード */
{
	int base1, k, eval = 0, tmp, j = 0;
	do {
		k = -1;
		tmp = 0;
		do {
			k++;
			base1 = base0 + (1 << k);
			while (st->count != 0 && st->code < base1) {
				tmp += st->count;
				st++;
			}
		} while (rest > tmp * 3);
		eval += tmp * k;
		base0 = base1;
		if (rest > tmp)
			eval += rest;
		j++;
		rest -= tmp;
	} while (rest > 0);
	*pj = j;
	return eval;
}

int optimize_uc(struct STR_UC *ucstr, struct STR_STATISTICS *stat, int (*func)(struct STR_UC *), UCHAR *ucprm0, UCHAR *ucprm1, UCHAR *ucprm2)
/* 評価関数利用型の前進法＋単純二分法 */
{
	UCHAR minprm[TEK1_MAXLEN + 2];
	int min, i, j, k, tmp, rc = 0, rest, rest0, base0, base1, max, dummy;
	int count, tmpmin, min_k, min_c;
	struct STR_STATISTICS *st, *min_s;
	#if (defined(NOWARN))
		min_k = min_c = 0;
		min_s = stat;
	#endif
	if (ucprm0) {
		initucstr(ucstr, 0, ucprm0);
		if (stat->count == 0)
			goto fin;
		min = calclen_uc(ucstr, stat);
		optsub_saveprm(minprm, ucstr);
		if (ucprm2)
			min += 2;
	} else {
		if (ucprm1) {
			min = 0x7fffffff;
			goto skip;
		}
		optsub_limcbit(ucstr, 0, 0);
		if (stat->count == 0)
			goto fin;
		optsub_saveprm(minprm, ucstr);
		min = calclen_uc(ucstr, stat);
		for (j = 1; j < 8; j++) {
			optsub_limcbit(ucstr, j, 0);
			tmp = calclen_uc(ucstr, stat);
			if (min > tmp) {
				min = tmp;
				optsub_saveprm(minprm, ucstr);
				rc = j;
			}
		}
		min += 3;
	}
	if (ucprm1) {
		initucstr(ucstr, 0, ucprm1);
		tmp = calclen_uc(ucstr, stat);
		if (ucprm2)
			tmp += 2;
		if (min > tmp) {
			min = tmp;
			optsub_saveprm(minprm, ucstr);
			rc = 1;
		}
	}
	if (ucprm2) {
		initucstr(ucstr, 0, ucprm2);
		tmp = calclen_uc(ucstr, stat) + 2;
		if (min > tmp) {
			min = tmp;
			optsub_saveprm(minprm, ucstr);
			rc = 2;
		}
	}
skip:
	rest0 = 0;
	for (i = 0; stat[i].count != 0; i++)
		rest0 += stat[i].count;
	max = stat[i - 1].code + 1;

	for (i = 0; i < 3; i++) {
		ucstr->base[0] = base0 = 0;
		st = stat;
		rest = rest0;
		j = 0;
		do {
			k = 0;
			if (j > 0) {
				if (i < 2)
					k = ucstr->lentbl[j - 1];
				if (i == 0 && k > 0)
					k++;
			}
			count = 0;
			tmpmin = 0x7fffffff;
			do {
				base1 = base0 + (1 << k);
				while (st->count != 0 && st->code < base1) {
					count += st->count;
					st++;
				}
				tmp = k * count;
				if (rest > count) {
					tmp += rest;
					if (i == 0) {
						if (k == 0)
							tmp += optsub_evaluate0(rest - count, st, base1, &dummy);
						else
							tmp += optsub_evaluate1(rest - count, st, base1, &dummy, k, 0);
					}
					if (i == 1)
						tmp += optsub_evaluate1(rest - count, st, base1, &dummy, k, -1);
					if (i == 2)
						tmp += optsub_evaluate2(rest - count, st, base1, &dummy);
				}
				if (tmpmin > tmp) {
					tmpmin = tmp;
					min_k = k;
					min_c = count;
					min_s = st;
				}
				k++;
			} while (rest > count);
			base0 += 1 << min_k;
			ucstr->lentbl[j] = min_k;
			ucstr->base[j + 1] = base0;
			rest -= min_c;
			st = min_s;
			j++;
		} while (rest > 0 && j < TEK1_MAXLEN);
		ucstr->maxlen = j - 1;
		if (rest > 0) {
			k = 0;
			if (i < 2)
				k = ucstr->lentbl[TEK1_MAXLEN - 1];
			if (i == 0 && k > 0)
				k++;
			count = 0;
			do {
				base1 = base0 + (1 << k);
				while (st->count != 0 && st->code < base1) {
					count += st->count;
					st++;
				}
				k++;
			} while (st->count != 0);
			k--;
			ucstr->lentbl[TEK1_MAXLEN] = k;
			ucstr->base[TEK1_MAXLEN + 1] = base0 + (1 << k);
			ucstr->maxlen = TEK1_MAXLEN;
		}
		tmp = calclen_uc(ucstr, stat) + (*func)(ucstr);
		if (min > tmp) {
			min = tmp;
			optsub_saveprm(minprm, ucstr);
			rc = -1;
		}

		/* 単純二分法 */
		st = stat;
		rest = rest0;
		j = 0;
		base0 = 0;
		do {
			k = 0;
			if (j > 0) {
				if (i < 2)
					k = ucstr->lentbl[j - 1];
				if (i == 0 && k > 0)
					k++;
			}
			tmp = 0;
			k--;
			do {
				k++;
				base1 = base0 + (1 << k);
				while (st->count != 0 && st->code < base1) {
					tmp += st->count;
					st++;
				}
			} while (tmp * 3 < rest);
			ucstr->lentbl[j] = k;
			ucstr->base[j + 1] = base0 = base1;
			rest -= tmp;
			j++;
		} while (rest > 0 && j < TEK1_MAXLEN);
		ucstr->maxlen = j - 1;
		if (rest > 0) {
			k = 0;
			if (i < 2)
				k = ucstr->lentbl[TEK1_MAXLEN - 1];
			if (i == 0 && k > 0)
				k++;
			tmp = 0;
			k--;
			do {
				k++;
				base1 = base0 + (1 << k);
				while (st->count != 0 && st->code < base1) {
					tmp += st->count;
					st++;
				}
			} while (rest > tmp);
			ucstr->lentbl[TEK1_MAXLEN] = k;
			ucstr->base[TEK1_MAXLEN + 1] = base1;
			ucstr->maxlen = TEK1_MAXLEN;
		}
		tmp = calclen_uc(ucstr, stat) + (*func)(ucstr);
		if (min > tmp) {
			min = tmp;
			optsub_saveprm(minprm, ucstr);
			rc = -1;
		}
	}

	/* 再帰を使う完全走査型 */
	if (complev >= 6) {
		optsub_ucstr = ucstr;
		optsub_max = max;
		optsub_min = min;
		optsub_minprm = minprm;
		optsub_func = func;
		if (complev == 6)
			optsub_fullopt1(0, rest0, stat, 0);
		else
			optsub_fullopt2(0, rest0, stat, 0);
		if (min > optsub_min) {
		//	min = optsub_min;
			rc = -1;
		}
	}

	optsub_inituc(ucstr, minprm);
fin:
	return rc;
}

#endif

static UCHAR tek2_invtrt[256];

int setstatistics1(struct STR_STATISTICS *stat, UCHAR *trt, unsigned int len, UCHAR *dat)
/* 当然sizは257以上とみなされる */
/* countでソートもされる */
{
	unsigned int count0[256], i, j, k, l, max, max_j;
	for (j = 0; j < 256; j++)
		count0[j] = 0;
	for (j = 0; j < len; j++)
		count0[dat[j]]++;
	k = 0;
	for (j = 0; j < 256; j++) {
		if (count0[j]) {
			stat[k].count = count0[j];
			stat[k].code  = j;
			k++;
		}
	}
	l = k;
	for (j = 0; j < 256; j++) { /* refresh用 */
		if (count0[j] == 0) {
			stat[l].count &= 0;
			stat[l].code  = j;
			trt[j] = l;
			l++;
		}
	}
	stat[256].count &= 0;
	for (i = k; i < 257; i++)
		stat[i].count &= 0; /* 存在判定に使うのでゼロクリア */
//	stat[k].count = 0; /* ターミネータ */
	/* 頭の悪いソート */
	/* とりあえずノンゼロだけでソートしているので少しは速い */
	for (i = 0; i < k; i++) {
		max = stat[i].count;
		max_j = i;
		for (j = i + 1; j < k; j++) {
			if (max < stat[j].count) {
				max = stat[j].count;
				max_j = j;
			}
		}
		stat[max_j].count = stat[i].count;
		stat[i].count = max;
		j = stat[max_j].code;
		stat[max_j].code = stat[i].code;
		stat[i].code = j;
		trt[j] = i;
	}
	for (i = 0; i < 256; i++) {
		tek2_invtrt[i] = stat[i].code;
		stat[i].code = i;
	}
	return k;
}

int setstatistics2(struct STR_STATISTICS *stat, int *trt, int *invtrt, unsigned int len, int *dat)
/* 当然sizは288以上とみなされる */
/* countでソートもされる */
{
	unsigned int count0[288], i, j, k, l, max, max_j;
	for (j = 0; j < 288; j++)
		count0[j] = 0;
	for (j = 0; j < len; j++)
		count0[dat[j]]++;
	k = 0;
	for (j = 0; j < 288; j++) {
		if (count0[j]) {
			stat[k].count = count0[j];
			stat[k].code  = j;
			k++;
		}
	}
	l = k;
	for (j = 0; j < 288; j++) { /* refresh用 */
		if (count0[j] == 0) {
			stat[l].count &= 0;
			stat[l].code  = j;
			trt[j] = l;
			l++;
		}
	}
	stat[288].count &= 0;
	for (i = k; i < 288 + 1; i++)
		stat[i].count &= 0; /* 存在判定に使うのでゼロクリア */
//	stat[k].count = 0; /* ターミネータ */
	for (i = 0; i < 288; i++) {
		trt[i] |= -1; /* まず、全て未使用に */
		invtrt[i] |= -1;
	}
	/* 頭の悪いソート */
	/* とりあえずノンゼロだけでソートしているので少しは速い */
	for (i = 0; i < k; i++) {
		max = stat[i].count;
		max_j = i;
		for (j = i + 1; j < k; j++) {
			if (max < stat[j].count) {
				max = stat[j].count;
				max_j = j;
			}
		}
		stat[max_j].count = stat[i].count;
		stat[i].count = max;
		j = stat[max_j].code;
		stat[max_j].code = stat[i].code;
		stat[i].code = j;
		trt[j] = i;	/* 外部コードを内部コードに変換するテーブル */
	}
	for (i = 0; i < 288; i++) {
		invtrt[i] = stat[i].code; /* 内部コードを外部コードに変換するテーブル */
		stat[i].code = i;
	}
	return k;
}

int setstatistics3(struct STR_STATISTICS *stat, unsigned int len, UCHAR *dat)
/* 当然sizは256以上とみなされる */
/* countでソートもされる */
{
	unsigned int count0[256], j, k;
	for (j = 0; j < 256; j++)
		count0[j] = 0;
	for (j = 0; j < len; j++)
		count0[dat[j]]++;
	k = 0;
	for (j = 0; j < 256; j++) {
		if (count0[j]) {
			stat[k].count = count0[j];
			stat[k].code  = j;
			k++;
		}
	}
	stat[k].count &= 0;
	return k;
}

static struct STR_STATISTICS *tek2_stat_calclen_ucstr_by;
extern UCHAR tek2_table_tr0[256], tek2_table_tr1[256];

int tek2_output_ucstr_by_sub(struct STR_UC *ucstr, UCHAR *lvt, int *pfix)
{
	int i, j, k, max = 0;
	for (i = 0; i < 256; i++)
		lvt[i] = 0x7f; /* 未使用マーク */
refresh:
	for (i = 0; tek2_stat_calclen_ucstr_by[i].count > 0; i++) {
		j = tek2_invtrt[i];
		for (k = 0; k < 32; k++) {
			if (ucstr->base[k] <= i && i < ucstr->base[k + 1])
				break;
		}
		lvt[j] = k;
		if (max < k)
			max = k;
	}
	*pfix = 0;
	if (ucstr->base[max + 1] >= 256) {
		for (i = 0; i < 256; i++) {
			if (lvt[i] == 0x7f) {
				lvt[i] = max;
				*pfix = 2;
			}
		}
		if (*pfix == 2) {
			for (i = 0; tek2_stat_calclen_ucstr_by[i].count > 0; i++);
			do {
				tek2_stat_calclen_ucstr_by[i].count = 1;
				i++;
			} while (i < 256);
			tek2_stat_calclen_ucstr_by[256].count &= 0;
			goto refresh;
		}
	} else {
		for (i = 0; i < 256; i++) {
			if (lvt[i] == 0x7f) {
				*pfix = 1;
				max++;
				for (; i < 256; i++) {
					if (lvt[i] == 0x7f)
						lvt[i] = max;
				}
			}
		}
	}
	return max;
}

int tek2_calclen_ucstr_by(struct STR_UC *ucstr)
{
	int max, fix;
	UCHAR lvt[256];
//	struct STR_STATISTICS *stat;
//	UCHAR table[256], outtmp[512], work[(512 + 272) * 34];
	max = tek2_output_ucstr_by_sub(ucstr, lvt, &fix);

/* 超頭悪い方法で出力させよう、とりあえず */
	return 1 + 4 + 4 + 8 + (tek1_intlog2p(max + 1) << 8); /* fix以降 */
//	return 3 + 5 + 3 + 2 + 2 + 8 + (intlog2p(max + 1) << 8);
		/* 利用レベルが14までに抑えられたら、これは131バイトになる */
		/* 利用レベルが6までに抑えられたら、これは99バイトになる */
/*
255を出力せよ

0:0(1bit)
1:2(2bit)
2:6(3bit)
3:14(4bit)
4:30(5bit)
5:62(6bit)
6:126(7bit)
7:254(1bit)
*/
	/* 一致などを抽出 */
//	return lzcompress_tek1s(256, table, 512, outtmp, sizeof work, work, 0x02 | intlog2m(max + 1) << 8, 12, 256);
	/* このときはbit数で返される、実際の出力はされない */
	/* このことで分かるように、符号類ではbytableが最初に出力される。 */
}

void tek2_output_ucstr_by(struct STR_BITBUF *bitbuf, struct STR_UC *ucstr)
{
	int i, max, fix;
	UCHAR lvt[256];
	putbitbuf(bitbuf, 3, max = tek1_intlog2p(tek2_output_ucstr_by_sub(ucstr, lvt, &fix) + 1));
	#if (defined(DEBUGMSG))
		printf("bitlen = log2p(max + 1) = %d\n", max);
		for (i = 0; i < 256; i++) printf("%x", lvt[i]);
		fputc('\n', stdout);
	#endif
	putbitbuf(bitbuf, 1, fix);
	putbitbuf(bitbuf, 4 + 4, 0); /* 000_0, 000_0 */
//	putbitbuf(bitbuf, 4 + 3 + 2 + 2, 0);
	putbitbuf(bitbuf, 8 - 1, 0); /* (1)0000000 */
	putbitbuf(bitbuf, 1, 1); /* 255 */
	for (i = 0; i < 256; i++)
		putbitbuf(bitbuf, max, lvt[i]);
	return;
}

void tek2_initucstr(UCHAR *table_lv, struct STR_UC *ucstr, int fix)
{
	int i, j, k, pop[TEK1_MAXLEN + 1];
	for (i = 0; i <= TEK1_MAXLEN; i++)
		pop[i] = 0;
	for (i = 0; i < 256; i++)
		pop[table_lv[i]]++;
	ucstr->base[0] = 0;
	for (i = 0; pop[i] > 0; i++) {
		ucstr->lentbl[i] = tek1_intlog2p(pop[i]);
		ucstr->base[i + 1] = ucstr->base[i] + (1 << ucstr->lentbl[i]);
	}
	ucstr->maxlen = i - 1 - fix; /* 制御ビット（旧len）の最高長 */

	j = 0;
	for (i = 0; pop[i] > 0; i++) {
		k = j;
		j += pop[i];
		pop[i] = k;
	}
	for (i = 0; i < 256; i++)
		tek2_table_tr0[pop[table_lv[i]]++] = i;
	return;
}

void tek2_fix_trt(struct STR_UC *ucstr, UCHAR *trt)
/* tek2_table_t[]を破壊 */
{
	int i, j, fix;
	UCHAR lvt[256], tmp[256];
	tek2_output_ucstr_by_sub(ucstr, lvt, &fix);
	tek2_initucstr(lvt, ucstr, fix);
//	for (i = 0; i < 256 && tek2_stat_calclen_ucstr_by[i].count > 0; i++) {
	for (i = 0; tek2_stat_calclen_ucstr_by[i].count > 0; i++) {
		j = tek2_table_tr0[i]; /* 内部コードiは、文字jに変換される */
		tmp[j] = i;
	}
	for (i = 0; i < 256; i++)
		trt[i] = tmp[i];
	return;
}

int output_lifetime(struct STR_BITBUF *btout, int lt0, int lt1, int len, int def0)
{
	if (lt1 < 0) {
		/* 最初 */
		if (lt0 < len && len <= (lt0 * 3) / 2)
			lt0 *= 2;
		if (lt0 >= len) {
			if (len < def0)
				lt0 = def0; /* 小さすぎをx1に引き上げる */
			if (def0 < len && len <= def0 * 2)
				lt0 = def0 * 2; /* x2で済むならx2にする */
		}
		if (lt0 == def0)
			putbitbuf(btout, 1, 0); /* lifetime x1 0 */
		else {
			if (lt0 >= len && lt0 != def0 * 2)
				putbitbuf(btout, 4, 0x0f); /* inf 111_1 */
			else if (lt0 == def0 / 32)
				putbitbuf(btout, 7, 0x7d /* 1_1_11_10_1 */);
			else if (lt0 == def0 / 16)
				putbitbuf(btout, 7, 0x75 /* 1_1_10_10_1 */);
			else if (lt0 == def0 / 8)
				putbitbuf(btout, 7, 0x6d /* 1_1_01_10_1 */);
			else if (lt0 == def0 / 4)
				putbitbuf(btout, 7, 0x65 /* 1_1_00_10_1 */);
			else if (lt0 == def0 / 2)
				putbitbuf(btout, 3, 0x03 /* 01_1 */);
			else if (lt0 == def0 * 2)
				putbitbuf(btout, 3, 0x01 /* 00_1 */);
			else if (lt0 == def0 * 4)
				putbitbuf(btout, 7, 0x45 /* 1_0_00_10_1 */);
			else if (lt0 == def0 * 8)
				putbitbuf(btout, 7, 0x4d /* 1_0_01_10_1 */);
			else if (lt0 == def0 * 16)
				putbitbuf(btout, 7, 0x55 /* 1_0_10_10_1 */);
			else if (lt0 == def0 * 32)
				putbitbuf(btout, 7, 0x5d /* 1_0_11_10_1 */);
			else
				exit(1); /* エラー */
		}
	} else {
		/* 2度目以降 */
		if (lt0 < len && len <= (lt0 * 3) / 2) {
			/* 2倍モード */
			lt0 *= 2;
			putbitbuf(btout, 3, 1); /* lifetime x2 00_1 */
		} else
			putbitbuf(btout, 1, 0); /* lifetime x1 0 */
	}
	return lt0;
}

int calclen_lifetime(int *plt0, int lt1, int len, int def0)
{
	UCHAR l;
	int lt0 = *plt0;
	if (lt1 < 0) {
		/* 最初 */
		if (lt0 < len && len <= (lt0 * 3) / 2)
			lt0 *= 2;
		if (lt0 >= len) {
			if (len < def0)
				lt0 = def0; /* 小さすぎをx1に引き上げる */
			if (def0 < len && len <= def0 * 2)
				lt0 = def0 * 2; /* x2で済むならx2にする */
		}
		if (lt0 == def0)
			l = 1;
		else {
			if (lt0 >= len && lt0 != def0 * 2)
				l = 4; /* inf 111_1 */
			else if (lt0 == def0 / 32)
				l = 7; /* 1_1_11_10_1 */
			else if (lt0 == def0 / 16)
				l = 7; /* 1_1_10_10_1 */
			else if (lt0 == def0 / 8)
				l = 7; /* 1_1_01_10_1 */
			else if (lt0 == def0 / 4)
				l = 7; /* 1_1_00_10_1 */
			else if (lt0 == def0 / 2)
				l = 3; /* 01_1 */
			else if (lt0 == def0 * 2)
				l = 3; /* 00_1 */
			else if (lt0 == def0 * 4)
				l = 7; /* 1_0_00_10_1 */
			else if (lt0 == def0 * 8)
				l = 7; /* 1_0_01_10_1 */
			else if (lt0 == def0 * 16)
				l = 7; /* 1_0_10_10_1 */
			else if (lt0 == def0 * 32)
				l = 7; /* 1_0_11_10_1 */
			else
				exit(1); /* エラー */
		}
	} else {
		/* 2度目以降 */
		if (lt0 < len && len <= (lt0 * 3) / 2) {
			/* 2倍モード */
			lt0 *= 2;
			l = 3; /* lifetime x2 00_1 */
		} else
			l = 1; /* lifetime x1 0 */
	}
	*plt0 = lt0;
	return (int) l;
}

//extern struct STR_UC tek2_ucstby[2];

int lzcompress_tek3h(int srcsiz, int *src, int outsiz, UCHAR *outbuf, UCHAR *work, int modecode)
/* workは3KB必要(intが32bitだとして) */
{
	int *s256 = (int *) work, i, j;
	UCHAR *p;
	struct STR_BITBUF btbuf;
	struct STR_UC ucstr;
	struct STR_STATISTICS *stat = (struct STR_STATISTICS *) (s256 + 256);
	tek1_s7ptr = outbuf;
	if (modecode >= 0)
		tek1_puts7s(modecode);
	tek1_puts7s(src[0]);
	p = tek1_s7ptr;
	if (srcsiz >= 2) {
		for (i = 0; i < srcsiz - 1; i++)
			s256[i] = src[i + 1] - src[i];
		for (i = srcsiz - 2; i >= 1; i--) {
			j = (s256[i] - s256[i - 1]) << 1;
			if (j < 0)
				j ^= -1;
			s256[i] = j;
		}
		/* s256のsrcsiz-1個のintに対して符号化する */
		setstatistics0(srcsiz, stat, srcsiz - 1, (unsigned int *)s256, 16);
		i = optimize_uc(&ucstr, stat, calclen_ucstr, tek1_ucprm_s4, tek1_ucprm_s41, NULL);
	//	i = optimize_uc(&ucstr, stat, calclen_ucstr, tek1_ucprm_s41);
		initbitbuf(&btbuf, &outbuf[outsiz] - tek1_s7ptr, tek1_s7ptr);
		putbitbuf(&btbuf, 1, i < 0);
		if (i < 0)
			output_ucstr(&btbuf, &ucstr);
		else
			putbitbuf(&btbuf, 1, i);

//printf("tek1h: (%x, l:%d) ", i, calclen_ucstr(&ucstr));
//for (i = ucstr.maxlen; i >= 0; i--) fputc(ucstr.lentbl[i] + ((ucstr.lentbl[i] < 10) ? 0x30 : 0x37), stdout);
//for (i = 0; i <= ucstr.maxlen + 1; i++) printf(" base[%d]=%x", i, ucstr.base[i]);

		for (i = 0; i < srcsiz - 1; i++)
			putnum_uc0(&btbuf, &ucstr, s256[i]);
		putbitbuf(&btbuf, 7, 0);
		p = btbuf.p;
	}
	return p - outbuf;
}

//extern UCHAR *comp_bt_c0, *comp_bt_c1;

#define LZSUB_STAT0_RESO	8

struct STR_LZSUB_STAT1 {
	UCHAR *src0, *src1, *l1table[256];
	int prm, maxdis, *table, btrees, repdis[4];
	struct STR_BTREE btree[LZSUB_STAT0_RESO];
	UCHAR *malloc_n0[LZSUB_STAT0_RESO], *malloc_n1[LZSUB_STAT0_RESO];
	int tree_lifetime, tree_phase, node0s, node1s, maxdis_r, lv;
};

/* seekはない。なぜなら一致したかどうかで動作を変える必要があるから */
/* でもsbuf0をNULLにしてもいいことにしようか */

void lzsub_stat1init(struct STR_LZSUB_STAT1 *work, UCHAR *src0, UCHAR *src1, int prm, int maxdis, int *table, int lv)
{
	int i, j;
	int srcsiz = src1 - src0;
	UCHAR *p;
	if (maxdis > srcsiz)
		maxdis = srcsiz;
	if (maxdis < 1024 * LZSUB_STAT0_RESO)
		maxdis = 1024 * LZSUB_STAT0_RESO;
	work->src0 = src0;
	work->src1 = src1;
	work->prm = prm;
	work->lv = lv;
	work->btrees = 1;
	work->maxdis_r = maxdis;
	work->table = table;
	if (maxdis < srcsiz) {
		/* 分割登録モード */
		work->maxdis_r /= LZSUB_STAT0_RESO;
		maxdis = work->maxdis_r * LZSUB_STAT0_RESO; /* 端数補正 */
		work->btrees = LZSUB_STAT0_RESO;
	}
	work->maxdis = maxdis;
	j = 2 * 1024 * 1024 / work->maxdis_r;
	work->node0s = TEK1_BT_NODES0 / j + 7;
	work->node1s = TEK1_BT_NODES1 / j + 7;
	for (i = 0; i < work->btrees; i++) {
		init_btree(&work->btree[i]);
		p = work->malloc_n0[i] = malloc(work->node0s * sizeof (struct STR_BT_NODE0));
		if (p == NULL)
			exit(1);
		p = work->malloc_n1[i] = malloc(work->node1s * sizeof (struct STR_BT_NODE1));
		if (p == NULL)
			exit(1);
		add_free1(&work->btree[i], (struct STR_BT_NODE1 *) p); /* 空っぽであることを示すために、1個だけ作っておく */
	}
	for (i = 0; i < 256; i++)
		work->l1table[i] = NULL; /* 一文字一致高速検索テーブル */
	for (i = 0; i < 4; i++)
		work->repdis[i] |= -1;
	work->tree_lifetime = 0;
	work->tree_phase = -1;
	return;
}

void lzsub_stat1end(struct STR_LZSUB_STAT1 *work)
{
	int i;
	for (i = 0; i < work->btrees; i++) {
		free(work->malloc_n0[i]);
		free(work->malloc_n1[i]);
	}
	return;
}

int lzsub_stat1enc(struct STR_LZSUB_STAT1 *work, UCHAR *src, UCHAR *src1, UCHAR *sbuf0, UCHAR *sbuf1)
/* sbuf0 == NULLだと読み進めて検索バッファを更新するのみで出力はしない */
{
	int j, k, len, range, mc[8], srchloglen = -1, srchlogdis = 0, maxlen, distance;
	UCHAR *sbuf = sbuf0, *p;
	len = 0;
	if (src <= work->src0)
		goto entry;
	goto fin;
	while (src < src1) {
#if 0
		for (j = 0; j < 2*0; j++) {
			if (*src == src[work->repdis[j]]) {
				/* repdis hit! */
				srchlogdis = work->repdis[j];
				len = matchlen_bt(src, &src[srchlogdis], work->src1);
				if (len < 2)
					continue;
				distance = -1 - j;
				if (len < 10 && work->table[len - 1] > 1 - j)
					continue;
printf("[%d %d] ", j, len);
				srchloglen = -1;
				if (len >= 3) {
				//	if (j >= 3)
				//		work->repdis[3] = work->repdis[2];
				//	if (j >= 2)
				//		work->repdis[2] = work->repdis[1];
					if (j >= 1)
						work->repdis[1] = work->repdis[0];
					work->repdis[0] = srchlogdis;
				}
				goto entry;
			}
		}
#endif
		maxlen = work->src1 - src;
		range = (src - work->src0) - work->maxdis;
		if (range < 0)
			range = 0;
		distance = srchlogdis;
		if ((len = srchloglen) < 0)
			len = search0c(work->prm, src, work->src0 + range, maxlen, &distance, work->table, work->btrees, work->btree, work->l1table);
		if (len > src1 - src)
			len = src1 - src; /* この範囲でならdisが改良できる可能性はあるが、面倒なので省略（search0cを改良すればいい） */
		srchloglen = -1;
		if (len >= 2) {
			range = (src - work->src0) + 1 - work->maxdis;
			if (range < 0)
				range = 0;
			srchloglen = search0c(work->prm, src + 1, work->src0 + range, maxlen - 1, &srchlogdis, work->table, work->btrees, work->btree, work->l1table);
			if (len < srchloglen) {
				len = 0;
				if (work->l1table[*src]) {
					distance = work->l1table[*src] - src;
					if (distance >= work->table[0])
						len = 1;
				}
			}
		}
#if 0
		if (len >= 3) {
		//	work->repdis[3] = work->repdis[2];
		//	work->repdis[2] = work->repdis[1];
			work->repdis[1] = work->repdis[0];
			work->repdis[0] = distance;
		}
#endif
	/* もう余計な加工はしない */
	//	if (len > 0)
	//		distance -= 4;
entry:
		/* btreeの更新 */
		j = len;
		if (len < 1)
			j = 1;
		mc[0] = j; /* 回数 */
		mc[1] = 1; /* コマンド */
		mc[2] = 0; /* ターミネーション */
		if (j >= 64 && work->lv <= 1) {
			mc[3] = 0;
			mc[5] = 1;
			mc[6] = 0;
			if (work->lv == 0) {
				mc[0] = 1;
				mc[2] = j - 2;
				mc[4] = 1;
			}
			if (work->lv == 1) {
				mc[0] = 16;
				mc[2] = j - 32;
				mc[4] = 16;
			}
		}
		j = 0;
		do {
			do {
				if (work->tree_lifetime <= 0) {
					/* btree[tree_phase]を壊して作り直し */
					work->tree_lifetime = work->maxdis_r;
					work->tree_phase = (work->tree_phase + 1) % LZSUB_STAT0_RESO;
					init_btree(&work->btree[work->tree_phase]);
					p = work->malloc_n0[work->tree_phase];
					for (k = 0; k < work->node0s; k++) {
						add_free0(&work->btree[work->tree_phase], (struct STR_BT_NODE0 *) p);
						p += sizeof (struct STR_BT_NODE0);
					}
					p = work->malloc_n1[work->tree_phase];
					for (k = 0; k < work->node1s; k++) {
						add_free1(&work->btree[work->tree_phase], (struct STR_BT_NODE1 *) p);
						p += sizeof (struct STR_BT_NODE1);
					}
				}
				work->tree_lifetime--;
				if (mc[j + 1])	/* 登録 */
					insert_val(&work->btree[work->tree_phase], src, work->src1);
				work->l1table[*src] = src;
				src++;
			} while (--mc[j]);
			j += 2;
		} while (mc[j] > 0);
		if (sbuf == NULL)
			goto skip_out;
		if (len <= 0) {
			sbuf[0] = 1;
			sbuf[1] = src[-1];
			sbuf += 2;
		} else {
			sbuf[0] = 2;
			distance ^= -1;
			if (distance <= 255) {
				sbuf[1] = 1;
				sbuf[2] = distance;
				sbuf += 3;
			} else if (distance <= 65535) {
				sbuf[1] = 2;
				sbuf[2] = (distance >>  8) & 0xff;
				sbuf[3] =  distance        & 0xff;
				sbuf += 4;
			} else {
				sbuf[1] = 4;
				sbuf[2] = (distance >> 24) & 0xff;
				sbuf[3] = (distance >> 16) & 0xff;
				sbuf[4] = (distance >>  8) & 0xff;
				sbuf[5] =  distance        & 0xff;
				sbuf += 6;
			}
			if (len <= 255) {
				sbuf[0] = 1;
				sbuf[1] = len;
				sbuf += 2;
			} else {
				sbuf[0] = 4;
				sbuf[1] = (len >> 24) & 0xff;
				sbuf[2] = (len >> 16) & 0xff;
				sbuf[3] = (len >>  8) & 0xff;
				sbuf[4] =  len        & 0xff;
				sbuf += 5;
			}
skip_out:
			if (len >= 2)
				srchloglen = -1;
		}
	}
//	if (sbuf)
//		*sbuf = 0;
fin:
	return sbuf - sbuf0;
}

int lzsub_stat1(int srcsiz, UCHAR *src, unsigned short *fdstbl, UCHAR *subbuf, int sbufsiz, UCHAR *sbuf0,
					int wrksiz, UCHAR *work, int prm, int maxdis, int submaxdis, int *table, UCHAR lv)
/* sbufにスライド辞書の基本情報を出力 */
/* lvは、0:1バイトのみ、1:16バイト、2:全部、がある。 */
/* btreeを使うが、バグ回避のためにdeleteはしない版。deleteのバグが直ったら、lzsub_stat1を作りたい。 */
/* workは最大のmaxdis:2MBに対して、果たしてどれほど必要なのか？ */
{
	struct STR_LZSUB_STAT1 fdis0, fdis1;
	int i, j, fds;
	UCHAR *sbuf1 = sbuf0 + sbufsiz, *sbuf = sbuf0, *p, *p1, *q;
	lzsub_stat1init(&fdis0, src, src + srcsiz, prm, maxdis, table, lv);
	if (fdstbl) {
		j = 0;
		for (i = 0; i < srcsiz; i++) {
			if (fdstbl[i])
				j++;
		}
		lzsub_stat1init(&fdis1, subbuf, subbuf + j, prm, submaxdis, table, lv);
	}
//	sbuf[0] = 3;
//	sbuf[1] = 0;
//	sbuf += 2;
	if (fdstbl == NULL)
		sbuf += lzsub_stat1enc(&fdis0, src, src + srcsiz, sbuf, sbuf1);
	else {
		p = src;
		p1 = src + srcsiz;
		q = subbuf;
		while (p < p1) {
			i = p - src;
			fds = fdstbl[i];
			j = i;
			for (i++; i < srcsiz && fds == fdstbl[i]; i++);
			i -= j;
			sbuf[0] = 3;
			if (fds <= 255) {
				sbuf[1] = 1;
				sbuf[2] = fds;
				sbuf += 3;
			} else {
				sbuf[1] = 4;
				sbuf[2] = (fds >> 24) & 0xff;
				sbuf[3] = (fds >> 16) & 0xff;
				sbuf[4] = (fds >>  8) & 0xff;
				sbuf[5] =  fds        & 0xff;
				sbuf += 6;
			}
//printf("(%d %d) ", fds, i);
			if (fds == 0)
				sbuf += lzsub_stat1enc(&fdis0, p, p + i, sbuf, sbuf1);
			else {
				sbuf += lzsub_stat1enc(&fdis1, q, q + i, sbuf, sbuf1);
				q += i;
				lzsub_stat1enc(&fdis0, p, p + i, NULL, sbuf1);
			}
			p += i;
		}
	}
	lzsub_stat1end(&fdis0);
	if (fdstbl)
		lzsub_stat1end(&fdis1);
	*sbuf = 0;
	return sbuf - sbuf0;
}

int lzsub_stat0(int srcsiz, UCHAR *src, int sbufsiz, UCHAR *sbuf0, int wrksiz, UCHAR *work, int prm, int maxdis, int *table, UCHAR lv)
/* sbufにスライド辞書の基本情報を出力 */
/* lvは、0:1バイトのみ、1:16バイト、2:全部、がある。 */
/* btreeを使うが、バグ回避のためにdeleteはしない版。deleteのバグが直ったら、lzsub_stat1を作りたい。 */
/* workは最大のmaxdis:2MBに対して、果たしてどれほど必要なのか？ */
{
	struct STR_LZSUB_STAT1 fdis0;
	UCHAR *sbuf1 = sbuf0 + sbufsiz, *sbuf = sbuf0;
	lzsub_stat1init(&fdis0, src, src + srcsiz, prm, maxdis, table, lv);
	sbuf += lzsub_stat1enc(&fdis0, src, src + srcsiz, sbuf, sbuf1);
	lzsub_stat1end(&fdis0);
	*sbuf = 0;
	return sbuf - sbuf0;
}

int lzsub_decstat(UCHAR *src, int *pdis, int *plen)
{
	int d = 0, l = 0, i;
	UCHAR *s0 = src;
	if (src[0] == 1) {
		/* 通常文字 */
		d = src[1];
		src += 2;
	} else if (src[0] == 2) {
		/* 文字一致 */
		i = src[1];
		src += 2;
		while (i > 0) {
			d = d << 8 | *src++;
			i--;
		}
		i = *src++;
		while (i > 0) {
			l = l << 8 | *src++;
			i--;
		}
	} else if (src[0] == 3) {
		/* fdis切り替え */
		l--;
		i = src[1];
		src += 2;
		while (i > 0) {
			d = d << 8 | *src++;
			i--;
		}
	}
	if (pdis)
		*pdis = d;
	if (plen)
		*plen = l;
	return src - s0;
}

/* 圧縮高速化のためのB木 */
/* ノードの削除に際して、pvalを修正しなくてよいと仮定 */

void init_btree(struct STR_BTREE *btree)
{
	btree->level = 0; /* topは数えない */
	btree->top = NULL;
	btree->free0 = NULL;
	btree->free1 = NULL;
	return;
}

void add_free0(struct STR_BTREE *btree, struct STR_BT_NODE0 *newnode)
{
	newnode->pkey[0] = btree->free0;
	btree->free0 = newnode;
	return;
}

void add_free1(struct STR_BTREE *btree, struct STR_BT_NODE1 *newnode)
{
	if (btree->top == NULL) {
		btree->top = newnode;
		newnode->nodes = 0;
		newnode->skiplen = 0;
	} else {
		newnode->pkey[0] = btree->free1;
		btree->free1 = newnode;
	}
	return;
}

//static UCHAR *comp_bt_c0 = NULL, *comp_bt_c1 = NULL;

int comp_bt(UCHAR *a, UCHAR *b, UCHAR *e)
{
//	UCHAR *p;
	if (a == b)
		return 0;
//	if (comp_bt_c0) {
//		if ((comp_bt_c0 <= a && a < comp_bt_c1) && (comp_bt_c0 <= b && b < comp_bt_c1)) {
//			p = a;
//			if (p < b)
//				p = b;
//			a += comp_bt_c1 - p;
//			b += comp_bt_c1 - p;
//		}
//	}
	a--;
	b--;
	do {
		a++;
		b++;
		if (a >= e)
			return -1;
		if (b >= e)
			return 1;
	} while (*a == *b);
	return *a - *b;
}

int matchlen_bt(UCHAR *a, UCHAR *b, UCHAR *e)
{
	UCHAR *a0 = a;
	if (a == b)
		return e - a;
	a--;
	b--;
	do {
		a++;
		b++;
		if (a >= e)
			break;
		if (b >= e)
			break;
	} while (*a == *b);
	return a - a0;
}

int test_pkey_sub(struct STR_BTREE *btree, struct STR_BT_NODE0 *node0, int lv, UCHAR *min, UCHAR *max, UCHAR *s1)
/* 大小関係は壊れていないものとする */
{
	struct STR_BT_NODE0 *node0c;
	int i, ec = 0;
	if (lv >= btree->level)
		return 0;
	for (i = 0; i < node0->nodes; i++) {
		node0c = node0->node[i];
		if (comp_bt(node0->pkey[i], node0c->pkey[0], s1) > 0)
			ec++;
		if (min != NULL && comp_bt(min, node0c->pkey[0], s1) > 0)
			ec++;
		if (i < node0->nodes - 1 && comp_bt(node0c->pkey[node0c->nodes - 1], node0->pkey[i + 1], s1) >= 0)
			ec++;
		if (max != NULL && comp_bt(node0c->pkey[node0c->nodes - 1], max, s1) >= 0)
			ec++;
		ec += test_pkey_sub(btree, node0c, lv + 1, node0->pkey[i], (i < node0->nodes - 1) ? node0->pkey[i + 1] : max, s1);
	}
	return ec;
}

void test_pkey(struct STR_BTREE *btree, UCHAR *s1)
{
	int ec;
	struct STR_BT_NODE0 *node0 = btree->top;

	ec = test_pkey_sub(btree, node0, 0, node0->pkey[0], NULL, s1);
	if (ec)
		printf("#%d ", ec);
	return;
}

void search0(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle, UCHAR *s, UCHAR *s1)
{
	struct STR_BT_NODE0 *node0;
	struct STR_BT_NODE1 *node1;
	int min, max, middle, i, lv1 = btree->level, lv = 0, skiplen = 0;

	node0 = btree->top;
	while (lv < lv1) {
		min = 0;
		max = node0->nodes - 1;
		if (comp_bt(s + skiplen, node0->pkey[0] + skiplen, s1) <= 0)
			goto skip0;
		if (comp_bt(s + skiplen, node0->pkey[max] + skiplen, s1) >= 0) {
			min = max;
			goto skip0;
		}
		skiplen = node0->skiplen;
		while (min < max - 1) {
			middle = (min + max) / 2;
			i = comp_bt(s + skiplen, node0->pkey[middle] + skiplen, s1);
			if (i < 0)
				max = middle;
			else
				min = middle;
		}
skip0:
		handle->node[lv] = node0;
		handle->pos[lv] = min;
		lv++;
		node0 = node0->node[min];
	}

	/* node1内を検索 */
	node1 = (struct STR_BT_NODE1 *) node0;
	min = 0;
	max = node1->nodes - 1;
	if (comp_bt(s + skiplen, node1->pkey[0] + skiplen, s1) <= 0)
		goto skip1;
	if (comp_bt(s + skiplen, node1->pkey[max] + skiplen, s1) >= 0) {
		min = max;
		goto skip1;
	}
	skiplen = node1->skiplen;
	while (min < max - 1) {
		middle = (min + max) / 2;
		i = comp_bt(s + skiplen, node1->pkey[middle] + skiplen, s1);
		if (i < 0)
			max = middle;
		else
			min = middle;
	}
skip1:
	handle->node[lv] = node1;
	handle->pos[lv] = min;
	handle->pkey = node1->pkey[min];
	return;
//	return matchlen_bt(s, handle->pkey, s1); /* 最長一致長(nextが最長一致かもしれないけど) */
}

#if 0

void search0_min(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle)
/* デバッグ用 */
{
	struct STR_BT_NODE0 *node0;
	struct STR_BT_NODE1 *node1;
	int min, lv1 = btree->level, lv = 0;

	node0 = btree->top;
	while (lv < lv1) {
		min = 0;
		handle->node[lv] = node0;
		handle->pos[lv] = min;
		lv++;
		node0 = node0->node[min];
	}

	/* node1内を検索 */
	node1 = (struct STR_BT_NODE1 *) node0;
	min = 0;
	handle->node[lv] = node1;
	handle->pos[lv] = min;
	handle->pkey = node1->pkey[min];
	return;
}

#endif

int search_back(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle)
/* 失敗したら1 */
{
	struct STR_BT_NODE0 *node0;
	struct STR_BT_NODE1 *node1;
	int i, lv1 = btree->level, lv;
	if (handle->pos[lv1] > 0) {
		i = --(handle->pos[lv1]);
		node1 = handle->node[lv1];
		handle->pkey = node1->pkey[i];
		return 0;
	}
	lv = lv1 - 1;
	while (lv >= 0) {
		node0 = handle->node[lv];
		if (handle->pos[lv] > 0) {
			i = --(handle->pos[lv]);
			node0 = node0->node[i];
			lv++;
			while (lv < lv1) {
				handle->node[lv] = node0;
				i = node0->nodes - 1;
				handle->pos[lv] = i;
				node0 = node0->node[i];
				lv++;
			}
			node1 = (struct STR_BT_NODE1 *) node0;
			handle->node[lv] = node1;
			i = node1->nodes - 1;
			handle->pos[lv] = i;
			handle->pkey = node1->pkey[i];
			return 0;
		}
		lv--;		
	}
	return 1; /* 何も書き換えてない */
}

int search_next(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle)
/* 失敗したら1 */
{
	struct STR_BT_NODE0 *node0;
	struct STR_BT_NODE1 *node1;
	int i, lv1 = btree->level, lv, max;
	node1 = handle->node[lv1];
	max = node1->nodes - 1;
	if (handle->pos[lv1] < max) {
		i = ++(handle->pos[lv1]);
		handle->pkey = node1->pkey[i];
		return 0;
	}
	lv = lv1 - 1;
	while (lv >= 0) {
		node0 = handle->node[lv];
		max = node0->nodes - 1;
		if (handle->pos[lv] < max) {
			i = ++(handle->pos[lv]);
			node0 = node0->node[i];
			lv++;
			while (lv < lv1) {
				handle->node[lv] = node0;
				handle->pos[lv] = 0;
				node0 = node0->node[0];
				lv++;
			}
			node1 = (struct STR_BT_NODE1 *) node0;
			handle->node[lv] = node1;
			handle->pos[lv] = 0;
			handle->pkey = node1->pkey[0];
			return 0;
		}
		lv--;
	}
	return 1; /* 何も書き換えてない */
}

void btsub_setskiplen(void *node, UCHAR *s1, int flag)
/* フラグは親が使えない状況だと1 */
{
	struct STR_BT_NODE1 *node1 = node;
	node1->skiplen = matchlen_bt(node1->pkey[0], node1->pkey[node1->nodes - 1], s1);
	return;
}

void btsub_setmin(struct STR_BTREE *btree, struct STR_BT_HANDLE *handle, void *node, int lv, UCHAR *s1)
/* nodeは最小値が変わった、lvに属している、親はたどってくれ */
{
	struct STR_BT_NODE1 *node1 = node;
	UCHAR *min = node1->pkey[0];
	if (lv <= 0)
		return;
	lv--;
	do {
		int i = handle->pos[lv];
		node1 = handle->node[lv];
		node1->pkey[i] = min;
		if (i > 0)
			break;
		btsub_setskiplen(node1, s1, 0);
		lv--;
	} while (lv >= 0);
	return;
}

int insert_val(struct STR_BTREE *btree, UCHAR *s, UCHAR *s1)
/* 失敗したら1 */
{
//static int debug = 0; /* 1669で死ぬ */
	struct STR_BT_HANDLE handle;
	struct STR_BT_NODE0 *node0, *free0, *node00;
	struct STR_BT_NODE1 *node1, *free1;
	int lv, lv1, max, i, j;


//UCHAR *deb[64 * 1024], **p;
//search0_min(btree, &handle);
//i = 0;
//p = deb; do { *p = handle.pkey; if (p > deb && comp_bt(p[-1], *p, s1) >= 0) i++; p++; } while (search_next(btree, &handle) == 0); *p = NULL;
//if (i > 0) { printf("[%d] ", i); for (p = deb; *p; p++) printf("%08X ", *p); fputc('\n', stdout); exit(1); }
//test_pkey(btree, s1);

//debug++;
//if ((debug & 4095 & 0) == 0) printf("\r%d", debug);
	node1 = btree->top;
	if (node1->nodes == 0) {
		node1->nodes = 1;
		node1->pkey[0] = s;
		node1->skiplen = 0;
//fputc('*', stdout);
		return 0;
	}
retry:
//if (debug==14274) printf("pass1:");
	lv1 = btree->level;
	search0(btree, &handle, s, s1);
	node1 = handle.node[lv1];
	max = node1->nodes;
	if (max < TEK1_BT_NODESIZ) {
		/* 分割の必要なし */
		node1->nodes++;
		i = handle.pos[lv1] + 1;
		if (i == 1 && comp_bt(s, handle.pkey, s1) < 0)
			i = 0;
		for (j = max - 1; j >= i; j--)
			node1->pkey[j + 1] = node1->pkey[j];
		node1->pkey[i] = s;
		if (i == 0 || i == max)
			node1->skiplen = matchlen_bt(node1->pkey[0], node1->pkey[max], s1); /* 親がいたら親のskiplenを利用するともっとよい（親の右端の子は利用不能なのに注意） */
		if (i > 0) {
//fputc(',', stdout);
//if (debug > 14000) fputc('o', stdout);
			return 0;
		}
		/* 最小値の更新 */
		lv = lv1 - 1;
		while (lv >= 0) {
			node0 = handle.node[lv];
			i = handle.pos[lv];
			if (comp_bt(s, node0->pkey[i], s1) >= 0)
				break;
			node0->pkey[i] = s;
			if (i > 0)
				break;
			node0->skiplen = matchlen_bt(node0->pkey[0], node0->pkey[node0->nodes - 1], s1); /* 親利用可能 */
			lv--;
		}
//if (debug > 14000) fputc('o', stdout);
//fputc('e', stdout);
		return 0;
	}
//fputc('f', stdout);
//if (debug==14274) printf("pass2:");
	/* 分割(node1) */
	if (lv1 == 0) {
		free0 = btree->free0;
		if (free0 == NULL) {
err:
			return 1;
		}
		free1 = btree->free1;
		if (free1 == NULL)
			goto err;
		btree->free0 = free0->pkey[0];
		btree->free1 = free1->pkey[0];

		for (i = 0; i < TEK1_BT_NODESIZ / 2; i++)
			free1->pkey[i] = node1->pkey[i + TEK1_BT_NODESIZ / 2];
		node1->nodes = TEK1_BT_NODESIZ / 2;
		free1->nodes = TEK1_BT_NODESIZ / 2;
		free0->node[0] = node1;
		free0->node[1] = free1;
		free0->pkey[0] = node1->pkey[0];
		free0->pkey[1] = free1->pkey[0];
		free0->nodes = 2;
		free0->skiplen = matchlen_bt(free0->pkey[0], free0->pkey[1], s1);
		node1->skiplen = matchlen_bt(node1->pkey[0], node1->pkey[TEK1_BT_NODESIZ / 2 - 1], s1); /* 親利用可能 */
		free1->skiplen = matchlen_bt(free1->pkey[0], free1->pkey[TEK1_BT_NODESIZ / 2 - 1], s1);
		btree->top = free0;
		btree->level = 1;
		goto retry;
	}
	lv = lv1 - 1;
	node0 = handle.node[lv];
	max = node0->nodes;
//if (debug==14274) printf("pass3:");
	if (max < TEK1_BT_NODESIZ) {
		free1 = btree->free1;
		if (free1 == NULL)
			goto err;
		btree->free1 = free1->pkey[0];

		for (i = 0; i < TEK1_BT_NODESIZ / 2; i++)
			free1->pkey[i] = node1->pkey[i + TEK1_BT_NODESIZ / 2];
		node1->nodes = TEK1_BT_NODESIZ / 2;
		free1->nodes = TEK1_BT_NODESIZ / 2;
		for (i = max - 1; i > handle.pos[lv]; i--) {
			node0->pkey[i + 1] = node0->pkey[i];
			node0->node[i + 1] = node0->node[i];
		}
		i = handle.pos[lv] + 1;
		node0->node[i] = free1;
		node0->pkey[i] = free1->pkey[0];
		if (i == 0 || i == node0->nodes)
			node0->skiplen = matchlen_bt(node0->pkey[0], node0->pkey[node0->nodes], s1); /* 親利用可能 */
		node1->skiplen = matchlen_bt(node1->pkey[0], node1->pkey[TEK1_BT_NODESIZ / 2 - 1], s1); /* 親利用可能 */
		free1->skiplen = matchlen_bt(free1->pkey[0], free1->pkey[TEK1_BT_NODESIZ / 2 - 1], s1); /* 親利用可能 */
		node0->nodes++;
		goto retry;
	}
	lv--;
//if (debug==14274) printf("pass4:");
	while (lv >= 0) {
		node00 = handle.node[lv];
		if (node00->nodes < TEK1_BT_NODESIZ) {
			node0 = handle.node[lv + 1];
			free0 = btree->free0;
			if (free0 == NULL)
				goto err;
			btree->free0 = free0->pkey[0];
			for (i = 0; i < TEK1_BT_NODESIZ / 2; i++) {
				free0->pkey[i] = node0->pkey[i + TEK1_BT_NODESIZ / 2];
				free0->node[i] = node0->node[i + TEK1_BT_NODESIZ / 2];
			}
			node0->nodes = TEK1_BT_NODESIZ / 2;
			free0->nodes = TEK1_BT_NODESIZ / 2;
			for (i = node00->nodes - 1; i > handle.pos[lv]; i--) {
				node00->pkey[i + 1] = node00->pkey[i];
				node00->node[i + 1] = node00->node[i];
			}
			i = handle.pos[lv] + 1;
			node00->node[i] = free0;
			node00->pkey[i] = free0->pkey[0];
			if (i == 0 || i == node00->nodes)
				node00->skiplen = matchlen_bt(node00->pkey[0], node00->pkey[node00->nodes], s1); /* 親利用可能 */
			node0->skiplen = matchlen_bt(node0->pkey[0], node0->pkey[TEK1_BT_NODESIZ / 2 - 1], s1); /* 親利用可能 */
			free0->skiplen = matchlen_bt(free0->pkey[0], free0->pkey[TEK1_BT_NODESIZ / 2 - 1], s1); /* 親利用可能 */
			node00->nodes++;
			goto retry;
		}
		lv--;
	}
//if (debug==14274) printf("pass5:");
	/* 分割(node0) */
	node0 = btree->top;
	free0 = btree->free0;
	if (free0 == NULL)
		goto err;
	node00 = free0->pkey[0];
	if (node00 == NULL)
		goto err;
	btree->free0 = node00->pkey[0];
	btree->level++;
	for (i = 0; i < TEK1_BT_NODESIZ / 2; i++) {
		free0->pkey[i] = node0->pkey[i + TEK1_BT_NODESIZ / 2];
		free0->node[i] = node0->node[i + TEK1_BT_NODESIZ / 2];
	}
	node0->nodes = TEK1_BT_NODESIZ / 2;
	free0->nodes = TEK1_BT_NODESIZ / 2;
	node00->nodes = 2;
	node00->pkey[0] = node0->pkey[0];
	node00->pkey[1] = free0->pkey[0];
	node00->node[0] = node0;
	node00->node[1] = free0;
	btree->top = node00;
	node00->skiplen = matchlen_bt(node00->pkey[0], node00->pkey[1], s1);
	node0->skiplen = matchlen_bt(node0->pkey[0], node0->pkey[TEK1_BT_NODESIZ / 2 - 1], s1); /* 親利用可能 */
	free0->skiplen = matchlen_bt(free0->pkey[0], free0->pkey[TEK1_BT_NODESIZ / 2 - 1], s1);
	goto retry;
}

int loadfdis(int siz, unsigned short *tbl);
void setsubbuf(int siz, UCHAR *src, unsigned short *fdstbl, UCHAR *subbuf);

int setbylen(UCHAR *sbuf, int len0, UCHAR **sbuf1, int mode)
/* mode == 0 : tek3sモード */
{
	int l = 0, i, dis, len /* , tl, td */;
	for (;;) {
		i = lzsub_decstat(sbuf, &dis, &len);
		if (i == 0)
			break;
		if (len < 0)
			break; /* fdis変更 */
		if (len >= len0)
			break;

#if 0
		if (len >= len0) {
			if (mode > 0)
				break;
			if (lzsub_decstat(sbuf + i, &tl, &td) <= 0)
				tl = 0;
			if (mode == 0) {
				/* tek3sモード */
				td = len0;
				if (tl < len0) {
					/* lz == 1だ */
					td++; /* それくらいなら切り替えないほうがマシなケースが多いから */
						/* 条件を厳しくする */
				}
				if (td <= len && len <= len0 + 4 && dis < 64)
					break; /* 少なくとも1バイトは得するだろう */
				if (td + 1 <= len && len <= len0 + 31 && dis < 256)
					break; /* 少なくとも1バイトは得するだろう */
				if (td + 2 <= len && len <= len0 + 31 && dis < 65536 + 256)
					break; /* 少なくとも1バイトは得するだろう */
			//	if (td + 2 <= len && dis < 256)
			//		break; /* 少なくとも1バイトは得するだろう */
				if (td + 3 <= len && dis < 256 + 32768)
					break; /* 少なくとも1バイトは得するだろう */
				if (td + 4 <= len && dis < 0x408100)
					break; /* 少なくとも1バイトは得するだろう */
				if (td + 5 <= len && dis < 0x20408100)
					break; /* 少なくとも1バイトは得するだろう */
			}
		}
#endif
		if (len == 0)
			len++;
		l += len;
		sbuf += i;
	}
	if (sbuf1)
		*sbuf1 = sbuf;
	return l;
}

int setlzlen(UCHAR *sbuf, int len0, UCHAR **sbuf1)
{
	int l = 0, i, dis, len;
	for (;;) {
		i = lzsub_decstat(sbuf, &dis, &len);
		if (i == 0)
			break;
		if (len < len0)
			break;
		l++;
		sbuf += i;
	}
	if (sbuf1)
		*sbuf1 = sbuf;
	return l;
}

int setflen(UCHAR *sbuf, int len0, UCHAR **sbuf1, int mode)
{
	int l = 0, i, dis, len;
	while (*sbuf != 3 && *sbuf != 0) {
		i = lzsub_decstat(sbuf, &dis, &len);
		if (len < len0)
			setbylen(sbuf, len0, &sbuf, mode);
		else
			setlzlen(sbuf, len0, &sbuf);
		l++;
	}
	return l;
}

int lzcompress_tek1s(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int opt, int prm, int maxdis, int submaxdis)
/* opt:0 -- 圧縮率優先 */
/* opt:1 -- 展開速度優先 */
/* 今はもうopt == 0でも展開速度と圧縮率を両立させている */
/* とりあえず、workは(srcsiz + 257) * 2 * sizeof (int)でいいらしい。 */
{
	/* ds3:cp4 */
	static int table[11] = {
		0, 0, -0x8, -0x408, -0x20408, -0x1020408,
		-0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff
	};
	int i, j, k, len, dis;
	int jj, kk;
	UCHAR *p, *sbuf0, *sbuf1, *sbuf, *t;
	#if (defined(NOWARN))
		k = 0;
	#endif
	sbuf0 = (UCHAR *) work;
	i = 0;
	if (complev >= 4)
		i = 1;
	if (complev >= 5)
		i = 2;
	sbuf1 = sbuf0 + lzsub_stat1(srcsiz, src, NULL, NULL, (srcsiz + 257) * 2 * sizeof (int), sbuf0, 0, NULL, prm, maxdis, submaxdis, table + 1 - 0, i);

	sbuf = sbuf0;
	tek1_s7ptr = outbuf;
	p = src;
	if (*sbuf == 3)	/* いきなりfdis切り替え */
		sbuf += lzsub_decstat(sbuf, &dis, &len);
	i = lzsub_decstat(sbuf, &dis, &len);

	*tek1_s7ptr++ = 0x01; /* flen+スペシャルキャラクタ */
	while (i > 0) {
		jj = j = setbylen(sbuf, 2, &t, 0);
		kk = k = setlzlen(t, 2, NULL);
		if (j >= 16)
			jj = 0;
		if (k >= 16)
			kk = 0;
		*tek1_s7ptr++ = kk << 4 | jj;
		if (jj == 0)
			tek1_puts7s(j); /* パディングしたいかもしれないからs7sで */
		if (kk == 0)
			tek1_puts7s(k); /* パディングしたいかもしれないからs7sで */
		do {
			*tek1_s7ptr++ = *p++;
		} while (--j);
		sbuf = t;
		i = lzsub_decstat(sbuf, &dis, &len);
		if (i == 0)
			break;
		do {
			/* ds3:cp4 */
			jj = tek1_intlog2p(dis + 1); /* disを格納するのに必要なbit数 */
			for (kk = 0; jj - kk > 3; kk += 7); /* 下位kkbitはs7sに任せる */
			jj = len - 1;
			if (jj >= 16)
				jj = 0;
			*tek1_s7ptr++ = (kk == 0) | (dis >> kk) << 1 | jj << 4;
			if (kk > 0) {
				/* puts7sだと上位の0を省略しかねないので自前で */
				if (kk > 28)
					*tek1_s7ptr++ = ((dis >> 28) & 0x7f) << 1;
				if (kk > 21)
					*tek1_s7ptr++ = ((dis >> 21) & 0x7f) << 1;
				if (kk > 14)
					*tek1_s7ptr++ = ((dis >> 14) & 0x7f) << 1;
				if (kk > 7)
					*tek1_s7ptr++ = ((dis >> 7) & 0x7f) << 1;
				*tek1_s7ptr++ = (dis & 0x7f) << 1 | 1;
			}
			if (jj == 0)
				tek1_puts7s(len - 1);
			sbuf += i;
			p += len;
			i = lzsub_decstat(sbuf, &dis, &len);
		} while (--k);
	}
	return tek1_s7ptr - outbuf;
}

#define TEK2S_DS0	6

int lzcompress_tek2s(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int opt, int prm, int maxdis, int submaxdis)
/* opt:0 -- 圧縮率優先 */
/* opt:1 -- 展開速度優先 */
/* 今はもうopt == 0でも展開速度と圧縮率を両立させている */
{
	/* ds3:cp4 */
	static int table[11] = {
		0 + TEK2S_DS0, 0 + TEK2S_DS0, -0x8 + TEK2S_DS0, -0x408 + TEK2S_DS0, -0x20408 + TEK2S_DS0, -0x1020408 + TEK2S_DS0,
		-0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff, -0x7fffffff
	};
	int i, j, k, len, dis, flen, fdis; // , sll = 1, sdis1, slen1, slen2; // maxdissub(MDS)
	int repdis[2][4], jj, kk, fmode = 0;
	UCHAR *p, *sbuf0, *sbuf1, *sbuf, *q, *subbuf, *t, bylz[2], *pbylz[2]; // , flag = 1;
	signed char cbylz[2];
	unsigned short *fdstbl;
	#if (defined(NOWARN))
		k = 0;
	#endif
	sbuf0 = (UCHAR *) work;
	fdstbl = (unsigned short *) (sbuf0 + (srcsiz + 256) * 2);
	subbuf = ((UCHAR *) fdstbl) + srcsiz * 2;
	if (hint == NULL)
		fdstbl = NULL;
	else {
		if (loadfdis(srcsiz, fdstbl))
			fdstbl = NULL; /* 何かトラブルがあったか、もしくは結局差分必要なし */
	}

	i = 0;
	if (complev >= 4)
		i = 1;
	if (complev >= 5)
		i = 2;
	if (fdstbl)
		setsubbuf(srcsiz, src, fdstbl, subbuf); /* fdisは最高で65535 */
	sbuf1 = sbuf0 + lzsub_stat1(srcsiz, src, fdstbl, subbuf, (srcsiz + 257) * 2 * sizeof (int), sbuf0, 0, NULL, prm, maxdis, submaxdis, table + 1 - 0, i);

//for (sbuf = sbuf0; ;) { i = lzsub_decstat(sbuf, &dis, &len); printf("[%d %d] ", dis, len); sbuf += i; if (i == 0) break; }

	sbuf = sbuf0;
	tek1_s7ptr = outbuf;
	p = src;
	q = subbuf;
	flen = 0x7fffffff;
	fdis = 0;
	if (*sbuf == 3)	/* いきなりfdis切り替え */
		sbuf += lzsub_decstat(sbuf, &dis, &len);
	if (fdstbl == NULL)
		*tek1_s7ptr++ = 0x01; /* fdis0無限 */
	else {
		flen = setflen(sbuf, 2, NULL, 0);
		tek1_puts7s(flen);
	}
	for (i = 0; i < 4; i++)
		repdis[0][i] = repdis[1][i] = i;
	i = lzsub_decstat(sbuf, &dis, &len);

	cbylz[0] = cbylz[1] = 0;
//	hbylz[0] = hbylz[1] = 1; /* 最初の1bitは破棄しなければいけないから */
	bylz[0] = bylz[1] = 0;
	fmode = (fdis != 0);

	while (i > 0) {
		if (flen == 0) {
/* 以下は要再検討 */
			sbuf += i;
			flen = setflen(sbuf, 2, NULL, 0);
			fdis = dis;
			j = flen;
			if (flen >= 16)
				j = 0;
			k = fdis + 1;
			if (k >= 8)
				k = 0;
			i = lzsub_decstat(sbuf, &dis, &len);
			if (len >= 2)
				j |= 0x10; /* いきなりlzから始まる */
			*tek1_s7ptr++ = j | k << 5;
			if (j == 0)
				tek1_puts7(flen - 16);
			if (k == 0)
				tek1_puts7(fdis - 7);
			fdis = - fdis;
			if (len >= 2) {
				k = setlzlen(sbuf, 2, NULL);
				tek1_puts7s(k - 1);
				goto skip_by;
			}
		}

		/* byフェーズ */
		j = setbylen(sbuf, 2, &t, 0);
		jj = j;
		if (jj > 17)
			jj = 17;
		if (--jj) {
			do {
			//	if (hbylz[fmode])
			//		hbylz[fmode]--;
			//	else {
					if (cbylz[fmode] == 8) {
						*(pbylz[fmode]) = bylz[fmode];
						cbylz[fmode] = 0;
						bylz[fmode] = 0;
					}
					if (cbylz[fmode] == 0) {
						pbylz[fmode] = tek1_s7ptr;
						tek1_s7ptr++;
					}
				//	bylz[fmode] |= 0 << cbylz[fmode];
					cbylz[fmode]++;
			//	}
			} while (--jj);
		}
		if (j >= 17) {
			tek1_puts7s(j - 17);
		} else {
			/* 切れ目を認識させるために、1bit挿入 */
			if (cbylz[fmode] == 8) {
				*(pbylz[fmode]) = bylz[fmode];
				cbylz[fmode] = 0;
				bylz[fmode] = 0;
			}
			if (cbylz[fmode] == 0) {
				pbylz[fmode] = tek1_s7ptr;
				tek1_s7ptr++;
			}
			bylz[fmode] |= 1 << cbylz[fmode];
			cbylz[fmode]++;
		}

		if (fdis == 0) {
			do {
				*tek1_s7ptr++ = *p++;
			} while (--j);
		} else {
			p += j;
			do {
				*tek1_s7ptr++ = *q++;
			} while (--j);
		}
		sbuf = t;
		i = lzsub_decstat(sbuf, &dis, &len);
		if (i == 0)
			break;
		flen--;
		if (flen == 0) {
			i = lzsub_decstat(sbuf, &dis, &len);
			continue;
		}
skip_by:

		/* lzフェーズ */
		k = setlzlen(sbuf, 2, NULL);
		jj = k;
		if (jj > 17)
			jj = 17;
		if (--jj) {
			do {
				if (cbylz[fmode] == 8) {
					*(pbylz[fmode]) = bylz[fmode];
					cbylz[fmode] = 0;
					bylz[fmode] = 0;
				}
				if (cbylz[fmode] == 0) {
					pbylz[fmode] = tek1_s7ptr;
					tek1_s7ptr++;
				}
				cbylz[fmode]++;
			} while (--jj);
		}
		if (k >= 17) {
			tek1_puts7s(k - 17);
		} else {
			/* 切れ目を認識させるために、1bit挿入 */
			if (cbylz[fmode] == 8) {
				*(pbylz[fmode]) = bylz[fmode];
				cbylz[fmode] = 0;
				bylz[fmode] = 0;
			}
			if (cbylz[fmode] == 0) {
				pbylz[fmode] = tek1_s7ptr;
				tek1_s7ptr++;
			}
			bylz[fmode] |= 1 << cbylz[fmode];
			cbylz[fmode]++;
		}
		do {
#if 1
			if (repdis[fmode][0] == dis)
				dis = 0 - TEK2S_DS0;
			else if (repdis[fmode][1] == dis) {
				repdis[fmode][1] = repdis[fmode][0];
				repdis[fmode][0] = dis;
				dis = 1 - TEK2S_DS0;
			} else if (repdis[j][2] == dis) {
				repdis[fmode][2] = repdis[fmode][1];
				repdis[fmode][1] = repdis[fmode][0];
				repdis[fmode][0] = dis;
				dis = 2 - TEK2S_DS0;
			} else if (repdis[j][3] == dis) {
				repdis[fmode][3] = repdis[fmode][2];
				repdis[fmode][2] = repdis[fmode][1];
				repdis[fmode][1] = repdis[fmode][0];
				repdis[fmode][0] = dis;
				dis = 3 - TEK2S_DS0;
			} else {
				repdis[fmode][3] = repdis[fmode][2];
				repdis[fmode][2] = repdis[fmode][1];
				repdis[fmode][1] = repdis[fmode][0];
				repdis[fmode][0] = dis;
				if (dis >= 0x408 - TEK2S_DS0) {
					/* 2バイト以上のdsバイト */
					if (repdis[fmode][1] < dis && dis <= repdis[fmode][1] + 128) {
						kk = dis - repdis[fmode][1] - 1;
						dis = 4 - TEK2S_DS0;
					} else if (repdis[fmode][1] - 128 <= dis && dis < repdis[fmode][1]) {
						kk = repdis[fmode][1] - dis - 1;
						dis = 5 - TEK2S_DS0;
					}
				}
				if (dis >= 0x20408 - TEK2S_DS0) {
					/* 3バイト以上のdsバイト */
					if (repdis[fmode][1] < dis && dis <= repdis[fmode][1] + 0x4080) {
						kk = dis - repdis[fmode][1] - 1;
						dis = 4 - TEK2S_DS0;
					} else if (repdis[fmode][1] - 0x4080 <= dis && dis < repdis[fmode][1]) {
						kk = repdis[fmode][1] - dis - 1;
						dis = 5 - TEK2S_DS0;
					}
				}
			}
#endif
			dis += TEK2S_DS0;
			/* ds3:cp4 */
#if 1
			jj = len - 1;
			if (jj >= 16)
				jj = 0;
			*tek1_s7ptr++ = (dis < 8) | (dis & 0x07) << 1 | jj << 4;
			if (dis >= 8)
				tek1_puts7((dis >> 3) - 1);
			if (4 <= dis && dis <= 5)
				tek1_puts7(kk);
			if (jj == 0)
				tek1_puts7(len - 16 - 1);
#endif
			sbuf += i;
			p += len;
			i = lzsub_decstat(sbuf, &dis, &len);
		} while (--k);
		flen--;
	}

	if (cbylz[0])
		*(pbylz[0]) = bylz[0];
	if (cbylz[1])
		*(pbylz[1]) = bylz[1];

	return tek1_s7ptr - outbuf;
}

int lzcompress_tek1h(int srcsiz, int *src, int outsiz, UCHAR *outbuf, UCHAR *work, int modecode)
/* workは3KB必要(intが32bitだとして) */
{
	int *s256 = (int *) work, i, j;
	UCHAR *p;
	tek1_s7ptr = outbuf;
	if (modecode >= 0)
		tek1_puts7s(modecode);
	tek1_puts7s(src[0]);
	p = tek1_s7ptr;
	if (srcsiz >= 2) {
		for (i = 0; i < srcsiz - 1; i++)
			s256[i] = src[i + 1] - src[i];
		for (i = srcsiz - 2; i >= 1; i--) {
			j = (s256[i] - s256[i - 1]) << 1;
			if (j < 0)
				j ^= -1;
			s256[i] = j;
		}
		/* s256のsrcsiz-1個のintに対して符号化する */
		for (i = 0; i < srcsiz - 1; i++)
			tek1_puts7s(s256[i]);
		p = tek1_s7ptr;
	}
	return p - outbuf;
}

int blockofs0 = 0;

int lzcompress_tek1(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int bsiz, int flags, int opt, int prm, int maxdis, int submaxdis)
{
	int i, j, k, l, m, *s256;
	UCHAR *p, *q, *r, *s, *t, *u;
	tek1_s7ptr = outbuf;
	if (bsiz > srcsiz) {
		while ((bsiz >> 1) >= srcsiz)
			bsiz >>= 1;
	}
	s256 = (int *) work;
	work += 256 * 257 * sizeof (int); /* 257KB */
	wrksiz -= 256 * 257 * sizeof (int);
	for (i = 0, j = bsiz >> 8; j > 1; i++, j >>= 1);
	tek1_puts7s(i << 1 | 1); /* 上位3bitはバージョン(0はtek1sのみを意味する、さらにレベル0サイズ情報なし) */
	/* 下位1bitについて：0（スーパーショート） */
//	/* 001:ノーマルロング、011:マルチモードロング、その他はリザーブ */
//	tek1_puts7(2); /* 2:default, 1:無圧縮 */
//	tek1_puts7(0); /* ターミネータ */
	p = tek1_s7ptr;
	r = q = &p[322 * 1024]; /* レベル0出力用 */
	i = (srcsiz + bsiz - 1) / bsiz;
	s = src;
	for (j = 0; j < i; j++, s += bsiz) {
		t = src;
		if (&src[srcsiz] >= &s[bsiz]) {
			for (k = 0; k < j; k++, t += bsiz) {
				for (l = 0; l < bsiz; l++) {
					if (s[l] != t[l])
						goto skip0;
				}
				/* 一致ブロック発見 */
				s256[j] = s256[k]; /* k番目のブロックの内容をそのまま使え */
				goto skip1;
	skip0:
				;
			}
		}
		k = &src[srcsiz] - s;
		if (k > bsiz)
			k = bsiz;
		s256[j] = q - r; /* ブロック開始位置 */
		blockofs0 = s - src;
		if (flags == 1)
			k = lzcompress_tek2s(k, s, &outbuf[outsiz] - q, q, wrksiz, work, opt, prm, maxdis, submaxdis);
		else
			k = lzcompress_tek1s(k, s, &outbuf[outsiz] - q, q, wrksiz, work, opt, prm, maxdis, submaxdis);
		q += k;
skip1:
		;
	}
	t = s = &p[(1 + 256) * 5];
	/* i:ブロック総数 */
	/* j:レベル1ブロック総数 */
	j = (i + 255) >> 8;
	for (k = 0; k < j; k++) {
		if (((k + 1) << 8) <= i) {
			for (l = 0; l < k; l++) {
				for (m = 0; m < 256; m++) {
					if (s256[(k << 8) + m] != s256[(l << 8) + m])
						goto skip2;
				}
				/* 一致ブロック発見 */
				s256[256 * 256 + k] = s256[256 * 256 + l];
				goto skip3;
	skip2:
				;
			}
		}
		l = i - (k << 8);
		if (l > 256)
			l = 256;
		s256[256 * 256 + k] = s - t; /* ブロック開始位置 */

		s += lzcompress_tek1h(l, &s256[k << 8], r - s, s, work, 0-1 /* mode */);
		/* フォーマットバージョン0ではモードフィールドはない */
skip3:
		;
	}
	if (j > 1) {
		/* レベル2出力 */
		u = &p[5];
		m = lzcompress_tek1h(j, &s256[256 * 256], t - u, u, work, -1 /* mode */);
		tek1_s7ptr = p;
		tek1_puts7s(m - 2); /* レベル2ゾーンサイズ */
		p = tek1_s7ptr;
		for (l = 0; l < m; l++)
			p[l] = u[l];
		p += m;
	}
	/* レベル1出力(s, t) */
	if (i > 1) {
		tek1_s7ptr = p;
		tek1_puts7s((m = s - t) - 2); /* レベル1ゾーンサイズ */
		p = tek1_s7ptr;
		for (l = 0; l < m; l++)
			p[l] = t[l];
		p += m;
	}
	/* レベル0出力(q, r) */
	/* verによっては以下をやる */
//	tek1_s7ptr = p;
//	tek1_puts7((m = q - r) - 2); /* レベル0ゾーンサイズ */
//	p = tek1_s7ptr;
	m = q - r;
	for (l = 0; l < m; l++)
		p[l] = r[l];
	return &p[m] - outbuf;
}

UCHAR *loadfdis_sub(UCHAR *p, UCHAR *p1)
{
	while (p < p1 && *p != '\n' && *p <= ' ')
		p++;
	return p;
}

int loadfdis(int siz, unsigned short *tbl)
{
	UCHAR *p, *p0, *p1;
	int fsiz, i, j, k;
	for (i = 0; i < siz; i++)
		tbl[i] = 0;
	fseek(hint, 0, SEEK_END);
	fsiz = ftell(hint);
	p0 = malloc(fsiz);
	if (p0 == NULL)
		return 1;
	p1 = p0 + fsiz;
	fseek(hint, 0, SEEK_SET);
	fread(p0, 1, fsiz, hint);
	p = p0;
	while (p < p1) {
		p = loadfdis_sub(p, p1);
		p = getnum0(p, p1, &i); /* ofs0 */
		p = loadfdis_sub(p, p1);
		p = getnum0(p, p1, &j); /* ofs1 */
		p = loadfdis_sub(p, p1);
		p = getnum0(p, p1, &k);	/* fdis */
		while (p < p1 && *p != '\n')
			p++;
		if (p < p1 && *p == '\n')
			p++;
		if (i < blockofs0)
			i = blockofs0;
		if (j > blockofs0 + siz)
			j = blockofs0 + siz;
		i -= blockofs0;
		j -= blockofs0;
		while (i < j)
			tbl[i++] = k;
	}
	free(p0);
	for (i = 0; i < siz; i++) {
		if (tbl[i])
			return 0;
	}
	return 1; /* オールゼロ */
}

void setsubbuf(int siz, UCHAR *src, unsigned short *fdstbl, UCHAR *subbuf)
{
	int i, j = 0, k;
	for (i = 0; i < siz; i++) {
		if ((k = fdstbl[i]) != 0)
			subbuf[j++] = src[i] - src[i - k];
	}
	return;
}

static UCHAR *tek1_s7ptr;
//static struct TEK1_STR_UC tek2_ucstby[2];

//static UCHAR tek2_table_tr1[256];

/* tek1展開用関数群 */

struct TEK1_STR_BTBUF {
	unsigned int buf0, buf1, rest;
	unsigned int *ptr;
};

#if (!defined(TEK1_DEFINED_UC))

struct TEK1_STR_UC { /* ユニバーサルコード */
	UCHAR lentbl[TEK1_MAXLEN + 1], maxlen, flag;
	int base[TEK1_MAXLEN + 2], limit;
};

#endif

/* IA-32以外への移植の際は、
	tek1_getnum_s7, tek1_btseek, tek1_getlen, tek1_getdat
あたりをいじればいいだろうと思われる */
/* tek1_getlt0にもIA-32依存の記述がある */

int tek1_intlog2p(int i)
/* 0～(i - 1)を表現するのに何ビット必要か？ */
/* i = 1に対して0を返す */
/* i = 0101に対して、3を返す */
/* i = 100に対して2を返す */
{
	int j;
	for (j = 0; i > (1 << j); j++);
	return j;
}

unsigned int tek1_getnum_s7s(UCHAR **pp)
/* これは必ずbig-endian */
/* 下駄がないので中身をいじりやすい */
{
	unsigned int s = 0;
	UCHAR *p = *pp;
	for (;;) {
		s = s << 8 | *p++;
		if (s & 1)
			break;
		s >>= 1;
	}
	s >>= 1;
	*pp = p;
	return s;
}

unsigned int tek1_getnum_s7(UCHAR **pp)
/* これは必ずbig-endian */
{
	unsigned int s = 0, b = 0, a = 1;
	UCHAR *p = *pp;
	for (;;) {
		s = s << 8 | *p++;
		if (s & 1)
			break;
		s >>= 1;
		a <<= 7;
		b += a;
	}
	s >>= 1;
	*pp = p;
	return s + b;
}

void tek1_btseek(struct TEK1_STR_BTBUF *btbuf, UCHAR *p)
/* アライン位置までアクセスがさかのぼることもありうる */
/* また最大で終端+8バイトまでアクセスしうる */
/* いずれもアクセスしうるというだけで、データが意味あるものである必要はない */
{
	int bit = (((int) p) & 0x03) << 3;
	//((int) p) &= ~0x03;
	p = (UCHAR *)(((int) p) & ~0x03);
	btbuf->rest = 32 - bit;
	btbuf->buf0 = (*(unsigned int *) &p[0]) >> bit;
	btbuf->buf1 &= 0;
	btbuf->ptr = (unsigned int *) &p[4];
	return;
}

int tek1_getlen(struct TEK1_STR_BTBUF *btbuf, int max)
/* 1の数を数える */
{
	static const UCHAR table[16] = {
		4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0	/* 1で停止の場合 */
	};
	int i = 0, j;
	for (;;) {
		if (btbuf->rest < 32) {
			btbuf->buf0 |= (*btbuf->ptr) << btbuf->rest;
			btbuf->buf1  = (*btbuf->ptr++) >> (32 - btbuf->rest);
			btbuf->rest += 32;
		}
		j = table[btbuf->buf0 & 0x0f];
		if (i + j >= max) {
			j = max - i;
			i = max;
			if (j == 0)
				goto skip0;
			goto skip1;
		}
		i += j;
		if (j != 4) {
			j++;
	skip1:
			btbuf->buf0 = btbuf->buf0 >> j | btbuf->buf1 << (32 - j);
			btbuf->buf1 >>= j;
			btbuf->rest -= j;
	skip0:
			return i;
		}
		btbuf->rest -= 4;
		btbuf->buf0 = btbuf->buf0 >> 4 | btbuf->buf1 << (32 - 4);
		btbuf->buf1 >>= 4;
	}
}

unsigned int tek1_getdat(struct TEK1_STR_BTBUF *btbuf, int len)
/* lenビットを切り出す */
{
	unsigned int i;
	if (btbuf->rest < 32) {
		btbuf->buf0 |= (*btbuf->ptr) << btbuf->rest;
		btbuf->buf1 = (*btbuf->ptr++) >> (32 - btbuf->rest);
		btbuf->rest += 32;
	}
	i = btbuf->buf0;
	btbuf->buf0 = btbuf->buf0 >> len | btbuf->buf1 << (32 - len);
	btbuf->buf1 >>= len;
	btbuf->rest -= len;
	return i & ((1 << len) - 1);
}

#if 0

void tek1_initbaselim1(struct TEK1_STR_UC *ucstr)
/* getnum_uc1用 */
{
	ucstr->limit &= 0;
	int m = ucstr->maxlen, i = ucstr->lentbl[m];
	if (i > 0)
		i--;
	else {
		m--;
		if (m >= 0)
			i = ucstr->lentbl[m];
	}
	if (m >= 0)
		ucstr->limit = ucstr->base[m] + (1 << i);
	return;
}

#endif

unsigned int tek1_getnum_uc0(struct TEK1_STR_BTBUF *btbuf, struct TEK1_STR_UC *ucstr)
{
	unsigned int i = 0;
	int j;
	if (ucstr->maxlen)
		i = tek1_getlen(btbuf, ucstr->maxlen);
	j = ucstr->base[i];
	if (ucstr->lentbl[i] > 0)
		j += tek1_getdat(btbuf, ucstr->lentbl[i]);
	return j;
}

void tek1_inituc0sub(struct TEK1_STR_BTBUF *btbuf, struct TEK1_STR_UC *ucstr)
{
	int max, i = ucstr->maxlen;
	UCHAR c, m = 0, s;
	signed char n = ucstr->lentbl[i], d;
lp:
	c = tek1_getlen(btbuf, 3);
	if (c == 3) {
		m = 3; /* "000"が2回きたら、l1cで最大値指定。これで無限大対応。 */
		goto lp;
	}
	max = c * 8 + (tek1_getdat(btbuf, 1) + 1) * 4;
	if (m < 3)
		m = tek1_getdat(btbuf, 1);
	ucstr->base[0] = 0;
	for (;;) {
		if (m < 3 && n >= max)
			break;
		s = 0;
		if (m == 3) {
			s = tek1_getdat(btbuf, 1);
			if (n == 0 && s != 0)
				break; /* 0の場合は負の符号が出るだけでbreak; */
		}
		if (s == 0) {
			if (m == 0 && n > 0)
				d = tek1_getlen(btbuf, max - n) + 1;
			else
				d = tek1_getlen(btbuf, max - n + 1);
			n += d;
			if (n > max)
				break;
		} else {
			n -= tek1_getlen(btbuf, n + 1) + 1;
			if (n < 0)
				break;
		}
		ucstr->lentbl[i] = n;
		ucstr->base[i + 1] = ucstr->base[i] + (1 << n);
		i++;
	}
	ucstr->maxlen = i - 1;
	if (m < 3 && ucstr->lentbl[i - 1] > 0 && i > 1) {
		ucstr->lentbl[i - 1] -= tek1_getlen(btbuf, ucstr->lentbl[i - 1]);
		ucstr->base[i] = ucstr->base[i - 1] + (1 << ucstr->lentbl[i - 1]);
	}
	return;
}

void tek1_inituc0(struct TEK1_STR_BTBUF *btbuf, struct TEK1_STR_UC *ucstr)
{
	ucstr->maxlen = 0;
	ucstr->lentbl[0] = 0;
	tek1_inituc0sub(btbuf, ucstr);
	return;
}

void tek1_inituc1(struct TEK1_STR_UC *uc, UCHAR *prm)
{
	struct TEK1_STR_BTBUF btbuf;
	tek1_btseek(&btbuf, prm);
	tek1_inituc0(&btbuf, uc);
	return;
}

void tek1_inituc2(struct TEK1_STR_UC *ucstr, int lim, unsigned int cbits)
{
	int i, j;
	for (i = 0; i < lim; i++)
		ucstr->lentbl[i] = 0;
	j = 0;
	do {
		for (;;) {
			j++;
			if ((cbits & 1) == 0)
				break;
			cbits >>= 1;
		}
		cbits >>= 1;
		ucstr->lentbl[i] = j;
		i++;
	} while (j < 24);
	ucstr->base[0] = 0;
	ucstr->maxlen = i - 1;
	for (j = 0; j < i; j++)
		ucstr->base[j + 1] = ucstr->base[j] + (1 << ucstr->lentbl[j]);
	return;
}

int tek1_lzrestore_tek1s(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int subsiz, UCHAR *subbuf);
void tek1_lzrestore_tek1h(int srcsiz, UCHAR *src, int i, int *s256, int ver, int *ftyp);
int tek1_lzrestore_tek2s(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int subsiz, UCHAR *subbuf);
//int tek1_lzrestore_tek3s(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int flags, int subsiz, UCHAR *subbuf);
void tek1_lzrestore_tek3h(int srcsiz, UCHAR *src, int i, int *s256, int ver, int *ftyp);

int tek1_lzrestore_tek1(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, int flags)
/* レベル1までの対応。レベル0の場合、workは使わない */
/* レベル1の場合、workは512*sizeof(int)だけ使う */
/* このルーチンはマルチモードにも対応している */
/* srcsiz+8までアクセスしうるし、srcより手前のアラインまではアクセスをさかのぼることがあるので注意 */
/* どちらの場合もリードしうるというだけで、その値はなんでもよい */
{
	int i, j, k, bsiz, ver, *s256 = (int *) work;
	int ftyp[2], flags_tek2s;
	UCHAR *p, *q, *subbuf, *s7ptr;

	/* ヘッダ読み込み */
	s7ptr = src;
	i = tek1_getnum_s7s(&s7ptr);
	bsiz = 1 << (((i >> 1) & 0x0f) + 8);
	flags_tek2s &= 0;
	if (i & 1) {
		ver = (i >> 5) & 0x3;
		i = (outsiz + bsiz - 1) / bsiz;
	} else {
		ver = 0;
		i = 1;
		s7ptr = src;
		flags_tek2s |= 1; /* ショート形式認識 */
	}
	if (ver >= 4 * 256) {
err:
		return -1; /* 理解できないバージョン */
		/* パディングを入れておくと後から追加できる */
		/* しかしECCなどがくっついたらこの限りではない */
		/* ECCなどが付くかどうかは、verのbit1で決まる */
	}
	ftyp[0] = 2; /* tek1s/tek2s(デフォルト), 1:無圧縮, 0:ターミネータ */
	/* s7sになったのでpadはいらない（勝手に長くできる） */
	if (ver & 1) {
		ftyp[0] = tek1_getnum_s7s(&s7ptr);
		do {
			j = tek1_getnum_s7s(&s7ptr);
			if (j)
				ftyp[1] = j;
		} while (j);
	}
	if (i <= 1) {
		/* レベル0 */
		if (ver & 2)
			tek1_getnum_s7s(&s7ptr);
		if (ftyp[0] == 2) {
			subbuf = malloc(outsiz);
			if (subbuf == NULL)
				return 1;
			i = 1 << tek1_intlog2p(outsiz);
		/*	if (flags == 2)
				tek1_lzrestore_tek3s(&src[srcsiz] - s7ptr, s7ptr, outsiz, outbuf, flags_tek2s, i, subbuf);
			else */ if (flags == 1)
				tek1_lzrestore_tek2s(&src[srcsiz] - s7ptr, s7ptr, outsiz, outbuf, i, subbuf);
			else
				tek1_lzrestore_tek1s(&src[srcsiz] - s7ptr, s7ptr, outsiz, outbuf, i, subbuf);
			free(subbuf);
			goto fin;
		}
		if (ftyp[0] == 1) {
			q = s7ptr; /* 無圧縮ブロック */
			for (i = 0; i < outsiz; i++)
				outbuf[i] = q[i];
fin:
			return 0;
		}
		goto err;
	}
	if (i <= 256) {
		/* レベル1(32KBならこれでも8MBまでOK) */
		j = tek1_getnum_s7s(&s7ptr) + 2;
		q = s7ptr;
		p = q + j;
		if (ver & 2)
			tek1_getnum_s7s(&p); /* さらに読み飛ばす */
		if (flags == 2)
			tek1_lzrestore_tek3h(&src[srcsiz] - q, q, i, s256, ver, ftyp);
		else
			tek1_lzrestore_tek1h(&src[srcsiz] - q, q, i, s256, ver, ftyp);
		i--;
		subbuf = malloc(bsiz);
		if (subbuf == NULL)
			return 1;
		for (j = 0; j < i; j++) {
			q = &p[s256[j * 2 + 1]];
			if (s256[j * 2] == 2) {
			/*	if (flags == 2)
					tek1_lzrestore_tek3s(&src[srcsiz] - q, q, bsiz, outbuf, 0, srcsiz, subbuf);
				else */ if (flags == 1)
					tek1_lzrestore_tek2s(&src[srcsiz] - q, q, bsiz, outbuf, srcsiz, subbuf);
				else
					tek1_lzrestore_tek1s(&src[srcsiz] - q, q, bsiz, outbuf, srcsiz, subbuf);
			}
			if (s256[j * 2] == 1) {
				for (k = 0; k < bsiz; k++)
					outbuf[k] = q[k];
			}
			outbuf += bsiz;
		}
		q = &p[s256[i * 2 + 1]];
		outsiz -= bsiz * i;
		if (s256[i * 2] == 2) {
		/*	if (flags == 2)
				tek1_lzrestore_tek3s(&src[srcsiz] - q, q, bsiz, outbuf, 0, srcsiz, subbuf);
			else */ if (flags == 1)
				tek1_lzrestore_tek2s(&src[srcsiz] - q, q, bsiz, outbuf, srcsiz, subbuf);
			else
				tek1_lzrestore_tek1s(&src[srcsiz] - q, q, bsiz, outbuf, srcsiz, subbuf);
		}
		if (s256[i * 2] == 1) {
			for (k = 0; k < outsiz; k++)
				outbuf[k] = q[k];
		}
		free(subbuf);
		goto fin;
	}
	goto err;
}

void tek1_lzrestore_tek3h(int srcsiz, UCHAR *src, int i, int *s256, int ver, int *ftyp)
/* verのbit6を1にすると、レベル2以上のヘッダ解析にも使える */
{
	int j, k, l, m, n;
	UCHAR *s7ptr = src;
	if ((ver & 1) == 0) {
		for (n = 0; n < i; n++)
			s256[n * 2] = ftyp[0];
	}
	if (ver & 1) {
		j = i;
		n = 0;
		do {
			k = tek1_getnum_s7s(&s7ptr); /* s7sにすることで意味なく長くすることができ、パッドになる */
			l = (k >> 2) & 0xff;
			m = (k & 0x03) | ((k >> 8) & ~0x03);
			if (l == 0)
				l = j;
			j -= l;
			do {
				s256[n * 2] = m;
				n++;
			} while (--l);
		} while (j);
	}
	s256[1] = tek1_getnum_s7s(&s7ptr); /* ここにもパッドが入れられる */
	if (i > 1) {
		struct TEK1_STR_BTBUF btbuf;
		struct TEK1_STR_UC ucstr;
		static UCHAR *table[] = { tek1_ucprm_s4, tek1_ucprm_s41 };
		tek1_btseek(&btbuf, s7ptr);
		if (tek1_getdat(&btbuf, 1))
			tek1_inituc0(&btbuf, &ucstr);
		else
			tek1_inituc1(&ucstr, table[tek1_getdat(&btbuf, 1)]);
		n = 0;
		for (j = 1; j < i; j++) {
			k = tek1_getnum_uc0(&btbuf, &ucstr);
			if (j > 1) {
				if (k & 1)
					k ^= -1;
				k >>= 1;
			}
			n += k;
			s256[j * 2 + 1] = s256[j * 2 - 1] + n;
		}
	}
	return;
}

int tek1_getlt0(struct TEK1_STR_BTBUF *btbuf, int lt0)
{
	if (tek1_getdat(btbuf, 1)) {
		int i = tek1_getdat(btbuf, 2);
		if (i == 0)
			lt0 *= 2;
		if (i == 1)
			lt0 /= 2;
		if (i == 3) {
			tek1_getdat(btbuf, 1); /* "1"を読み飛ばす */
			lt0 = 0x7fffffff;
		}
		if (i == 2) {
			i = tek1_getdat(btbuf, 4);
			if (i <= 6)
				lt0 += (((signed int) tek1_getdat(btbuf, i * 4)) << (32 - i * 4)) >> (32 - i * 4);
					/* 変なシフトは、符号拡張のため(IA-32依存？) */
			if (8 <= i && i <= 11)
				lt0 <<= i - 6;
			if (12 <= i)
				lt0 >>= i - 10;
		}
	}
	return lt0;
}

void tek1_loaduc(struct TEK1_STR_BTBUF *btbuf, int lt1, struct TEK1_STR_UC *uc0, struct TEK1_STR_UC *uc1, int *pl0, UCHAR *prm, int flags)
{
	struct TEK1_STR_UC *uc[2];
	int i, j;
	uc[0] = uc0;
	uc[1] = uc1;
	for (i = 0; i < 2; i++) {
		if (lt1 == 0) {
			/* 2回目以降で更新フラグが0ならスキップ */
			if (tek1_getdat(btbuf, 1) == 0)
				continue;
		}
		if (tek1_getdat(btbuf, 1))
			tek1_inituc0(btbuf, uc[i]);
		else {
			/* 各種デフォルト設定利用 */
			if (pl0 == NULL)
				tek1_inituc2(uc[i], tek1_getdat(btbuf, 3), 0); /* by, lz */
			else {
				/* ds, cp */
				static UCHAR *table[] = { tek1_ucprm_l1c, tek1_ucprm_l1b, tek1_ucprm_l1a };
				UCHAR *ucprm = tek1_ucprm_ds1; /* ds */
				if (i == 1)
					ucprm = table[tek1_getdat(btbuf, 2)]; /* cp */
				tek1_inituc1(uc[i], ucprm);
			}
		}
		j = uc[i]->maxlen;
	//	tek1_initbaselim1(uc[i]);
		if (pl0 != NULL) {
			if (i == 0) {
				for (j = 0; j <= TEK1_MAXLEN; j++)
					prm[j] = uc0->lentbl[j];
				prm[TEK1_MAXLEN + 1] = uc0->maxlen;
				prm[TEK1_MAXLEN + 2] = 0;
				if (uc0->maxlen == 0 && uc0->lentbl[0] == 0)
					prm[TEK1_MAXLEN + 2] = 1;
				uc0->limit = 1;
				uc0->lentbl[0] = 0;
				uc0->maxlen = 0;
			} else
				*pl0 = tek1_getdat(btbuf, 2) + (flags & 1);
		}
	}
}

void tek1_loaduc0(struct TEK1_STR_BTBUF *btbuf, int lt1, struct TEK1_STR_UC *ucstr, int *pl0, int mode)
/* mode0:byかlz */
/* mode1:ds */
/* mode2:cp */
{
	int i;
	if (lt1 == 0) {
		/* 2回目以降で更新フラグが0ならスキップ */
		if (tek1_getdat(btbuf, 1) == 0)
			goto fin;
	}
	if (tek1_getdat(btbuf, 1))
		tek1_inituc0(btbuf, ucstr);
	else {
		/* 各種デフォルト設定利用 */
		if (mode == 0)
			tek1_inituc2(ucstr, tek1_getdat(btbuf, 3), 0); /* by, lz */
		if (mode == 1)
			tek1_inituc1(ucstr, tek1_ucprm_ds1); /* ds */
		if (mode == 2) {
			static UCHAR *table[] = { tek1_ucprm_l1c, tek1_ucprm_l1b, tek1_ucprm_l1a };
			tek1_inituc1(ucstr, table[tek1_getdat(btbuf, 2)]);
		}
	}
	/* 追加情報 */
	if (mode == 1) { /* ds */
		struct TEK1_STR_UC ucl1c;
		i = tek1_getdat(btbuf, 3);
		if (i >= 7) {
			tek1_inituc1(&ucl1c, tek1_ucprm_l1c);
			i += tek1_getnum_uc0(btbuf, &ucl1c);
		}
		*pl0 = i;
	}
	if (mode == 2)
		*pl0 = tek1_getdat(btbuf, 2);
fin:
	return;
}

int tek1_lzrestore_tek1s(int srcsiz, UCHAR *src, int outsiz, UCHAR *q, int subsiz, UCHAR *subbuf)
{
	int by, lz, cp, ds;
	UCHAR *q1 = q + outsiz, *s7ptr = src;
	if (outsiz) {
		if (tek1_getnum_s7s(&s7ptr) != 0)
			return 1;
		do {
			/* byとlzのデコード */
			/* LLLLBBBB */
			lz = by = *s7ptr++;
			if ((by &= 0x0f) == 0)
				by = tek1_getnum_s7s(&s7ptr);
			if ((lz >>= 4) == 0)
				lz = tek1_getnum_s7s(&s7ptr);
			/* byフェーズ */
			do {
				*q++ = *s7ptr++;
			} while (--by);

			if (q >= q1)
				break;

			/* lzフェーズ */
			do {
				/* dsが下位 */
				ds = *s7ptr++;
				cp = ds;
				ds &= 0x0f;
				if ((ds & 1) == 0) {
					do {
						ds = ds << 7 | *s7ptr++;
					} while ((ds & 1) == 0);
				}
				ds >>= 1;
				cp >>= 4;
				if (cp == 0) {
					do {
						cp = cp << 7 | *s7ptr++;
					} while ((cp & 1) == 0);
					cp >>= 1;
				} /* 0がこないことをあてにする */
				ds ^= -1;
				cp++;
				do {
					*q = *(q + ds);
					q++;
				} while (--cp);
			} while (--lz);
		} while (q < q1);
	}

	return 0;
}

int tek1_lzrestore_tek2s(int srcsiz, UCHAR *src, int outsiz, UCHAR *q, int subsiz, UCHAR *subbuf)
{
	int by, lz, cp, ds, flen, fdis = 0, subi = 0, repdis[2][4], i, j;
	UCHAR *q1 = q + outsiz, *s7ptr = src, bylz[2], cbylz[2], fmode = 0;
	#if (defined(TEK1_NOWARN))
		lz = 0;
	#endif
	for (j = 0; j < 4; j++)
		repdis[0][j] = repdis[1][j] = 6 + j;
	bylz[0] = bylz[1] = 0;
	cbylz[0] = cbylz[1] = 0;
	if (outsiz) {
		flen = tek1_getnum_s7s(&s7ptr);
		if (flen == 0)
			flen = 0x7fffffff;
		/* 常に先頭1bitを読み終わった状態にする */
		do {
			do {
				/* byフェーズ */
				/* bylzストリームから、"0"の数を数える */
				j = 0;
				do {
					j++;
					if (j >= 17) {
						j += tek1_getnum_s7s(&s7ptr);
						break;
					}
					if (cbylz[fmode] == 0) {
						cbylz[fmode] = 8;
						bylz[fmode] = *s7ptr++;
					}
					cbylz[fmode]--;
					i = bylz[fmode] & 1;
					bylz[fmode] >>= 1;
				} while (i == 0);

				if (fdis == 0) {
					do {
						*q++ = *s7ptr++;
					} while (--j);
				} else {
					do {
						subbuf[subi] = *s7ptr++;
						*q = *(q + fdis) + subbuf[subi];
						subi = (subi + 1) & (subsiz - 1);
						q++;
					} while (--j);
				}
				if (q >= q1)
					goto fin;
				flen--;
				if (flen == 0)
					break;

	loop_lz:
				/* lzフェーズ */
				/* bylzストリームから、"0"の数を数える */
				j = 0;
				do {
					j++;
					if (j >= 17) {
						j += tek1_getnum_s7s(&s7ptr);
						break;
					}
					if (cbylz[fmode] == 0) {
						cbylz[fmode] = 8;
						bylz[fmode] = *s7ptr++;
					}
					cbylz[fmode]--;
					i = bylz[fmode] & 1;
					bylz[fmode] >>= 1;
				} while (i == 0);

				do {

#if 1
					by = 0;
					if (fdis != 0)
						by = 1;
					ds = *s7ptr++;
					cp = ds >> 4;
					ds &= 0x0f;
					if (ds & 1)
						ds >>= 1;
					else
						ds = ds >> 1 | (tek1_getnum_s7(&s7ptr) + 1) << 3;
					if (ds == 4)
						ds = repdis[by][0] + tek1_getnum_s7(&s7ptr) + 1 + 6*0;
					if (ds == 5)
						ds = repdis[by][0] - tek1_getnum_s7(&s7ptr) - 1 + 6*0;
					if (cp == 0)
						cp = tek1_getnum_s7(&s7ptr) + 16;
					cp++;
					i = ds;
					if (ds < 4)
						i = repdis[by][ds];
					if (ds == 0)
						goto skip_repdis0;
					if (ds == 1)
						goto skip_repdis1;
					if (ds == 2)
						goto skip_repdis2;
					repdis[by][3] = repdis[by][2];
skip_repdis2:
					repdis[by][2] = repdis[by][1];
skip_repdis1:
					repdis[by][1] = repdis[by][0];
skip_repdis0:
					repdis[by][0] = i;
					ds = i - 6;
#endif
					ds ^= -1;
					if (fdis == 0) {
						do {
							*q = *(q + ds);
							q++;
						} while (--cp);
					} else {
						if (ds + subsiz < 0)
							return 1;
						do {
							subbuf[subi] = subbuf[(subi + ds) & (subsiz - 1)];
							*q = *(q + fdis) + subbuf[subi];
							subi = (subi + 1) & (subsiz - 1);
							q++;
						} while (--cp);
					}
				} while (--j);
				if (q >= q1)
					goto fin;
			} while (--flen);
			by = fdis = *s7ptr++;
			flen = fdis & 0x0f;
			if (flen == 0)
				flen = tek1_getnum_s7(&s7ptr) + 16;
			fdis >>= 5;
			if (fdis == 0)
				fdis = tek1_getnum_s7(&s7ptr) + 8;
			fdis = 1 - fdis;
		} while ((by & 0x10) == 0);
		lz = tek1_getnum_s7(&s7ptr) + 1;
		goto loop_lz;
	}
fin:
	return 0;
}

void tek1_lzrestore_tek1h(int srcsiz, UCHAR *src, int i, int *s256, int ver, int *ftyp)
/* verのbit6を1にすると、レベル2以上のヘッダ解析にも使える */
{
	int j, k, l, m, n;
	UCHAR *s7ptr = src;
	if ((ver & 1) == 0) {
		for (n = 0; n < i; n++)
			s256[n * 2] = ftyp[0];
	}
	if (ver & 1) {
		j = i;
		n = 0;
		do {
			k = tek1_getnum_s7s(&s7ptr); /* s7sにすることで意味なく長くすることができ、パッドになる */
			l = (k >> 2) & 0xff;
			m = (k & 0x03) | ((k >> 8) & ~0x03);
			if (l == 0)
				l = j;
			j -= l;
			do {
				s256[n * 2] = m;
				n++;
			} while (--l);
		} while (j);
	}
	s256[1] = tek1_getnum_s7s(&s7ptr); /* ここにもパッドが入れられる */
	if (i > 1) {
		n = 0;
		for (j = 1; j < i; j++) {
			k = tek1_getnum_s7s(&s7ptr);
			if (j > 1) {
				if (k & 1)
					k ^= -1;
				k >>= 1;
			}
			n += k;
			s256[j * 2 + 1] = s256[j * 2 - 1] + n;
		}
	}
	return;
}

int rjc(UCHAR *p0, UCHAR *p1, int ofs0, int ofs, int ofs1, int mode)
/* mode:	0:decode, 1:encode */
{
	UCHAR *p = p0, *pp = p0 - 4;
	int i, j, k, m = 0;
	while (p < p1) {
		if (0xe8 <= *p && *p <= 0xe9 && &p[4] < p1) {	/* e8 (call32), e9 (jmp32) */
	r32:
			p++;
			if (p - pp < 4)
				continue;
			i = p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
			j = (p - p0) + ofs + 4; /* 相対アドレス基点 */
			k = i;
			if (mode) { /* encode */
				if (ofs0 - j <= i && i < ofs1 - j)
					i += j;
				else if (ofs1 - j <= i && i < ofs1)
					i -= ofs1 - ofs0;
			} else { /* decode */
				if (ofs0 <= i && i < ofs1)
					i -= j;
				else if (ofs0 - j <= i && i < ofs0)
					i += ofs1 - ofs0;
			}
			pp = p;
			if (i != k ) {
				p[0] =  i        & 0xff;
				p[1] = (i >>  8) & 0xff;
				p[2] = (i >> 16) & 0xff;
				p[3] = (i >> 24) & 0xff;
				p += 4;
				m++;
			}
			continue;
		}
		p++;
		if (p[-1] == 0x0f && &p[4] < p1 && (p[0] & 0xf0) == 0x80)	/* 0f 8x (jcc32) */
			goto r32;
	}
	return m;
}

/* auto, osaapp-dec, osaapp-enc, full-dec, full-enc */

void osarjc(int siz, UCHAR *p, int mode)
/* mode:	bit0:dec(0)/enc(1), bit1:auto(0)/force-full(1) */
{
	static UCHAR header1[0x48] = {
		"\x2e\x8b\x62\x20"
		"\x8b\xfc"
		"\xeb\x20"
		"GUIGUI00"
		"\x01\x01\x01\x01\x01\x01\x01\x01"
		"\x01\x01\x01\x01\x01\x01\x01\x01"
		"\x01\x01\x01\x01\x01\x01\x01\x01"
		"\x6a\xff"
		"\x0e"
		"\x68\x01\x01\x01\x01"		/* +0x2c : data begin */
		"\x2e\xff\x72\x24"
		"\x6a\x01"
		"\x6a\x04"
		"\x8b\xdc"
		"\x57"
		"\x9a\x00\x00\x00\x00\xc7\x00"
		"\x5c"
		"\xe9\x01\x01\x01\x01"		/* +0x44 : entry */
	};
	UCHAR flag = 0, ggflag = 0;
	int i = 0;
	if (siz >= 0x20
		&& *(int *) &p[8] == ('G' | 'U' << 8 | 'I' << 16 | 'G' << 24)
		&& *(int *) &p[12] == ('U' | 'I' << 8 | '0' << 16 | '0' << 24))
		ggflag = 1;
	if (ggflag != 0 && siz >= 0x48) {
		flag = 1;
		for (; i < 0x48; i++) {
			if (header1[i] == 0x01)
				continue;
			if (header1[i] != p[i])
				flag = 0;
		}
		i = *(int *) &p[0x2c];
	}
	if (ggflag) {
		if (mode == 0 && (p[20] & 2) != 0) {
			/* エンコードしてあるものをデコード */
			if ((p[20] & 0xfe) == 0x06) {
				rjc(p + 0x20, p + i, 0x20, 0x20, i, 0); /* decode */
				p[20] &= 0x01;
			} else if ((p[20] & 0xfe) == 0x02) {
				rjc(p + 0x20, p + siz, 0x20, 0x20, siz, 0); /* decode */
				p[20] &= 0x01;
			}
			/* 他の形式は今はサポートなし */
		}
		if (mode == 1 && (p[20] & 2) == 0) {
			/* デコードしてあるものをエンコード */
			p[20] &= 0x01;
			if (flag) {
				rjc(p + 0x20, p + i, 0x20, 0x20, i, 1); /* encode */
				p[20] |= 0x06;
			} else {
				rjc(p + 0x20, p + siz, 0x20, 0x20, siz, 1); /* encode */
				p[20] |= 0x02;
			}
		}
	}
}

#define TEK5_TMPNAME	"_tektmp_.__"

struct str_testall_t5lzma {
	int a, d, fb, lc, lp, pb, mf;
	int phase, min, min_p, *tst_p;
};

int lzcompress_tek5(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, int wrksiz, UCHAR *work, UCHAR *argv0, UCHAR *eopt, int bsiz, int flags, int opt, int prm, int maxdis, int submaxdis)
{
	int i, j;
	struct str_testall_t5lzma testall;
	UCHAR eoptmp[100];
	FILE *fp;
	UCHAR *p, *q, *r;
	testall.phase = 0;
	if (bsiz > srcsiz) {
		while ((bsiz >> 1) >= srcsiz)
			bsiz >>= 1;
	}
	if (bsiz < srcsiz)
		return 0;
	fp = fopen(TEK5_TMPNAME "0", "wb");
	if (fp == NULL)
		return 0;
	i = fwrite(src, 1, srcsiz, fp);
	fclose(fp);
	if (i != srcsiz)
		return 0;

retry:
	q = p = outbuf;
	r = argv0;
	do {
		*p = *r++;
		if (*p == '/' || *p == '\\')
			q = p + 1;
		p++;
	} while (*r);

#ifdef HOST_MINGW
	int outbufLength = strlen(outbuf);
	UCHAR *shortPathName = malloc(outbufLength+1); // '\0'の分+1
	int needLength = GetShortPathName(outbuf, shortPathName, outbufLength+1);
	if (needLength == 0 || needLength >= outbufLength) {
		// なんかエラー or 長さが足りない → 何もしないでおく
	} else {
		memset(outbuf, '\0', outbufLength);
		strcpy(outbuf, shortPathName);
		q -= outbufLength - needLength;
	}
	free(shortPathName);
#endif //HOST_MINGW

	for (p = (UCHAR *)"t5lzma e " TEK5_TMPNAME "0 " TEK5_TMPNAME "1 "; *p; *q++ = *p++);
	if (eopt) {
		if (eopt[0] == '@' && (eopt[1] == '\0' || (eopt[1] == '@' && eopt[2] == '\0'))) {
			static char *mfname[] = {
				"bt2", "bt3", "bt4", "bt4b",
				"pat2r", "pat2", "pat2h", "pat3h", "pat4h",
				"hc3", "hc4"
			};

			if (testall.phase == 0) {
				testall.phase = 1;
				testall.a = 2;
				testall.d = 23;
				testall.fb = 5;
				testall.lc = 0;
				testall.lp = 0;
				testall.pb = 0;
				testall.mf = 2;
				testall.min = 0x7fffffff;
				testall.tst_p = &testall.fb;
			} else if (testall.phase == 1) {
				 /* fb決定ループ */
				if (testall.fb < 255)
					testall.fb++;
				else {
					testall.fb = testall.min_p;
					testall.phase = 2;
					testall.tst_p = &testall.lc;
					testall.min_p = 0;
					testall.lc = 1;
				}
			} else if (testall.phase == 2) {
				 /* lc決定ループ */
				if (testall.lc < 8)
					testall.lc++;
				else {
					testall.lc = testall.min_p;
					testall.phase = 3;
					testall.tst_p = &testall.lp;
					testall.min_p = 0;
					testall.lp = 1;
				}
			} else if (testall.phase == 3) {
				 /* lp決定ループ */
				if (testall.lp < 4)
					testall.lp++;
				else {
					testall.lp = testall.min_p;
					testall.phase = 4;
					testall.tst_p = &testall.pb;
					testall.min_p = 0;
					testall.pb = 1;
				}
			} else if (testall.phase == 4) {
				 /* pb決定ループ */
				if (testall.pb < 4)
					testall.pb++;
				else {
					testall.pb = testall.min_p;
					testall.phase = -1;
					if (testall.lc | testall.lp | testall.pb) {
						testall.phase = 5;
						testall.tst_p = &testall.fb;
						testall.min_p = testall.fb;
						testall.fb = 5;
					}
				}
			} else if (testall.phase == 5) {
				 /* fb決定ループ2 */
				if (testall.fb < 255)
					testall.fb++;
				else {
					testall.fb = testall.min_p;
					testall.phase = -1;
				}
			}

#if 0
			if (testall.phase == 0) {
				testall.phase = 1;
				testall.a = 2;
				testall.d = 23;
				testall.fb = 128;
				testall.lc = 0;
				testall.lp = 0;
				testall.pb = 0;
				testall.mf = 2;
				testall.min = 0x7fffffff;
				testall.tst_p = &testall.lc;
			} else if (testall.phase == 1) {
				 /* lc決定ループ */
				if (testall.lc < 8)
					testall.lc++;
				else {
					testall.lc = testall.min_p;
					testall.phase = 2;
					testall.tst_p = &testall.lp;
					testall.min_p = 0;
					testall.lp = 1;
				}
			} else if (testall.phase == 2) {
				 /* lp決定ループ */
				if (testall.lp < 4)
					testall.lp++;
				else {
					testall.lp = testall.min_p;
					testall.phase = 3;
					testall.tst_p = &testall.pb;
					testall.min_p = 0;
					testall.pb = 1;
				}
			} else if (testall.phase == 3) {
				 /* pb決定ループ */
				if (testall.pb < 4)
					testall.pb++;
				else {
					testall.pb = testall.min_p;
					testall.phase = 4;
					testall.tst_p = &testall.mf;
					testall.min_p = 2;
					testall.mf = 0;
				}
			} else if (testall.phase == 4) {
				 /* mf決定ループ */
				if (testall.mf < 10)
					testall.mf++;
				else {
					testall.mf = testall.min_p;
					testall.phase = 5;
					testall.tst_p = &testall.d;
					testall.min_p = 23;
					testall.d = 22;
				}
			} else if (testall.phase == 5) {
				 /* d決定ループ */
				if (testall.d > 0)
					testall.d--;
				else {
					testall.d = testall.min_p;
					testall.phase = 6;
					testall.tst_p = &testall.a;
					testall.min_p = 2;
					testall.a = 1;
				}
			} else if (testall.phase == 6) {
				 /* a決定ループ */
				if (testall.a > 0)
					testall.a--;
				else {
					testall.a = testall.min_p;
					testall.phase = 7;
					testall.tst_p = &testall.fb;
					testall.min_p = 128;
					testall.fb = 5;
				}
			} else if (testall.phase == 7) {
				 /* a決定ループ */
				if (testall.fb < 255)
					testall.fb++;
				else {
					testall.fb = testall.min_p;
					testall.phase = -1;
				}
			}
#endif

			sprintf((char *)eoptmp, "-a%d_-d%d_-fb%d_-lc%d_-lp%d_-pb%d_-mf%s",
				testall.a, testall.d, testall.fb, testall.lc, testall.lp, testall.pb, mfname[testall.mf]);
			p = eoptmp;
			if (eopt[1])
				fputs((char *)eoptmp, stderr);
		} else
			p = eopt;
		for (; *p; p++, q++) {
			*q = ' ';
			if (*p != '_')
				*q = *p;
		}
		if (testall.phase < 0) {
			if (eopt[1])
				fputc('\n', stderr);
			printf("eopt:%s\n", eoptmp);
		}
	}
	for (p = (UCHAR *)" -notitle"; *p; *q++ = *p++);
	*q = '\0';
	i = system((char *)outbuf);
	if (i != 0) {
		remove(TEK5_TMPNAME "1");
		if (testall.phase > 0)
			goto retry;
		remove(TEK5_TMPNAME "0");
		return 0;
	}
	fp = fopen(TEK5_TMPNAME "1", "rb");
	if (fp == NULL)
		return 0;
	if (fread(outbuf, 1, 14, fp) != 14) {
		fclose(fp);
		remove(TEK5_TMPNAME "1");
		remove(TEK5_TMPNAME "0");
		return 0;
	}
	p = outbuf;
	q = outbuf + outsiz;
	p[1] = p[0];
	p[0] = 0x01;
	if (p[1] == 0x5d)
		p[0] = 0x11;
	else if (p[1] == 0x00)
		p[0] = 0x21;
	else
		p++;
	p++;
	i = fread(p, 1, q - p, fp);
	fclose(fp);
	remove(TEK5_TMPNAME "1");
	if (i >= q - p) {
		if (testall.phase > 0) {
			if (eopt[1])
				fprintf(stderr, "  i = %d\n", i);
			goto retry;
		}
		return 0;
	}

	if (testall.phase > 0) {
		if (eopt[1])
			fprintf(stderr, "  i = %d\n", i);
		if (testall.min > i) {
			testall.min = i;
			testall.min_p = *testall.tst_p;
		}
		goto retry;
	}
	remove(TEK5_TMPNAME "0");

	q = malloc(i = (p + i) - outbuf);
	for (j = 0; j < i; j++)
		q[j] = outbuf[j];

	i = tek_conv_tek5(i, q, srcsiz, src, outbuf, glb_str_eprm);
	/* 先頭1バイトの破棄は、conv_tek5内で行うので心配はいらない */
	free(q);

	return i;
}

UCHAR *tek_analyze_stk5_h(UCHAR *src, int *lc, int *lp, int *pb, int *flags)
{
	int prop0, fl;
	if ((fl = (prop0 = *src) & 0x0f) == 0x01) /* 0001 */
		*flags |= -1;
	else if (fl == 0x05)
		*flags = -2;
	else if (fl == 0x09)
		*flags &= 0;
	else
		return NULL;
	src++;
	prop0 >>= 4;
	if (prop0 == 0)
		prop0 = *src++;
	else {
		static UCHAR prop0_table[] = { 0x5d, 0x00 }, prop1_table[] = { 0x00 };
		if (*flags == -1) {
			if (prop0 >= 3)
				return NULL;
			prop0 = prop0_table[prop0 - 1];
		} else {
			if (prop0 >= 2)
				return NULL;
			prop0 = prop1_table[prop0 - 1];
		}
	}
	*lp = prop0 / (9 * 5);
	prop0 %= 9 * 5;
	*pb = prop0 / 9;
	*lc = prop0 % 9;
	if (*flags == 0) /* tek5:z2 */
		*flags = *src++;
	if (*flags == -1) { /* stk5 */
		prop0 = *lp;
		*lp = *pb;
		*pb = prop0;
	}
	return src; /* pb:0-4, lp:0-4, lc:0-8 */
}

#define MAX_STATE	12
#define MAX_PB2		16
#define MAX_LC2		256
#define MAX_LP2		16

#define TEK5_LIFETIME0	1024 * 16

struct STR_RCLOG {
	int id;
	UCHAR m, t, dmy[2];
};

struct STR_RCBITMODEL {
	int id, s, m, t, lt, lt0;
	unsigned int *logptr, logptr0[TEK5_LIFETIME0];
	unsigned int prb0, prb1, tmsk;
};

struct STR_BACKUP_PROB {
	unsigned int *prob, data;
};

struct STR_PRB {
	struct STR_PRB_PB {
		struct STR_PRB_PBST {
			int mch;
			int rep0l1;
		} st[MAX_STATE];
		int lenlow[2][8], lenmid[2][8];
	} pb[MAX_PB2];
	struct STR_PRB_ST {
		int rep, repg0, repg1, repg2;
	} st[MAX_STATE];
	int lensel[2], lensel2[2], lenhigh[2][256], lensele[8], lensel3[2];
	int pslot[4][64], algn[16*4];
	int spdis1[2][2], spdis2[2][4], spdis3[2][8], spdis4[2][16], spdis5[2][32];
	int lit[MAX_LP2][MAX_LC2][256 * 3];
	int repg3, repg4;
	int lenext1[2], lenext2[4], lenext3[8], lenext4[16], lenext5[32];
	int fchgprm[2 * 32], tbmt[16], tbmm[16], fchglt;
};

struct RC {
	UCHAR *p;
	unsigned int range, cache, ffnum;
	unsigned long long code;

	unsigned int *prob0, rmsk, probs, phase, idnext;
	struct STR_RCLOG *log1, log0[64 * 1024];
	struct STR_BACKUP_PROB backup[256 * 256 * 16 * 2];
	UCHAR prbflg[sizeof (struct STR_PRB) / sizeof (int)];
	unsigned int *fchgprm, *tbmt, *tbmm, *fchglt;
	struct STR_RCBITMODEL bm[32];
};

void rc_init(struct RC *rc, UCHAR *p);
void rc_encode0(struct RC *rc, int bit);
void rc_encode1(struct RC *rc, unsigned int *prob, int bit);
void rc_flush(struct RC *rc);

#define MAX_REP		16

#define MODE_STK5	0	/* 0か1 */

#define EPRM_PB			0	/* a */
#define EPRM_LC			1	/* b */
#define EPRM_LP			2	/* c */
#define EPRM_LIT		3	/* d:0-3(0) */
#define EPRM_MCH		4	/* e:0-1(0) */
#define EPRM_SDS		5	/* f:0-1(0) */
#define EPRM_PST		6	/* g:0-1(0) */
#define EPRM_LLM		7	/* h:0-1(0) */
#define EPRM_EPM		8	/* i:0-1(0) */
#define EPRM_PLT		9	/* j:0-1(1) */
#define EPRM_NST		25	/* z */

void rc_treeout0(struct RC *rc, int len, unsigned int data);
void rc_treeout1(struct RC *rc, unsigned int *prb, int len, unsigned int data);
unsigned int rc_revbit(unsigned int data, int len);
void tek_conv_tek5_len(struct RC *rc, struct STR_PRB *prb, int mode, int s_pos, int len, int t);
void rc_substat(struct RC *rc, struct STR_RCBITMODEL *bm);

void tek_conv_tek5_setbm(struct STR_RCBITMODEL *bm, int t, int m)
{
	bm->t = t;
	bm->m = m;
	bm->prb1 = -1 << (m + t);
	bm->prb0 = ~bm->prb1;
	bm->prb1 |= 1 << t;
	bm->tmsk = (-1 << t) & 0xffff;
	bm->prb0 &= bm->tmsk;
	bm->prb1 &= bm->tmsk;
	return;
}

int tek_conv_tek5(int csiz, UCHAR *src, int osiz, UCHAR *tmp, UCHAR *dst, UCHAR *str_eprm)
{
	UCHAR *mclp0 = malloc(osiz * 8), *p, /* *src1 = src + csiz ,*/ pmch, mcby, lastuint8_t, *mclp;
	struct STR_PRB *prb = malloc(sizeof (struct STR_PRB));
	int i, j, k, m, *ip, l, d, r, rep[MAX_REP], bylz;
	int lc, lp, pb, s, pos, s_pos, m_pos, m_lp, nst;
	unsigned int testall_z2_a = 0x7fffffff, testall_z2_b = 0;
	UCHAR eprm[26+26], testall[10];
	struct RC *rc = malloc(sizeof (struct RC));
	static int state_table[] = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 4, 5 };
	tek_lzrestore_tek5(csiz, src, osiz, tmp, mclp0);
	src = tek_analyze_stk5_h(src, &lc, &lp, &pb, &i);
//printf("lc:%d lp:%d pb:%d\n", lc, lp, pb);

	eprm[EPRM_PB] = pb;
	eprm[EPRM_LC] = lc;
	eprm[EPRM_LP] = lp;
	for (i = EPRM_LIT; i <= EPRM_LLM; i++)
		eprm[i] = 0;
	eprm[EPRM_PLT] = 1;
	eprm[EPRM_NST] = 2; /* fullset, not small */

	testall[0] = 0;
	if (str_eprm != NULL && str_eprm[0] == '@' && str_eprm[1] == '\0')
		testall[0] = 1;

retry:

	rc->bm[0].lt = 0; /* 寿命なし */
	tek_conv_tek5_setbm(&rc->bm[0], 16 - 11, 5);

	rc->bm[1].lt = 0; /* 寿命なし */
	tek_conv_tek5_setbm(&rc->bm[1], 0, 16); /* 不変型(常に1/2) */

	p = str_eprm;
	if (testall[0] == 0) {
		if (p != NULL /* && p[0] == 'e' && p[1] == 'p' && p[2] == 'r' && p[3] == 'm' && p[4] == ':' */) {
		//	p += 5;
			while (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z')) {
				if ('a' <= *p && *p <= 'z')
					i = *p++ - 'a';
				else
					i = (*p++ - 'A') + 26;
				eprm[i] = 0;
				while ('0' <= *p && *p <= '9')
					eprm[i] = eprm[i] * 10 + (*p++ - '0');
			}
		}
	} else {
		if (testall[0] == 1) {
			/* z2試行フェーズ */
			for (i = 3; i <= 7; i++)
				eprm[i] = testall[i] = 0;
			eprm[EPRM_NST] = 2;
			testall[0] = 2;
			testall[1] = 3;
		} else if (testall[0] == 2) {
			if (eprm[3] < 3)
				eprm[3]++;
			else {
				eprm[3] = testall[3];
				testall[0] = 3;
				testall[1] = 4;
				eprm[4] = 1;
			}
		} else if (testall[0] == 3) {
			i = testall[1];
			eprm[i] = testall[i];
			if (i < 7) {
				i++;
				testall[1] = i;
				eprm[i] = 1;
			} else {
				testall[0] = 4;
				testall[2] = 2;
				testall[1] = 2;
				eprm[EPRM_NST] = 1;
				for (i = 3; i <= 7; i++)
					eprm[i] = 0;
			}
		} else if (testall[0] == 4) {
			if (eprm[EPRM_NST])
				eprm[EPRM_NST]--;
			else {
				eprm[EPRM_NST] = testall[2];
printf("eprm:z%d", eprm[EPRM_NST]);
				if (eprm[EPRM_NST] == 2) {
					for (i = 3; i <= 7; i++)
						eprm[i] = testall[i];
printf("d%de%df%dg%dh%d", eprm[3], eprm[4], eprm[5], eprm[6], eprm[7]);
				}
putchar('\n');
				testall[0] = 0;
			}
		}
	}

//printf("z%dd%de%df%dg%dh%d  ", eprm[EPRM_NST], eprm[3], eprm[4], eprm[5], eprm[6], eprm[7]);

	rc->rmsk = -1;
	rc->phase = 0;
	if (eprm[EPRM_NST] == 0) {
		rc->rmsk <<= 11;
		rc->phase = 1;
	//	eprm[EPRM_EPM] = 1;
	}

	rc->prob0 = (unsigned int *) prb;
	rc->probs = sizeof (struct STR_PRB) / sizeof (int);

	rc->fchgprm = (unsigned int *)prb->fchgprm;
	rc->tbmt = (unsigned int *)prb->tbmt;
	rc->tbmm = (unsigned int *)prb->tbmm;
	rc->fchglt = (unsigned int *)&prb->fchglt;

phaseloop:
	pb = eprm[EPRM_PB];
	lc = eprm[EPRM_LC];
	lp = eprm[EPRM_LP];
	nst = (eprm[EPRM_NST] != 0);

	m_pos = (1 << pb) - 1;
	m_lp = (1 << lp) - 1;
	for (i = 0; i < sizeof (struct STR_PRB) / sizeof (int); i++)
		((unsigned int *) prb)[i] = 1 << 15;

	/* 11-5なので、 111111_00001_00000 */
	prb->repg3 = 0xffff; /* 自動補正を当てにする */
	prb->repg4 = 0xffff;

	rc_init(rc, dst + (eprm[EPRM_NST] == 2) + 2); /* tek5であれば1バイト余白をあける */
	rc->idnext = 0;
	rc->log1 = rc->log0;
	if (nst) {
		for (i = 2; i < 32; i++) {
			rc->bm[i].lt = 1; /* 寿命あり */
			rc->bm[i].lt0 = TEK5_LIFETIME0;
			tek_conv_tek5_setbm(&rc->bm[i], 16 - 11, 5);
			rc->bm[i].id = -1;
			rc->bm[i].s = 0;
		}
		tek_conv_tek5_setbm(&rc->bm[5], 0, 1); /* repg3 */
		if (eprm[EPRM_NST] == 1) {
			rc->bm[2].lt0 = TEK5_LIFETIME0 / 2;
			rc->bm[5].lt = 0; /* 寿命なし(repg3) --- 最初の1bitを惜しんだ */
			for (i = 0; i < MAX_PB2; i++) {
				for (j = 0; j < MAX_STATE; j++)
					prb->pb[i].st[j].mch |= 3 << 16;
				for (j = 0; j < MAX_STATE; j++)
					prb->pb[i].st[j].rep0l1 |= 3 << 16;
				for (j = 0; j < MAX_STATE; j++) {
					prb->st[j].rep   |= 3 << 16;
					prb->st[j].repg0 |= 3 << 16;
					prb->st[j].repg1 |= 3 << 16;
					prb->st[j].repg2 |= 3 << 16;
					prb->repg3 |= 5 << 16;
				}
			}
			for (i = 0; i < 2; i++) {
				for (j = 0; j < 2; j++)
					prb->spdis1[i][j] |= 4 << 16;
				for (j = 0; j < 4; j++)
					prb->spdis2[i][j] |= 4 << 16;
				for (j = 0; j < 8; j++)
					prb->spdis3[i][j] |= 4 << 16;
				for (j = 0; j < 16; j++)
					prb->spdis4[i][j] |= 4 << 16;
				for (j = 0; j < 32; j++)
					prb->spdis5[i][j] |= 4 << 16;
				prb->lensel[i] |= 3 << 16;
				prb->lensel2[i] |= 3 << 16;
				for (j = 0; j < 256; j++)
					prb->lenhigh[i][j] |= 3 << 16;
				for (j = 0; j < MAX_PB2; j++) {
					for (r = 0; r < 8; r++) {
						prb->pb[j].lenlow[i][r] |= 3 << 16;
						prb->pb[j].lenmid[i][r] |= 3 << 16;
					}
				}
			}
			for (i = 0; i < 64; i++) {
				prb->pslot[0][i] |= 3 << 16;
				prb->pslot[1][i] |= 3 << 16;
				prb->pslot[2][i] |= 3 << 16;
				prb->pslot[3][i] |= 3 << 16;
				prb->algn[i] |= 4 << 16;
			}
			for (i = 0; i < MAX_LP2; i++) {
				for (j = 0; j < MAX_LC2; j++) {
					for (r = 0; r < 256; r++) {
						prb->lit[i][j][r + 256] |= 3 << 16;
						prb->lit[i][j][r + 512] |= 3 << 16;
					}
					prb->lit[i][j][1] |= 2 << 16;
					prb->lit[i][j][2] |= 2 << 16;
					prb->lit[i][j][3] |= 2 << 16;
					for (r = 4; r < 8; r++)
						prb->lit[i][j][r] |= 2 << 16;
					for (r = 8; r < 16; r++)
						prb->lit[i][j][r] |= 2 << 16;
					for (r = 16; r < 32; r++)
						prb->lit[i][j][r] |= 2 << 16;
					for (r = 32; r < 64; r++)
						prb->lit[i][j][r] |= 2 << 16;
					for (r = 64; r < 128; r++)
						prb->lit[i][j][r] |= 2 << 16;
					for (r = 128; r < 256; r++)
						prb->lit[i][j][r] |= 2 << 16;
				}
			}
			for (i = 0; i < 32; i++) {
				if (i < 2)
					prb->lenext1[i] |= 3 << 16;
				if (i < 4)
					prb->lenext2[i] |= 3 << 16;
				if (i < 8)
					prb->lenext3[i] |= 3 << 16;
				if (i < 16)
					prb->lenext4[i] |= 3 << 16;
			//	if (i < 32)
					prb->lenext5[i] |= 3 << 16;
			}
		} else {
			/* 所属の割り当て */
			for (i = 0; i < MAX_PB2; i++) {
				prb->pb[i].st[0].mch |= 2 << 16;
				for (j = 1; j < MAX_STATE; j++)
					prb->pb[i].st[j].mch |= (2 + eprm[EPRM_MCH]) << 16;
				for (j = 0; j < MAX_STATE; j++)
					prb->pb[i].st[j].rep0l1 |= 4 << 16;
			}
			for (j = 0; j < MAX_STATE; j++) {
				prb->st[j].rep   |= 4 << 16;
				prb->st[j].repg0 |= 4 << 16;
				prb->st[j].repg1 |= 4 << 16;
				prb->st[j].repg2 |= 4 << 16;
				prb->repg3 |= 5 << 16;
			}
			for (i = 0; i < 2; i++) {
				for (j = 0; j < 2; j++)
					prb->spdis1[i][j] |= 6 << 16;
				for (j = 0; j < 4; j++)
					prb->spdis2[i][j] |= 6 << 16;
				for (j = 0; j < 8; j++)
					prb->spdis3[i][j] |= 6 << 16;
				for (j = 0; j < 16; j++)
					prb->spdis4[i][j] |= (6 + eprm[EPRM_SDS]) << 16;
				for (j = 0; j < 32; j++)
					prb->spdis5[i][j] |= (6 + eprm[EPRM_SDS]) << 16;
				prb->lensel[i] |= 9 << 16;
				prb->lensel2[i] |= 9 << 16;
				for (j = 0; j < 256; j++)
					prb->lenhigh[i][j] |= 12 << 16;
				for (j = 0; j < MAX_PB2; j++) {
					for (r = 0; r < 8; r++) {
						prb->pb[j].lenlow[i][r] |= 10 << 16;
						prb->pb[j].lenmid[i][r] |= (10 + eprm[EPRM_LLM]) << 16;
					}
				}
			}
			for (i = 0; i < 64; i++) {
				prb->pslot[0][i] |= 14 << 16;
				prb->pslot[1][i] |= 14 << 16;
				prb->pslot[2][i] |= 14 << 16;
				prb->pslot[3][i] |= (14 + eprm[EPRM_PST]) << 16;
				prb->algn[i] |= 8 << 16;
			}
			for (i = 0; i < MAX_LP2; i++) {
				for (j = 0; j < MAX_LC2; j++) {
					static UCHAR table[4][8] = {
						{ 16, 16, 16, 16, 16, 16, 16, 16 },
						{ 16, 16, 16, 16, 20, 20, 20, 20 },
						{ 16, 16, 18, 18, 20, 20, 22, 22 },
						{ 16, 17, 18, 19, 20, 21, 22, 23 }
					};
					for (r = 0; r < 256; r++) {
						prb->lit[i][j][r + 256] |= 13 << 16;
						prb->lit[i][j][r + 512] |= 13 << 16;
					}
					prb->lit[i][j][1] |= ((int) table[eprm[EPRM_LIT]][0]) << 16;
					prb->lit[i][j][2] |= ((int) table[eprm[EPRM_LIT]][1]) << 16;
					prb->lit[i][j][3] |= ((int) table[eprm[EPRM_LIT]][1]) << 16;
					for (r = 4; r < 8; r++)
						prb->lit[i][j][r] |= ((int) table[eprm[EPRM_LIT]][2]) << 16;
					for (r = 8; r < 16; r++)
						prb->lit[i][j][r] |= ((int) table[eprm[EPRM_LIT]][3]) << 16;
					for (r = 16; r < 32; r++)
						prb->lit[i][j][r] |= ((int) table[eprm[EPRM_LIT]][4]) << 16;
					for (r = 32; r < 64; r++)
						prb->lit[i][j][r] |= ((int) table[eprm[EPRM_LIT]][5]) << 16;
					for (r = 64; r < 128; r++)
						prb->lit[i][j][r] |= ((int) table[eprm[EPRM_LIT]][6]) << 16;
					for (r = 128; r < 256; r++)
						prb->lit[i][j][r] |= ((int) table[eprm[EPRM_LIT]][7]) << 16;
				}
			}
			for (i = 0; i < 32; i++) {
				if (i < 2)
					prb->lenext1[i] |= 24 << 16;
				if (i < 4)
					prb->lenext2[i] |= 24 << 16;
				if (i < 8)
					prb->lenext3[i] |= 24 << 16;
				if (i < 16)
					prb->lenext4[i] |= 24 << 16;
			//	if (i < 32)
					prb->lenext5[i] |= 24 << 16;
			}
			for (i = 16; i <= 23; i++)
				rc->bm[i].lt0 = TEK5_LIFETIME0 / 2;
		}
		for (i = 0; i < 32 * 2; i++)
			prb->fchgprm[i] |= 25 << 16;
		for (i = 0; i < 16; i++) {
			prb->tbmm[i] |= 26 << 16;
			prb->tbmt[i] |= 26 << 16;
		}
		rc->bm[25].lt = 0; /* 寿命なし */
		tek_conv_tek5_setbm(&rc->bm[25], 5, 3); /* t=16-11, m=3 */
		rc->bm[26].lt = 0; /* 寿命なし */
		tek_conv_tek5_setbm(&rc->bm[26], 9, 2 /* + eprm[EPRM_BMP] * 2 */); /* t=16-7, m=2 */
		rc->bm[27].lt = 0; /* 寿命なし */
		tek_conv_tek5_setbm(&rc->bm[27], 5, 5); /* t=16-11, m=5 */
		prb->fchglt |= 27 << 16;
		if (eprm[EPRM_PLT]) {
			tek_conv_tek5_setbm(&rc->bm[27], 0, 1); /* t=16-16, m=1 */
			prb->fchglt |= 0xffff;
		}
	}

	pmch = 0; mcby = 0; lastuint8_t = 0;
	mclp = mclp0;
	s = 0; pos = 0;

	for (r = 0; r < MAX_REP; r++)
		rep[r] = ~r;

	for (;;) {
		/* byフェーズ */
		p = mclp;
		bylz = 0;
		if (*p == 0)
			break;
		while (*p == 1) {
			bylz++;
			p += 2;
		}

		/* bylzを出力 */
		do {
			s_pos = pos & m_pos;
			rc_encode1(rc, (unsigned int *)&prb->pb[s_pos].st[s].mch, 0^nst);
			s = state_table[s];
			if (pmch) {
				k = mcby;
				pmch = 0;
				for (i = 0; i < 8; i++) {
					k <<= 1;
					j = ((0x100 | mclp[1]) >> (8 - i)) + (k & 0x100) + 0x100;
					ip = &prb->lit[pos & m_lp][lastuint8_t >> (8 - lc)][j];
					if (((mcby ^ mclp[1]) >> (7 - i)) & 1)
						goto skip0;
					rc_encode1(rc, (unsigned int *)ip, (mclp[1] >> (7 - i)) & 1);
				}
			} else {
				for (i = 0; i < 8; i++) {
					j = (0x100 | mclp[1]) >> (8 - i);
					ip = &prb->lit[pos & m_lp][lastuint8_t >> (8 - lc)][j];
	skip0:
					rc_encode1(rc, (unsigned int *)ip, (mclp[1] >> (7 - i)) & 1);
				}
			}
			lastuint8_t = mclp[1];
			/* mclp[1]を出力 */
			mclp += 2;
			pos++;
		} while (--bylz);

		/* lzフェーズ */
		p = mclp;
		bylz = 0;
		if (*p == 0)
			break;
		while (*p >= 2) {
			bylz++;
			p++;
			tek1_getnum_s7s(&p);
			tek1_getnum_s7s(&p);
			tek1_getnum_s7s(&p);
		}
		/* bylzを出力 */
		do {
			i = *mclp++;
			l = tek1_getnum_s7s(&mclp);
			d = ~tek1_getnum_s7s(&mclp);
			r = tek1_getnum_s7s(&mclp);
			if (r == 0x3fff) {
				for (r = 0; r < MAX_REP; r++) {
					if (rep[r] == d)
						break;
				}
				if (r <= 3 && i == 2)
					r = -1; /* しょうがないので重複を許す */
				if (r >= MAX_REP)
					r = -1;
			} else {
				if (rep[r] != d) {
					printf("pos=%d r=%d rep[r]=%d d=%d i=%d\n", pos, r, rep[r], d, i);
					puts("error!\n"); exit(1);
					break;
				}
			}
			if (i == 2) r = -1;

			/* i, l, d, rを出力 */
			s_pos = pos & m_pos;
			rc_encode1(rc, (unsigned int *)&prb->pb[s_pos].st[s].mch, 1^nst);
			if (i == 2) {
				k = ~d;
				rc_encode1(rc, (unsigned int *)&prb->st[s].rep, 0^nst);
				s = s < 7 ? 7 : 10;
				tek_conv_tek5_len(rc, prb, 0, s_pos, l, nst);
				j = l - 2;
				if (j >= 4)
					j = 3;
				if (k <= 3) {
					/* pslotのみ */
					rc_treeout1(rc, (unsigned int *)prb->pslot[j], 6, k);
				} else {
					/* pslotを算出 */
					for (m = 0; (1 << m) <= k; m++);
					/* m = 3 : [4,7], m = 4 : [8,15] */
					rc_treeout1(rc, (unsigned int *)prb->pslot[j], 6, (m - 1) * 2 + ((k >> (m - 2)) & 1));
					if (m == 3)
						rc_treeout1(rc, (unsigned int *)prb->spdis1[(k >> 1) & 1], 1, k & 1);
					if (m == 4)
						rc_treeout1(rc, (unsigned int *)prb->spdis2[(k >> 2) & 1], 2, rc_revbit(k & 0x03, 2));
					if (m == 5)
						rc_treeout1(rc, (unsigned int *)prb->spdis3[(k >> 3) & 1], 3, rc_revbit(k & 0x07, 3));
					if (m == 6)
						rc_treeout1(rc, (unsigned int *)prb->spdis4[(k >> 4) & 1], 4, rc_revbit(k & 0x0f, 4));
					if (m == 7)
						rc_treeout1(rc, (unsigned int *)prb->spdis5[(k >> 5) & 1], 5, rc_revbit(k & 0x1f, 5));
					if (m >= 8) {
						if (nst == 0) {
							/* align=4 */
							rc_treeout0(rc, m - 6, (k >> 4) & ((1 << (m - 6)) - 1));
							rc_treeout1(rc, (unsigned int *)prb->algn, 4, rc_revbit(k & 0x0f, 4));
						} else {
							/* align=6 */
							if (m > 8)
								rc_treeout0(rc, m - 8, (k >> 6) & ((1 << (m - 8)) - 1));
							rc_treeout1(rc, (unsigned int *)prb->algn, 6, rc_revbit(k & 0x3f, 6));
						}
					}
				}
			} else {
				rc_encode1(rc, (unsigned int *)&prb->st[s].rep, 1^nst);
				if (r == 0) {
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg0, 0^nst);
					if (l == 1) {
						rc_encode1(rc, (unsigned int *)&prb->pb[s_pos].st[s].rep0l1, 0);
						s = s < 7 ? 9 : 11;
						goto skip1;
					}
					rc_encode1(rc, (unsigned int *)&prb->pb[s_pos].st[s].rep0l1, 1);
				} else if (r == 1) {
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg0, 1^nst);
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg1, 0^nst);
				} else if (r == 2) {
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg0, 1^nst);
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg1, 1^nst);
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg2, 0^nst);
				} else if (r == 3) {
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg0, 1^nst);
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg1, 1^nst);
					rc_encode1(rc, (unsigned int *)&prb->st[s].repg2, 1^nst);
					if (nst)
						rc_encode1(rc, (unsigned int *)&prb->repg3, 1);
				} else {
					printf("rep error! "); exit(1);
				}
				tek_conv_tek5_len(rc, prb, 1, s_pos, l, nst);
				s = s < 7 ? 8 : 11;
			}
skip1:
			pos += l;
			i = r;
			if (r < 0)
				i = MAX_REP - 1;
			for (; i >= 1; i--)
				rep[i] = rep[i - 1];
			rep[0] = d;
		} while (--bylz);
		pmch = 1 /* eprm[EPRM_EPM] */;
		lastuint8_t = tmp[pos - 1];
		mcby = tmp[pos + d];
	}

	if (rc->phase == 0) {
		for (i = 2; i < 32; i++) {
			if (rc->bm[i].id != -1)
				rc_substat(rc, &rc->bm[i]);
		}
		rc->phase++;
		goto phaseloop;
	}

	rc_flush(rc);

	dst[0] = eprm[EPRM_NST] << 2 | 0x01;
	dst[1] = pb * 45 + lp * 9 + lc;
	if (nst) {
		dst[1] = lp * 45 + pb * 9 + lc; /* lpがめったに1以上にならないことを利用 */
		if (eprm[EPRM_NST] == 2)
			dst[3] = eprm[EPRM_LIT] | eprm[EPRM_MCH] << 2 | eprm[EPRM_LLM] << 3
				| eprm[EPRM_PST] << 4 | eprm[EPRM_SDS] << 5 | eprm[EPRM_PLT] << 6;
	}

	i = rc->p - dst;
	if (nst) {
		if (dst[1] == 0x00)
			dst[0] |= 0x10;
	} else {
		if (dst[1] == 0x5d)
			dst[0] |= 0x10;
		else if (dst[1] == 0x00)
			dst[0] |= 0x20;
	}
	j = 2;
	r = 1;
	if (dst[0] & 0xf0) {
		j = 1;
		r = 2;
	}

	if (testall[0]) {
//printf("%d\n", i - r);
		if ((testall_z2_a > i - r) || (testall_z2_a == i - r && testall_z2_b < rc->range)) {
			testall_z2_a = i - r;
			testall_z2_b = rc->range;
			if (testall[1] != 2)
				testall[testall[1]] = eprm[testall[1]];
			else
				testall[2] = eprm[EPRM_NST];
		}
		goto retry;
	}

	for (; j < i - r; j++)
		dst[j] = dst[j + r];

//	printf("\nd:%d\n", (src1 - src) - (rc->p - dst) + 1);

//	LzmaDecodeMclp((UCHAR *) prb, sizeof (struct STR_PRB) / sizeof (int), lc, lp, pb, dst + 1, rc->p - dst - 1, tmp, pos, NULL);

//	i = rc->p - dst;
	free(mclp0);
	free(prb);
	free(rc);
	return i - r;
}

void rc_treeout0(struct RC *rc, int len, unsigned int data)
/* 上位ビットから出力 */
{
	int i;
	for (i = len; i > 0; i--)
		rc_encode0(rc, (data >> (i - 1)) & 1);
	return;
}

void rc_treeout1(struct RC *rc, unsigned int *prb, int len, unsigned int data)
/* 上位ビットから出力 */
{
	int i;
	prb--;
	data |= 1 << len;
	for (i = len; i > 0; i--)
		rc_encode1(rc, &prb[data >> i], (data >> (i - 1)) & 1);
	return;
}

unsigned int rc_revbit(unsigned int data, int len)
{
	unsigned int rev = 0;
	do {
		rev += rev + (data & 1);
		data >>= 1;
	} while (--len);
	return rev;
}

void tek_conv_tek5_len(struct RC *rc, struct STR_PRB *prb, int mode, int s_pos, int len, int t)
{
	int l0, l1/*, i, j, k */;
	len -= 2;
	if (len < 8) {
		rc_encode1(rc, (unsigned int *)&prb->lensel[mode], 0^t);
		rc_treeout1(rc, (unsigned int *)prb->pb[s_pos].lenlow[mode], 3, len);
		return;
	}
	if (len < 16) {
		rc_encode1(rc, (unsigned int *)&prb->lensel[mode], 1^t);
		rc_encode1(rc, (unsigned int *)&prb->lensel2[mode], 0^t);
		rc_treeout1(rc, (unsigned int *)prb->pb[s_pos].lenmid[mode], 3, len - 8);
		return;
	}
	rc_encode1(rc, (unsigned int *)&prb->lensel[mode], 1^t);
	rc_encode1(rc, (unsigned int *)&prb->lensel2[mode], 1^t);
	if (len < 16 + 256 - 8) {
		rc_treeout1(rc, (unsigned int *)prb->lenhigh[mode], 8, len - 16);
		return;
	}
	len -= 16 + 256 - 8 - 1;
	for (l0 = 1; len >= (1 << l0); l0++);
	for (l1 = 0; l0 >= (2 << l1); l1++);
	rc_treeout1(rc, (unsigned int *)prb->lenhigh[mode], 8, 256 - 8 + l1);
	if (l1 > 0) {
		/* l1 = 1...7 */
		if (t != 0 && l1 < 6) {
			/* t == 0 のときはrmskのせいでうまくいかない */
			if (l1 == 1)
				rc_treeout1(rc, (unsigned int *)prb->lenext1, 1, l0);
			if (l1 == 2)
				rc_treeout1(rc, (unsigned int *)prb->lenext2, 2, l0);
			if (l1 == 3)
				rc_treeout1(rc, (unsigned int *)prb->lenext3, 3, l0);
			if (l1 == 4)
				rc_treeout1(rc, (unsigned int *)prb->lenext4, 4, l0);
			if (l1 == 5)
				rc_treeout1(rc, (unsigned int *)prb->lenext5, 5, l0);
		} else
			rc_treeout0(rc, l1, l0);
	}
	l0--;
	if (l0 > 0)
		rc_treeout0(rc, l0, len);
	return;
}

void rc_init(struct RC *rc, UCHAR *p)
{
	rc->p = p;
	rc->code = 0;
	rc->range = 0xffffffff;
	rc->ffnum = 0;
	rc->cache = 0;
}

void rc_shift(struct RC *rc)
{
	if (rc->code < (unsigned int) 0xff000000 || (unsigned int) (rc->code >> 32) == 1) {
		*rc->p++ = rc->cache + (UCHAR) (rc->code >> 32);
		if (rc->ffnum) {
			do {
				*rc->p++ = 0xff + (UCHAR) (rc->code >> 32);
			} while (--rc->ffnum);
		}
		rc->cache = (UCHAR) (((unsigned int) rc->code) >> 24);
	} else
		rc->ffnum++;
	rc->code = ((unsigned int) rc->code) << 8;
	return;
}

void rc_encode0(struct RC *rc, int bit)
{
	rc->range >>= 1;
	if (bit == 0)
		rc->code += rc->range;
	if (rc->range < (unsigned int) (1 << 24)) {
		rc->range <<= 8;
		rc_shift(rc);
	}
	return;
}

struct STR_BACKUP_PROB *rc_subsaveprb(struct STR_BACKUP_PROB *b, unsigned int *log0, unsigned int *log1, int probs, unsigned int *prob0, UCHAR *prbflg)
/* flagsの第一ビットを使用 */
{
	unsigned int i;
	for (i = 0; i < probs; i++)
		prbflg[i] |= 0x01;
	do {
		i = *log0 >> 1;
		if (prbflg[i] & 0x01) {
			prbflg[i] &= 0xfe;
			b->prob = prob0 + i;
			b->data = prob0[i];
			b++;
		}
		log0++;
	} while (log0 < log1);
	b->prob = NULL;
	return b;
}

void rc_subloadprb(struct STR_BACKUP_PROB *b)
{
	if (b->prob) {
		do {
			*(b->prob) = b->data;
			b++;
		} while (b->prob);
	}
	return;
}

unsigned int rc_subtest(struct RC *rc, unsigned int *p0, unsigned int *p1, unsigned int move, unsigned int total, unsigned int *prange)
/* 最初の0x00があるので、常に+1したバイト数を返す */
/* 前もってprobのバックアップをしておくこと */
/* totalは 16-旧total の意味 */
{
	unsigned int range = 0xffffffff, uint8_ts = 0, prbmax, prbmin;
#if 0
	if (total == 15) {
		/* オール0かオール1 */
		prbmin = ((- move) & 0xffff) ^ 1;
		do {
			unsigned int *pprob = &rc->prob0[*p0 >> 1];
			if ((*pprob & 0xffff) != prbmin)
				*pprob = (*pprob & 0xffff) | prbmin;
			uint8_ts |= (*p0++ ^ move) & 1;
		} while (p0 < p1);
		uint8_ts = - uint8_ts;
		*prange = ~uint8_ts;
		return (uint8_ts | 1) & 0x7ffffff1; /* 一応それなりに扱いやすい数字に補正 */
	}
#endif
	prbmax = -1 << (move + total);
	prbmin = ~prbmax;
	prbmax |= 1 << total;
	total = (-1 << total) & 0xffff;
	prbmin &= total; /* これは下位を殺すためであって、上位はどうでもいい。 --- とんでもない、上位も大事 */
	prbmax &= total;
//	例：
//	move = 4,total=4のとき
//	111111110001 0000
//	000000001111 0000
	do {
		unsigned int *pprob = &rc->prob0[*p0 >> 1], prob = *pprob & 0xffff;
		unsigned int bound = ((unsigned long long) (range & rc->rmsk) * prob) >> 16;
		if (prob < prbmin) {
			prob = prbmin;
			goto fixprob;
		}
		if (prob > prbmax) {
			prob = prbmax;
			goto fixprob;
		}
		if (prob & ~total) {
			prob &= total;
fixprob:
			*pprob = (*pprob & 0xffff0000) | prob;
		}
		if (*p0 & 1) {
			range = bound;
			*pprob += ((0x10000 - prob) >> move) & total;
		} else {
			range -= bound;
			*pprob -= (prob >> move) & total;
		}
		if (range < (unsigned int) (1 << 24)) {
			do {
				range <<= 8;
				uint8_ts++;
			} while (range < (unsigned int) (1 << 24));
		}
		p0++;
	} while (p0 < p1);
	*prange = range;
	return uint8_ts;
}

int rc_subcomp(unsigned int b0, unsigned int r0, unsigned int b1, unsigned int r1)
/* 0がよければ－、1がよければ＋ */
{
	if (b0 < b1)
		return -1;
	if (b0 > b1)
		return 1;
	if (r0 > r1)
		return -1;
	if (r0 < r1)
		return 1;
	return 0;
}

void rc_substat(struct RC *rc, struct STR_RCBITMODEL *bm)
{
	unsigned int min_b, min_r, min_m, min_t, tmp_b, tmp_r;
	int i, j;
	rc_subsaveprb(rc->backup, bm->logptr0, bm->logptr, rc->probs, rc->prob0, rc->prbflg);
//	min_b = rc_subtest(rc, bm->logptr0, bm->logptr, min_m = bm->m, min_t = bm->t, &min_r) - 1;
	min_b = 0x7fffffff;
	min_r = 0;
	i = bm->m;
	do {
		tmp_b = rc_subtest(rc, bm->logptr0, bm->logptr, i, 0, &tmp_r);
		if ((j = rc_subcomp(min_b, min_r, tmp_b, tmp_r)) > 0) {
			min_m = i;
			min_b = tmp_b;
			min_r = tmp_r;
		}
		rc_subloadprb(rc->backup);
		i--;
	} while (j >= 0 && i >= 1);
	i = bm->m + 1;
	if (i <= 16) {
		do {
			tmp_b = rc_subtest(rc, bm->logptr0, bm->logptr, i, 0, &tmp_r);
			if ((j = rc_subcomp(min_b, min_r, tmp_b, tmp_r)) > 0) {
				min_m = i;
				min_b = tmp_b;
				min_r = tmp_r;
			}
			rc_subloadprb(rc->backup);
			i++;
		} while (j >= 0 && i <= 16);
	}
	i = 1;
	min_t = 0;
	do {
		tmp_b = rc_subtest(rc, bm->logptr0, bm->logptr, min_m, i, &tmp_r);
		if ((j = rc_subcomp(min_b, min_r, tmp_b, tmp_r)) > 0) {
			min_t = i;
			min_b = tmp_b;
			min_r = tmp_r;
		}
		rc_subloadprb(rc->backup);
		i++;
	} while (j >= 0 && i <= 14);

	tmp_b = rc_subtest(rc, bm->logptr0, bm->logptr, bm->m, bm->t, &tmp_r) - 1;
	if ((j = rc_subcomp(min_b, min_r, tmp_b, tmp_r)) > 0) {
		min_m = bm->m;
		min_t = bm->t;
		min_b = tmp_b;
		min_r = tmp_r;
	}
#if 0
	for (i = 0; i < 2; i++) {
		tmp_b = rc_subtest(rc, bm->logptr0, bm->logptr, i, 15, &tmp_r);
		if ((j = rc_subcomp(min_b, min_r, tmp_b, tmp_r)) > 0) {
			min_m = bm->m;
			min_t = bm->t;
			min_b = tmp_b;
			min_r = tmp_r;
		}
	}
#endif

	rc_subloadprb(rc->backup);
	rc_subtest(rc, bm->logptr0, bm->logptr, min_m, min_t, &tmp_r);
	/* これをログに出す */

//printf("[%02d %d %d] ", bm - rc->bm, min_m, min_t);

	rc->log1->m = bm->m = min_m;
	rc->log1->t = bm->t = min_t;
	rc->log1->id = bm->id;
	rc->log1++;
	return;
}

void rc_encode1(struct RC *rc, unsigned int *prob, int bit)
/* 所属ビット方式 */
{
	unsigned int bound, p = *prob & 0xffff;
	struct STR_RCBITMODEL *bm = &rc->bm[*prob >> 16];
	if (rc->phase == 0) {
		/* 統計フェーズ */
		if (bm->lt > 0) {
			/* 寿命がある */
			if (--bm->lt == 0) {
				/* 寿命が尽きた */
				if (bm->id != -1) {
					/* bm->logptrからあれこれ算出してrc->log0[]へ登録 */
					rc_substat(rc, bm);
				}
				bm->lt = bm->lt0;
				bm->id = rc->idnext++;
				bm->logptr = bm->logptr0;
			}
			*(bm->logptr)++ = (prob - rc->prob0) << 1 | bit;
		}
		/* 寿命が無いなら何もしない */
	} else {
		/* 出力フェーズ */
		if (bm->lt > 0) {
			/* 寿命がある */
			if (--bm->lt == 0) {
				/* 寿命が尽きた */
				rc_encode1(rc, rc->fchglt, 1); /* 寿命変更なし */
				bound = 0;
				while (rc->log0[bound].id != rc->idnext)
					bound++;
				if (bm->t == rc->log0[bound].t && bm->m == rc->log0[bound].m) {
					rc_encode1(rc, &rc->fchgprm[bm->s + (bm - rc->bm) * 2], 1);
					bm->s = 1;
				} else {
					rc_encode1(rc, &rc->fchgprm[bm->s + (bm - rc->bm) * 2], 0);
					bm->s = 0;
					rc_treeout1(rc, rc->tbmt, 4, bm->t = rc->log0[bound].t);
					bm->m = rc->log0[bound].m;
				//	rc_treeout1(rc, rc->tbmm + bm->t * 16, 4, bm->m);
				//	if (bm->t < 15)
						rc_treeout1(rc, rc->tbmm,   4, bm->m);
				//	else
				//		rc_treeout1(rc, rc->tbmm15, 4, bm->m);
					tek_conv_tek5_setbm(bm, bm->t, bm->m);
				}
				rc->idnext++;
				bm->lt = bm->lt0;
			}
		}
		if (p < bm->prb0) {
			p = bm->prb0;
			goto fixprob;
		}
		if (p > bm->prb1) {
			p = bm->prb1;
			goto fixprob;
		}
		if (p & ~bm->tmsk) {
			p &= bm->tmsk;
fixprob:
			*prob = (*prob & 0xffff0000) | p;
		}
		if (bm->t < 15) {
			bound = ((unsigned long long) (rc->range & rc->rmsk) * p) >> 16;
			if (bit) {
				rc->range = bound;
				*prob += ((0x10000 - p) >> bm->m) & bm->tmsk;
			} else {
				rc->code += bound;
				rc->range -= bound;
				*prob -= (p >> bm->m) & bm->tmsk;
			}
			if (rc->range < (unsigned int) (1 << 24)) {
				do {
					rc->range <<= 8;
					rc_shift(rc);
				} while (rc->range < (unsigned int) (1 << 24));
			}
		}
	}
	return;
}

void rc_flush(struct RC *rc)
{
	int i;
	for (i = 0; i < 5; i++)
		rc_shift(rc);
	return;
}

#include <setjmp.h>

typedef unsigned int UINT32;
typedef unsigned long long UINT64;

#define tek_getnum_s7s(pp)	tek1_getnum_s7s(pp)

int tek_decmain5(int *work, UCHAR *src, int osiz, UCHAR *q, int lc, int pb, int lp, int flags, UCHAR *mclp);

int tek_lzrestore_tek5(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf, UCHAR *mclp)
{
	int wrksiz, lc, lp, pb, flags, *work;

	src = tek_analyze_stk5_h(src, &lc, &lp, &pb, &flags);
	if (src == NULL)
		return 1;
	wrksiz = (0xc00 + (0x300 << (lc + lp))) * sizeof (UINT32); /* 最低15KB, lc+lp=3なら、36KB */
	work = malloc(wrksiz);
	if (work == NULL)
		return -1;

	return tek_decmain5(work, src, outsiz, outbuf, lc, pb, lp, flags, mclp);
}

struct tek_STR_BITMODEL {
	UINT32 t, m, prb0, prb1, tmsk, ntm, lt, lt0, s;
};

struct tek_STR_RNGDEC {
	UCHAR *p;
	UINT32 range, code, rmsk, *fchglt, *fchgprm, *tbmt, *tbmm;
	jmp_buf errjmp;
	struct tek_STR_BITMODEL bm[32];
};

struct tek_STR_PRB {
	struct tek_STR_PRB_PB {
		struct tek_STR_PRB_PBST {
			UINT32 mch, rep0l1;
		} st[12];
		UINT32 lenlow[2][8], lenmid[2][8];
	} pb[16];
	struct tek_STR_PRB_ST {
		UINT32 rep, repg0, repg1, repg2;
	} st[12];
	UINT32 lensel[2][2], lenhigh[2][256], pslot[4][64], algn[64];
	UINT32 spdis[2][2+4+8+16+32], lenext[2+4+8+16+32];
	UINT32 repg3, fchgprm[2 * 32], tbmt[16], tbmm[16], fchglt;
	UINT32 lit[1];
};

struct tek_STR_WORK5 {
	struct tek_STR_RNGDEC rd;
	struct tek_STR_PRB prb;
};

void tek_setbm5(struct tek_STR_BITMODEL *bm, int t, int m)
{
	bm->t = t;
	bm->m = m;
	bm->prb1 = -1 << (m + t);
	bm->prb0 = ~bm->prb1;
	bm->prb1 |= 1 << t;
	bm->tmsk = (-1 << t) & 0xffff;
	bm->prb0 &= bm->tmsk;
	bm->prb1 &= bm->tmsk;
	bm->ntm = ~bm->tmsk;
	return;
}

int tek_rdget0(struct tek_STR_RNGDEC *rd, int n, int i)
{
	do {
		if (rd->range < (UINT32) (1 << 24))
			goto shift;
shift1:
		rd->range >>= 1;
		i += i;
		if (rd->code >= rd->range) {
			rd->code -= rd->range;
			i |= 1;
		}
	} while (--n);
	return ~i;
shift:
	do {
		rd->range <<= 8;
		rd->code = rd->code << 8 | *rd->p++;
	} while (rd->range < (UINT32) (1 << 24));
	goto shift1;
}

int tek_rdget1(struct tek_STR_RNGDEC *rd, UINT32 *prob0, int n, int j)
{
	UINT32 p, i, *prob;
	struct tek_STR_BITMODEL *bm;
	prob0 -= j;
	do {
		prob = prob0 + j;
		bm = &rd->bm[*prob >> 16];
		p = *prob & 0xffff;
		if (bm->lt > 0) {
			if (--bm->lt == 0) {
				/* 寿命切れ */
				if (tek_rdget1(rd, rd->fchglt, 1, 0) == 0) {
		err:
					longjmp(rd->errjmp, 1);
				}
				if ((bm->s = tek_rdget1(rd, &rd->fchgprm[(*prob >> 16) * 2 + bm->s], 1, 0)) == 0) {
					if ((i = tek_rdget1(rd, rd->tbmt, 4, 1) & 15) == 15)
						goto err;
					tek_setbm5(bm, i, ((tek_rdget1(rd, rd->tbmm, 4, 1) - 1) & 15) + 1);
				}
				bm->lt = bm->lt0;
			}
			if (p < bm->prb0) {
				p = bm->prb0;
				goto fixprob;
			}
			if (p > bm->prb1) {
				p = bm->prb1;
				goto fixprob;
			}
			if (p & bm->ntm) {
				p &= bm->tmsk;
	fixprob:
				*prob = (*prob & 0xffff0000) | p;
			}
		}
		if (rd->range < (UINT32) (1 << 24))
			goto shift;
shift1:
		j += j;
		i = ((unsigned long long) (rd->range & rd->rmsk) * p) >> 16;
		if (rd->code < i) {
			j |= 1;
			rd->range = i;
			*prob += ((0x10000 - p) >> bm->m) & bm->tmsk;
		} else {
			rd->range -= i;
			rd->code -= i;
			*prob -= (p >> bm->m) & bm->tmsk;
		}
	} while (--n);
	return j;
shift:
	do {
		rd->range <<= 8;
		rd->code = rd->code << 8 | *rd->p++;
	} while (rd->range < (UINT32) (1 << 24));
	goto shift1;
}

UINT32 tek_revbit(UINT32 data, int len)
{
	UINT32 rev = 0;
	do {
		rev += rev + (data & 1);
		data >>= 1;
	} while (--len);
	return rev;
}

int tek_getlen5(struct tek_STR_RNGDEC *rd, struct tek_STR_PRB *prb, int m, int s_pos, int stk)
{
	int i;
	if (tek_rdget1(rd, &prb->lensel[m][0], 1, 0) ^ stk) /* low */
		i = tek_rdget1(rd, prb->pb[s_pos].lenlow[m], 3, 1) & 7;
	else if (tek_rdget1(rd, &prb->lensel[m][1], 1, 0) ^ stk) /* mid */
		i = tek_rdget1(rd, prb->pb[s_pos].lenmid[m], 3, 1);
	else {
		/* high */
		i = tek_rdget1(rd, prb->lenhigh[m], 8, 1) - (256 + 256 - 8);
		if (i > 0) {
			if (i < 6 && stk == 0)
				i = tek_rdget1(rd, &prb->lenext[(1 << i) - 2], i, 1) - 1;
			else
				i = tek_rdget0(rd, i, ~1) - 1;
			i = tek_rdget0(rd, i, ~1) - 1;
		}
		i += 256 - 8 + 16;
	}
	return i;
}

int tek_decmain5(int *work, UCHAR *src, int osiz, UCHAR *q, int lc, int pb, int lp, int flags, UCHAR *mclp)
{
	static int state_table[] = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 4, 5 };
	int i, j, k, pmch, rep[4], s, pos, m_pos = (1 << pb) - 1, m_lp = (1 << lp) - 1;
	int stk = (flags == -1), lcr = 8 - lc, s_pos;
	UINT32 *lit1;
	struct tek_STR_PRB *prb = &((struct tek_STR_WORK5 *) work)->prb;
	struct tek_STR_RNGDEC *rd = &((struct tek_STR_WORK5 *) work)->rd;

	rd->p = &src[4];
	rd->range |= -1;
	rd->code = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
	for (i = 0; i < 4; i++)
		rep[i] = ~i;
	if (setjmp(rd->errjmp))
		goto err;
	for (i = sizeof (struct tek_STR_PRB) / sizeof (UINT32) + (0x300 << (lc + lp)) - 2; i >= 0; i--)
		((UINT32 *) prb)[i] = 1 << 15;
	for (i = 0; i < 32; i++) {
		rd->bm[i].lt = (i >= 4); /* 0..3は寿命なし */
		rd->bm[i].lt0 = (i < 24) ? 16 * 1024 : 8 * 1024;
		rd->bm[i].s &= 0;
		rd->bm[i].t = rd->bm[i].m = 5;
	}
	lit1 = prb->lit + ((256 << (lc + lp)) - 2);
	if (stk)
		rd->rmsk = -1 << 11;
	else {
		UCHAR pt[24];
		static UCHAR pt1[24] = {
			 8,  8,  8,  8,  8,  8,  8,  8,
			 8,  8, 18, 18, 18,  8, 22,  0,
			24, 24, 24, 24, 24, 24, 24, 24
		};
		static UCHAR pt2[24] = {
			 8,  8, 10, 11, 12, 12, 14, 15,
			16, 16, 18, 18, 20, 21, 22,  0,
			24, 24, 24, 24, 24, 24, 24, 24
		};
		/*
			 0- 7:mch, mch, lit1, lensel, lenlow, lenmid, lenhigh, lenext
			 8-15:pslot, pslot, sdis, sdis, align, rep-repg2, repg3, rep0l1
			16-23:lit0
		*/
		rd->rmsk |= -1;
		rd->bm[1].t = 5; rd->bm[1].m = 3; /* for fchgprm */
		rd->bm[2].t = 9; rd->bm[2].m = 2; /* for tbmt, tbmm */
		prb->fchglt = 0x00038000;
		if (flags & 0x40) { /* lt-flag */
			rd->bm[3].t = 0; rd->bm[3].m = 1;
			prb->fchglt = 0x0003ffff;
		}
		for (i = 0; i < 32 * 2; i++)
			prb->fchgprm[i] |= 1 << 16;
		for (i = 0; i < 16; i++) {
			prb->tbmm[i] |= 2 << 16;
			prb->tbmt[i] |= 2 << 16;
		}
		rd->bm[22].t = 0; rd->bm[22].m = 1;
		prb->repg3 = 22 << 16 | 0xffff;
		if (flags == -2) { /* z1 */
			rd->bm[22].lt = 0; /* repg3のltを0に */
			for (i = 0; i < 24; i++)
				pt[i] = pt1[i];
		} else {
			for (i = 0; i < 24; i++)
				pt[i] = pt2[i];
			if ((flags & 3) == 0x01)
				pt[20] = pt[21] = pt[22] = pt[23] = 28;
			if ((flags & 3) == 0x02) {
				for (i = 0; i < 8; i += 2)
					pt[16 + i + 0] = pt[16 + i + 1] = 24 + i;
			}
			if ((flags & 3) == 0x03) {
				for (i = 16; i < 24; i++)
					pt[i] = i + 8;
			}
			if (flags & 0x04) /* mch */
				pt[1] = 9;
			if (flags & 0x08) /* llm */
				pt[5] = 13;
			if (flags & 0x10) /* pst */
				pt[9] = 17;
			if (flags & 0x20) /* sds */
				pt[11] = 19;
		}
		for (i = 0; i < 12; i++) {
			for (j = 0; j < 16; j++) {
				prb->pb[j].st[0].mch    |= pt[ 0] << 16;
				prb->pb[j].st[i].rep0l1 |= pt[13] << 16;
				if (i)
					prb->pb[j].st[i].mch |= pt[ 1] << 16;
			}
			prb->st[i].rep   |= pt[13] << 16;
			prb->st[i].repg0 |= pt[13] << 16;
			prb->st[i].repg1 |= pt[13] << 16;
			prb->st[i].repg2 |= pt[13] << 16;
		}
		for (i = 0; i < 2; i++) {
			for (j = 0; j < 16; j++) {
				for (k = 0; k < 8; k++) {
					prb->pb[j].lenlow[i][k] |= pt[ 4] << 16;
					prb->pb[j].lenmid[i][k] |= pt[ 5] << 16;
				}
			}
			prb->lensel[i][0] |= pt[ 3] << 16;
			prb->lensel[i][1] |= pt[ 3] << 16;
			for (j = 0; j < 256; j++)
				prb->lenhigh[i][j] |= pt[ 6] << 16;
			for (j = 0; j < 2 + 4 + 8; j++)
				prb->spdis[i][j] |= pt[10] << 16;
			for (j = 2 + 4 + 8; j < 2 + 4 + 8 + 16 + 32; j++)
				prb->spdis[i][j] |= pt[11] << 16;
		}
		for (i = 0; i < 64; i++) {
			prb->algn[i] |= pt[12] << 16; 
			prb->pslot[3][i] |= pt[ 9] << 16;
			for (j = 0; j < 3; j++)
				prb->pslot[j][i] |= pt[ 8] << 16;
		}
		for (i = 0; i < 2 + 4 + 8 + 16 + 32; i++)
			prb->lenext[i] |= pt[ 7] << 16;
		for (i = (0x100 << (lc + lp)) - 0x100; i >= 0; i -= 0x100) {
			s = 0;
			for (j = 0; j < 8; j++) {
				for (k = (1 << j); k > 0; k--) {
					prb->lit[i + s] |= pt[16 + j] << 16;
					lit1[(i + s) * 2 + 2] |= pt[ 2] << 16;
					lit1[(i + s) * 2 + 3] |= pt[ 2] << 16;
					s++;
				}
			}
		}
	}
	for (i = 0; i < 32; i++)
		tek_setbm5(&rd->bm[i], rd->bm[i].t, rd->bm[i].m);
	rd->fchgprm = prb->fchgprm;
	rd->tbmt = prb->tbmt;
	rd->tbmm = prb->tbmm;
	rd->fchglt = &prb->fchglt;

	if ((tek_rdget1(rd, &prb->pb[0].st[0].mch, 1, 0) ^ stk) == 0)
		goto err;
	*q++ = tek_rdget1(rd, prb->lit, 8, 1) & 0xff;
	if (mclp) {
		mclp[0] = 0x01;
		mclp[1] = q[-1];
		mclp += 2;
	}
	pmch &= 0; s &= 0; pos = 1;
	do {
		s_pos = pos & m_pos;
		if (tek_rdget1(rd, &prb->pb[s_pos].st[s].mch, 1, 0) ^ stk) { /* 非lz */
			i = (q[-1] >> lcr | (pos & m_lp) << lc) << 8;
			s = state_table[s];
			if (pmch == 0)
				*q = tek_rdget1(rd, &prb->lit[i], 8, 1) & 0xff;
			else {
				j = 1; /* lit1は最初から2を減じてある */
				k = 8;
				pmch = q[rep[0]];
				do {
					j += j + tek_rdget1(rd, &lit1[(i + j) << 1 | pmch >> 7], 1, 0);
					k--;
					if ((((pmch >> 7) ^ j) & 1) != 0 && k != 0) {
						j = tek_rdget1(rd, &prb->lit[i + j - 1], k, j);
						break;
					}
					pmch <<= 1;
				} while (k);
				*q = j & 0xff;
				pmch &= 0;
			}
			if (mclp) {
				mclp[0] = 0x01;
				mclp[1] = *q;
				mclp += 2;
			}
			pos++;
			q++;
		} else { /* lz */
			pmch |= 1;
			if (tek_rdget1(rd, &prb->st[s].rep, 1, 0) ^ stk) { /* len/dis */
				if (mclp)
					*mclp = 0x02;
				rep[3] = rep[2];
				rep[2] = rep[1];
				rep[1] = rep[0];
				j = i = tek_getlen5(rd, prb, 0, s_pos, stk);
				s = s < 7 ? 7 : 10;
				if (j >= 4)
					j = 3;
				rep[0] = j = tek_rdget1(rd, prb->pslot[j], 6, 1) & 0x3f;
				if (j >= 4) {
					k = (j >> 1) - 1; /* k = [1, 30] */
					rep[0] = (2 | (j & 1)) << k;
					if (j < 14) /* k < 6 */
						rep[0] |= tek_revbit(tek_rdget1(rd, &prb->spdis[j & 1][(1 << k) - 2], k, 1), k);
					else {
						if (stk == 0) {
							if (k -= 6)
								rep[0] |= tek_rdget0(rd, k, ~0) << 6;
							rep[0] |= tek_revbit(tek_rdget1(rd, prb->algn, 6, 1), 6);
						} else {
							rep[0] |= tek_rdget0(rd, k - 4, ~0) << 4;
							rep[0] |= tek_revbit(tek_rdget1(rd, prb->algn, 4, 1), 4);
						}
					}
				}
				rep[0] = ~rep[0];
				k = 0x3fff;
			} else { /* repeat-dis */
				if (mclp)
					*mclp = 0x03;
				if (tek_rdget1(rd, &prb->st[s].repg0, 1, 0) ^ stk) { /* rep0 */
					i |= -1;
					k = 0;
					if (tek_rdget1(rd, &prb->pb[s_pos].st[s].rep0l1, 1, 0) == 0) {
						s = s < 7 ? 9 : 11;
						goto skip;
					}
				} else {
					if (tek_rdget1(rd, &prb->st[s].repg1, 1, 0) ^ stk) { /* rep1 */
						i = rep[1];
						k = 1;
					} else {
						if (tek_rdget1(rd, &prb->st[s].repg2, 1, 0) ^ stk) { /* rep2 */
							i = rep[2];
							k = 2;
						} else {
							if (stk == 0) {
								if  (tek_rdget1(rd, &prb->repg3, 1, 0) == 0)
									goto err;
							}
							i = rep[3]; /* rep3 */
							k = 3;
							rep[3] = rep[2];
						}
						rep[2] = rep[1];
					}
					rep[1] = rep[0];
					rep[0] = i;
				}
				i = tek_getlen5(rd, prb, 1, s_pos, stk);
				s = s < 7 ? 8 : 11;
			}
skip:
			i += 2;
			if (pos + rep[0] < 0)
				goto err;

			if (mclp) {
				mclp = tek1_puts7sp(mclp + 1, i);
				mclp = tek1_puts7sp(mclp, ~rep[0]);
				mclp = tek1_puts7sp(mclp, k);
			}
			pos += i;
			if (pos > osiz)
				goto err;
			do {
				*q = q[rep[0]];
				q++;
			} while (--i);
		}
	} while (pos < osiz);
	if (mclp)
		*mclp = 0x00;
	return 0;
err:
	return 1;
}

