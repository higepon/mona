//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

static unsigned char *bim2bin_putb_overbuf;
static int bim2bin_putb_ptr;
static unsigned char bim2bin_putb_byte, bim2bin_putb_count = 8;

const int getbc(int bits)
{
	int ret = 0;
	do {
		if (bim2bin_putb_count == 8) {
			if (--bim2bin_putb_ptr < 0)
				return -1;
			bim2bin_putb_byte = *bim2bin_putb_overbuf++;
		}
		if (--bim2bin_putb_count == 0)
			bim2bin_putb_count = 8;
		ret <<= 1;
		if (bim2bin_putb_byte & 0x80)
			ret |= 0x01;
		bim2bin_putb_byte <<= 1;
	} while (--bits);
	return ret;
}

const int getbc0(int bits, int ret)
/* 初期値付き */
{
	do {
		if (bim2bin_putb_count == 8) {
			if (--bim2bin_putb_ptr < 0)
				return -1;
			bim2bin_putb_byte = *bim2bin_putb_overbuf++;
		}
		if (--bim2bin_putb_count == 0)
			bim2bin_putb_count = 8;
		ret <<= 1;
		if (bim2bin_putb_byte & 0x80)
			ret |= 0x01;
		bim2bin_putb_byte <<= 1;
	} while (--bits);
	return ret;
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

const int getnum_l0a(int z)
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

const int lzrestore_l2d3(unsigned char *buf, int k, int i, int outlimit)
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

const int lzrestore_tek0(unsigned char *buf, int k, int i, int outlimit)
{
	int len, distance, j, z0, z1;
	unsigned int dis_s, l_ofs, method;

	/* ヘッダ読み込み */
	dis_s = getnum_s8();
	l_ofs = getbc(2);
	method = getbc(1); /* l1a/l1b */
	z0 = getbc(2);
	z1 = getbc(2);

	//printf("method:l1%c(+%d) dis_s = %08x z0 = %d z1 = %d\n", 'a' + method, l_ofs, dis_s, z0, z1);

	i = 0;
	for (;;) {
		/* "0"-phase (非圧縮フェーズ) */
		j = getnum_l0a(z0);
		if (j < 0)
			break;
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
		do {
			distance = getnum_df(dis_s);
			if (method == 0)
				len = getnum_l1a();
			else
				len = getnum_l1b();
			if (len < 0)
				break;
			len += l_ofs;
			do {
				buf[i] = buf[i + distance];
				i++;
			} while (--len);
		} while (--j);
	}
	return i;
}

#if (defined(MONA) | defined(MONACUI))
int restore(unsigned char *buf, dword *filesize)
#else
int restore(unsigned char *buf, int *filesize)
#endif
{
	int compress = 2;
#if (defined(MONA) | defined(MONACUI))
	dword i;
#else
	int i;
#endif
	unsigned char *overbuf, c;
	static unsigned char signature[16] = "\xff\xff\xff\x01\x00\x00\x00OSASKCMP";

	// 解凍前のサイズの最大5倍になっても大丈夫なように定義
	overbuf = (unsigned char *)xmalloc(*filesize * 5);
	c = 0;
	for (i = 0; i < 15; i++)
		c |= buf[i + 1] ^ signature[i];
	if (c) {
		for (i = 0; i < *filesize; i++)
			overbuf[i] = buf[i];
	} else {
		*filesize -= 20;
		if (buf[0] == 0x82)
			compress = 2;
		if (buf[0] == 0x81)
			compress = 1;
		for (i = 0; i < *filesize; i++)
			overbuf[i] = buf[i + 20];
	}
	bim2bin_putb_overbuf = overbuf;
	bim2bin_putb_ptr = *filesize;
	if (compress == 1)
		*filesize = lzrestore_l2d3(buf, *filesize, 0, *filesize * 2 - 8);
	if (compress == 2)
		*filesize = lzrestore_tek0(buf, *filesize, 0, *filesize * 2 - 8);
	xfree(overbuf);

	return 0;
}

/*int main(int argc, char **argv)
{
	FILE *fp;
	int filesize = 0, i;
	unsigned char *buf, *overbuf;

	if (argc <= 2) {
		return 1;
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		printf("error: cannot open %s\n", argv[1]);
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp) + 1;
	if (filesize == 0) {
		printf("error: this filesize is 0\n");
		return 1;
	}
	printf("open %s (%d byte)\n", argv[1], filesize);
	rewind(fp);
	buf = (unsigned char *)malloc(filesize * 2);
	overbuf = (unsigned char *)malloc(filesize * 2);
	fread((unsigned char *)buf, filesize, 1, fp);
	fclose(fp);

	if (restore(buf, &filesize) == 0) {
		fp = fopen("output.bin", "wb");
		if (fp == NULL) {
			printf("error: cannot open %s\n", argv[1]);
			return 1;
		}
		fwrite(buf, 1, filesize, fp);
		fclose(fp);
		printf("wrote %s (%d byte)\n", "output.bin", filesize);
	}
	
	return 0;
}*/
