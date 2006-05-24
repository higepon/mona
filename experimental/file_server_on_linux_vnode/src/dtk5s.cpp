//#include <setjmp.h>
#ifdef ON_LINUX
#include <stdlib.h>		/* malloc */
#include <stdio.h>		/* NULL */
#endif
#include <monapi/messages.h>
#ifndef ON_LINUX
#include <monapi/syscall.h>
#endif
#include "FileServer.h"
#include "dtk5s.h"

typedef unsigned int UINT32;
typedef unsigned char UCHAR;
typedef UINT32 tek_TPRB;

int tek_checkformat(int siz, UCHAR *p); /* 展開後のサイズを返す */
	/* -1:非osacmp */
	/* -2:osacmpだが対応できない */

int tek_decode(int siz, UCHAR *p, UCHAR *q); /* 成功したら0 */
	/* 正の値はフォーマットの異常・未対応、負の値はメモリ不足 */

static unsigned int tek_getnum_s7s(UCHAR **pp);
static int tek_lzrestore_tek5(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf);
static int tek_decmain5(int *work, UCHAR *src, int osiz, UCHAR *q, int lc, int pb, int lp);

int tek_checkformat(int siz, UCHAR *p)
{
	static UCHAR header[] = "\xff\xff\xff\x01\x00\x00\x00" "OSASKCMP";
	int i;
	if (siz < 17)
		return -1;
	for (i = 0; i < 15; i++) {
		if (p[i + 1] != header[i])
			return -1;
	}
	if (p[0] != 0x89)
		return -2;
	p += 16;
	return tek_getnum_s7s(&p);
}

int tek_decode(int siz, UCHAR *p, UCHAR *q)
{
	UCHAR *p1 = p + siz;
	int dsiz, hed, bsiz, st = 0;
	p += 16;
	if ((dsiz = tek_getnum_s7s(&p)) > 0) {
		hed = tek_getnum_s7s(&p);
		if ((hed & 1) == 0)
			st = tek_lzrestore_tek5(p1 - p + 1, p - 1, dsiz, q);
		else {
			bsiz = 1 << (((hed >> 1) & 0x0f) + 8);
			if (hed & 0x20)
				return 1;
			if (bsiz == 256)
				st = tek_lzrestore_tek5(p1 - p, p, dsiz, q);
			else {
				if (dsiz > bsiz)
					return 1;
				if (hed & 0x40)
					tek_getnum_s7s(&p); /* オプション情報へのポインタを読み飛ばす */
				st = tek_lzrestore_tek5(p1 - p, p, dsiz, q);
			}
		}
	}
	return st;
}

static unsigned int tek_getnum_s7s(UCHAR **pp)
/* これは必ずbig-endian */
/* 下駄がないので中身をいじりやすい */
{
	unsigned int s = 0;
	UCHAR *p = *pp;
	do {
		s = s << 7 | *p++;
	} while ((s & 1) == 0);
	s >>= 1;
	*pp = p;
	return s;
}

static int tek_lzrestore_tek5(int srcsiz, UCHAR *src, int outsiz, UCHAR *outbuf)
{
	int wrksiz, lc, lp, pb, *work, prop0, fl;

	if ((fl = (prop0 = *src) & 0x0f) != 0x01) /* 0001 */
		return 1;
	src++;
	prop0 >>= 4;
	if (prop0 == 0)
		prop0 = *src++;
	else {
		static UCHAR prop0_table[] = { 0x5d, 0x00 };
		if (prop0 >= 3)
			return 1;
		prop0 = prop0_table[prop0 - 1];
	}
	pb = prop0 / (9 * 5);
	prop0 %= 9 * 5;
	lp = prop0 / 9;
	lc = prop0 % 9;
	wrksiz = (0x800 + (0x300 << (lc + lp))) * sizeof (tek_TPRB); /* 最低11KB, lc+lp=3なら、32KB */
	work = (int *)malloc(wrksiz);
	if (work == NULL)
		return -1;

	return tek_decmain5(work, src, outsiz, outbuf, lc, pb, lp);
}

struct tek_STR_RNGDEC {
	UCHAR *p;
	UINT32 range, code;
};

struct tek_STR_PRB {
	struct tek_STR_PRB_PB {
		struct tek_STR_PRB_PBST {
			tek_TPRB mch, rep0l1;
		} st[12];
		tek_TPRB lenlow[2][8], lenmid[2][8];
	} pb[16];
	struct tek_STR_PRB_ST {
		tek_TPRB rep, repg0, repg1, repg2;
	} st[12];
	tek_TPRB lensel[2][2], lenhigh[2][256], pslot[4][64], algn[16];
	tek_TPRB spdis[2][2+4+8+16+32];
	tek_TPRB lit[1];
};

struct tek_STR_WORK5 {
	struct tek_STR_RNGDEC rd;
	struct tek_STR_PRB prb;
};

static int tek_rdget0(struct tek_STR_RNGDEC *rd, int n, int i)
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

static int tek_rdget1(struct tek_STR_RNGDEC *rd, UINT32 *prob0, int n, int j)
{
	UINT32 i, *prob;
	prob0 -= j;
	do {
		prob = prob0 + j;
		if (rd->range < (UINT32) (1 << 24))
			goto shift;
shift1:
		j += j;
		i = (rd->range >> 11) * *prob;
		if (rd->code < i) {
			j |= 1;
			rd->range = i;
			*prob += (0x800 - *prob) >> 5;
		} else {
			rd->range -= i;
			rd->code -= i;
			*prob -= *prob >> 5;
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

static UINT32 tek_revbit(UINT32 data, int len)
{
	UINT32 rev = 0;
	do {
		rev += rev + (data & 1);
		data >>= 1;
	} while (--len);
	return rev;
}

static int tek_getlen5(struct tek_STR_RNGDEC *rd, struct tek_STR_PRB *prb, int m, int s_pos)
{
	int i;
	if (tek_rdget1(rd, &prb->lensel[m][0], 1, 0) == 0) /* low */
		i = tek_rdget1(rd, prb->pb[s_pos].lenlow[m], 3, 1) & 7;
	else if (tek_rdget1(rd, &prb->lensel[m][1], 1, 0) == 0) /* mid */
		i = tek_rdget1(rd, prb->pb[s_pos].lenmid[m], 3, 1);
	else {
		/* high */
		i = tek_rdget1(rd, prb->lenhigh[m], 8, 1) - (256 + 256 - 8);
		if (i > 0) {
			i = tek_rdget0(rd, i, ~1) - 1;
			i = tek_rdget0(rd, i, ~1) - 1;
		}
		i += 256 - 8 + 16;
	}
	return i;
}

static int tek_decmain5(int *work, UCHAR *src, int osiz, UCHAR *q, int lc, int pb, int lp)
{
	static int state_table[] = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 4, 5 };
	int i, j, k, pmch, rep[4], s, pos, m_pos = (1 << pb) - 1, m_lp = (1 << lp) - 1;
	int lcr = 8 - lc, s_pos;
	UINT32 *lit1;
	struct tek_STR_PRB *prb = &((struct tek_STR_WORK5 *) work)->prb;
	struct tek_STR_RNGDEC *rd = &((struct tek_STR_WORK5 *) work)->rd;

	rd->p = &src[4];
	rd->range |= -1;
	rd->code = src[0] << 24 | src[1] << 16 | src[2] << 8 | src[3];
	for (i = 0; i < 4; i++)
		rep[i] = ~i;
	for (i = sizeof (struct tek_STR_PRB) / sizeof (UINT32) + (0x300 << (lc + lp)) - 2; i >= 0; i--)
		((UINT32 *) prb)[i] = 1 << 10;
	lit1 = prb->lit + ((256 << (lc + lp)) - 2);

	if (tek_rdget1(rd, &prb->pb[0].st[0].mch, 1, 0))
		goto err;
	*q++ = tek_rdget1(rd, prb->lit, 8, 1) & 0xff;
	pmch &= 0; s &= 0; pos = 1;
	do {
		s_pos = pos & m_pos;
		if (tek_rdget1(rd, &prb->pb[s_pos].st[s].mch, 1, 0) == 0) { /* 非lz */
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
			pos++;
			q++;
		} else { /* lz */
			pmch |= 1;
			if (tek_rdget1(rd, &prb->st[s].rep, 1, 0) == 0) { /* len/dis */
				rep[3] = rep[2];
				rep[2] = rep[1];
				rep[1] = rep[0];
				j = i = tek_getlen5(rd, prb, 0, s_pos);
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
						rep[0] |= tek_rdget0(rd, k - 4, ~0) << 4;
						rep[0] |= tek_revbit(tek_rdget1(rd, prb->algn, 4, 1), 4);
					}
				}
				rep[0] = ~rep[0];
			} else { /* repeat-dis */
				if (tek_rdget1(rd, &prb->st[s].repg0, 1, 0) == 0) { /* rep0 */
					i |= -1;
					if (tek_rdget1(rd, &prb->pb[s_pos].st[s].rep0l1, 1, 0) == 0) {
						s = s < 7 ? 9 : 11;
						goto skip;
					}
				} else {
					if (tek_rdget1(rd, &prb->st[s].repg1, 1, 0) == 0) /* rep1 */
						i = rep[1];
					else {
						if (tek_rdget1(rd, &prb->st[s].repg2, 1, 0) == 0) /* rep2 */
							i = rep[2];
						else {
							i = rep[3]; /* rep3 */
							rep[3] = rep[2];
						}
						rep[2] = rep[1];
					}
					rep[1] = rep[0];
					rep[0] = i;
				}
				i = tek_getlen5(rd, prb, 1, s_pos);
				s = s < 7 ? 8 : 11;
			}
skip:
			i += 2;
			if (pos + rep[0] < 0)
				goto err;
			pos += i;
			if (pos > osiz)
				goto err;
			do {
				*q = q[rep[0]];
				q++;
			} while (--i);
		}
	} while (pos < osiz);
	return 0;
err:
	return 1;
}

int64_t GetST5DecompressedSize(monapi_cmemoryinfo* mi)
{
	int64_t ret = 0;
	ret = tek_checkformat(mi->Size, (UCHAR*)mi->Data);
	return ret;
}

monapi_cmemoryinfo* ST5Decompress(monapi_cmemoryinfo* mi)
{
	int64_t size = GetST5DecompressedSize(mi);
	if (size < 0) return NULL;
	
	// if size >= 4GB abort...
	if ((size >> 32) > 0) return NULL;
	
	monapi_cmemoryinfo* ret = new monapi_cmemoryinfo();
	if (!monapi_cmemoryinfo_create(ret, (dword)(size + 1), 0))
	{
		monapi_cmemoryinfo_delete(ret);
		return NULL;
	}
	ret->Size--;
	
	if (tek_decode(mi->Size, mi->Data, ret->Data) != 0) {
		// Decompress failed
		monapi_cmemoryinfo_dispose(ret);
		monapi_cmemoryinfo_delete(ret);
		return NULL;
	}
	
	ret->Data[ret->Size] = 0;
	return ret;
}

monapi_cmemoryinfo* ST5DecompressFile(const char* file, bool prompt /*= false*/)
{
	monapi_cmemoryinfo* mi = ReadFile(file, prompt), * ret = NULL;
	if (mi == NULL) return ret;
	
	if (prompt) printf("%s: Decompressing %s....", SVR, file);
	ret = ST5Decompress(mi);
	if (prompt) printf(ret != NULL ? "OK\n" : "ERROR\n");

	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	return ret;
}
