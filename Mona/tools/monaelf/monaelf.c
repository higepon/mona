#include	"compiler.h"
#include	"elfhdr.h"
#include	"monaelf.h"

static const char elftmp[] = "monaelf.tmp";


static int filedatafill(FILE *d, int c, UINT size) {

	UINT	r;
	UINT8	work[512];

	memset(work, c, sizeof(work));
	while(size) {
		r = (size < sizeof(work))?size:sizeof(work);
		if (fwrite(work, 1, r, d) != r) {
			return(-1);
		}
		size -= r;
	}
	return(0);
}

static int filedatacopy(FILE *d, FILE *s, UINT size) {

	UINT	r;
	UINT8	work[512];

	while(size) {
		r = (size < sizeof(work))?size:sizeof(work);
		if ((fread(work, 1, r, s) != r) || (fwrite(work, 1, r, d) != r)) {
			return(-1);
		}
		size -= r;
	}
	return(0);
}

static int filecopy(const char *srcname, const char *dstname) {

	FILE	*fp1;
	FILE	*fp2;
struct stat	sb;

	fp1 = fopen(srcname, "rb");
	if (fp1 == NULL) {
		goto fc_err1;
	}
	if (fstat(fileno(fp1), &sb) != 0) {
		goto fc_err2;
	}
	fp2 = fopen(dstname, "wb");
	if (fp2 == NULL) {
		goto fc_err2;
	}
	if (filedatacopy(fp2, fp1, sb.st_size) != 0) {
		goto fc_err3;
	}
	fclose(fp2);
	fclose(fp1);
	return(0);

fc_err3:
	fclose(fp2);

fc_err2:
	fclose(fp1);

fc_err1:
	return(0);
}


// ----

typedef struct {
	ELFHDR		hdr;
	ELFHDR32	hdr32;
	UINT		psize;
	UINT		ssize;
	ELFPRG		*prg;
	ELFSCT		*sct;
} _ELF, *ELF;

static ELF elfcre32(const ELFHDR *hdr, const ELFHDR32 *hdr32) {

	UINT	psize;
	UINT	ssize;
	UINT	size;
	ELF		ret;

	if ((hdr == NULL) || (hdr32 == NULL)) {
		goto ec32_err;
	}
	psize = hdr32->phdrent * hdr32->phdrcnt;
	ssize = hdr32->shdrent * hdr32->shdrcnt;
	size = sizeof(_ELF) + psize + ssize;
	ret = (ELF)malloc(size);
	if (ret == NULL) {
		printf("ELF32 header: memory alloc error\n");
		goto ec32_err;
	}
	memset(ret, 0, size);
	ret->hdr = *hdr;
	ret->hdr32 = *hdr32;
	ret->psize = psize;
	ret->ssize = ssize;
	ret->prg = (ELFPRG *)(ret + 1);
	ret->sct = (ELFSCT *)(((UINT8 *)ret->prg) + psize);
	return(ret);

ec32_err:
	return(NULL);
}

static ELF loadelfhdr(FILE *fp) {

	ELFHDR		hdr;
	ELFHDR32	hdr32;
	ELF			ret;

	if (fread(&hdr, sizeof(hdr), 1, fp) != 1) {
		printf("ELF header: read error\n");
		goto leh_err1;
	}
	if ((hdr.sig[0] != 0x7f) || (hdr.sig[1] != 'E') ||
		(hdr.sig[2] != 'L') || (hdr.sig[3] != 'F')) {
		printf("ELF header: isn't elf format\n");
		goto leh_err1;
	}
	if (hdr.bits != 1) {
		printf("ELF header: not 32bits\n");
		goto leh_err1;
	}
	if (hdr.endian != 1) {
		printf("ELF header: not little endian\n");
		goto leh_err1;
	}

	if (fread(&hdr32, sizeof(hdr32), 1, fp) != 1) {
		printf("ELF32 header: read error\n");
		goto leh_err1;
	}
	if (hdr32.type != 2) {
		printf("ELF32 header: not type executable\n");
		goto leh_err1;
	}
	if (hdr32.arch != 3) {
		printf("ELF32 header: not arch x86\n");
		goto leh_err1;
	}
	if (hdr32.hdrsize < (sizeof(ELFHDR) + sizeof(ELFHDR32))) {
		printf("ELF32 header: wrong header size\n");
		goto leh_err1;
	}
	if (hdr32.phdrent != sizeof(ELFPRG)) {
		printf("ELF32 header: wrong program header size\n");
		goto leh_err1;
	}
	if (hdr32.shdrent != sizeof(ELFSCT)) {
		printf("ELF32 header: wrong program section size\n");
		goto leh_err1;
	}
	ret = elfcre32(&hdr, &hdr32);
	if (ret == NULL) {
		goto leh_err1;
	}
	if (fseek(fp, hdr32.phdrpos, SEEK_SET) != 0) {
		printf("ELF32 program header: seek error\n");
		goto leh_err2;
	}
	if (fread(ret->prg, hdr32.phdrent, hdr32.phdrcnt, fp) != hdr32.phdrcnt) {
		printf("ELF32 program header: read error\n");
		goto leh_err2;
	}
	if (fseek(fp, hdr32.shdrpos, SEEK_SET) != 0) {
		printf("ELF32 section header: seek error\n");
		goto leh_err2;
	}
	if (fread(ret->sct, hdr32.shdrent, hdr32.shdrcnt, fp) != hdr32.shdrcnt) {
		printf("ELF32 section header: read error\n");
		goto leh_err2;
	}
	return(ret);

leh_err2:
	free(ret);

leh_err1:
	return(NULL);
}

static ELF elfcollect(ELF elf) {

	UINT		phdrcnt;
	UINT		shdrcnt;
	ELFPRG		*prg1;
	ELFSCT		*sct1;
	UINT		phdr;
	UINT		hit;
	ELFHDR32	hdr32;
	ELFPRG		*prg2;
//	ELFSCT		*sct2;
	ELF			ret;
	UINT32		offset;

	if (elf == NULL) {
		goto ecl_err;
	}
	phdr = 0;
	phdrcnt = elf->hdr32.phdrcnt;
	prg1 = elf->prg;
	while(phdrcnt) {
		if (prg1->filesize) {
			shdrcnt = elf->hdr32.shdrcnt;
			sct1 = elf->sct;
			hit = 0;
			while(shdrcnt) {
				if ((sct1->size) && (sct1->offset >= prg1->offset) &&
					((sct1->offset + sct1->size) <=
										(prg1->offset + prg1->filesize))) {
					hit++;
				}
				shdrcnt--;
				sct1++;
			}
			if (hit == 0) {
				printf("elf collect: not found section\n");
				goto ecl_err;
			}
			phdr += hit;
		}
		phdrcnt--;
		prg1++;
	}
	hdr32.phdrcnt = phdr;
	ret = elfcre32(&elf->hdr, &hdr32);
	if (ret == NULL) {
		goto ecl_err;
	}
	memcpy(ret->sct, elf->sct, hdr32.shdrent * hdr32.shdrcnt);

	phdrcnt = elf->hdr32.phdrcnt;
	prg1 = elf->prg;
	prg2 = ret->prg;
	while(phdrcnt) {
		if (prg1->filesize) {
			shdrcnt = elf->hdr32.shdrcnt;
			sct1 = elf->sct;
			while(shdrcnt) {
				if ((sct1->size) && (sct1->offset >= prg1->offset) &&
					((sct1->offset + sct1->size) <=
										(prg1->offset + prg1->filesize))) {
					*prg2 = *prg1;
					offset = sct1->offset - prg1->offset;
					prg2->offset += offset;
					prg2->filesize = sct1->size;
					prg2->virtualaddr += offset;
					prg2->physaddr += offset;
					prg2++;
				}
				shdrcnt--;
				sct1++;
			}
			phdr += hit;
		}
		phdrcnt--;
		prg1++;
	}
	return(ret);

ecl_err:
	return(NULL);
}

static int elfrelocate(FILE *fp, ELF elf, const char *dstname) {

	ELF		tmp;
	FILE	*fdp;
	UINT	fpos;
	UINT	shdrcnt;
	ELFSCT	*sct1;
	ELFSCT	*sct2;
	UINT	phdrcnt;
	ELFPRG	*prg1;
	ELFPRG	*prg2;
	UINT	pad;

	if (elf == NULL) {
		goto erl_err1;
	}
	tmp = elfcre32(&elf->hdr, &elf->hdr32);
	if (tmp == NULL) {
		goto erl_err1;
	}
	tmp->hdr32.hdrsize = sizeof(ELFHDR) + sizeof(ELFHDR32);
	tmp->hdr32.phdrpos = 0;
	tmp->hdr32.phdrcnt = 0;
	tmp->hdr32.shdrpos = 0;
	tmp->hdr32.shdrcnt = 0;

	// ƒtƒ@ƒCƒ‹`
	fdp = fopen(dstname, "wb");
	if (fdp == NULL) {
		printf("ELF relocate: output file couldn't create\n");
		goto erl_err2;
	}

	if (fwrite(&tmp->hdr, sizeof(tmp->hdr), 1, fdp) != 1) {
		printf("ELF header: write error\n");
		goto erl_err3;
	}
	if (fwrite(&tmp->hdr32, sizeof(tmp->hdr32), 1, fdp) != 1) {
		printf("ELF32 header: write error\n");
		goto erl_err3;
	}
	fpos = sizeof(tmp->hdr) + sizeof(tmp->hdr32);
	tmp->hdr32.phdrpos = fpos;
	tmp->hdr32.phdrcnt = elf->hdr32.phdrcnt;
	if (fwrite(tmp->prg, elf->hdr32.phdrent, elf->hdr32.phdrcnt, fdp)
													!= elf->hdr32.phdrcnt) {
		printf("ELF32 program header: write error\n");
		goto erl_err3;
	}
	fpos += elf->hdr32.phdrent * elf->hdr32.phdrcnt;
	tmp->hdr32.shdrpos = fpos;
	tmp->hdr32.shdrcnt = elf->hdr32.shdrcnt;
	if (fwrite(tmp->sct, elf->hdr32.shdrent, elf->hdr32.shdrcnt, fdp)
													!= elf->hdr32.shdrcnt) {
		printf("ELF32 section header: write error\n");
		goto erl_err3;
	}
	fpos += elf->hdr32.shdrent * elf->hdr32.shdrcnt;

	pad = (0 - fpos) & 15;
	if (filedatafill(fdp, 0, pad) != 0) {
		printf("ELF relocate: pad write error\n");
		goto erl_err3;
	}
	fpos += pad;

	sct1 = elf->sct;
	sct2 = tmp->sct;
	shdrcnt = elf->hdr32.shdrcnt;
	while(shdrcnt) {
		*sct2 = *sct1;
		if (sct1->type == 0) {
		}
		else if ((sct1->type == 1) || (sct1->type == 3)) {
			sct2->offset = fpos;
			if (fseek(fp, sct1->offset, SEEK_SET) != 0) {
				printf("ELF relocate: seek error\n");
				goto erl_err3;
			}
			if (filedatacopy(fdp, fp, sct1->size) != 0) {
				printf("ELF relocate: file error\n");
				goto erl_err3;
			}
			fpos += sct1->size;

			pad = (0 - fpos) & 15;
			if (filedatafill(fdp, 0, pad) != 0) {
				printf("ELF relocate: pad write error\n");
				goto erl_err3;
			}
			fpos += pad;
		}
		else if (sct1->type == 8) {
			sct2->offset = fpos;
		}
		else {
			printf("ELF relocate: unknown type\n");
			sct2->offset = fpos;
			sct2->size = 0;
		}
		sct1++;
		sct2++;
		shdrcnt--;
	}

	prg1 = elf->prg;
	prg2 = tmp->prg;
	phdrcnt = elf->hdr32.phdrcnt;
	while(phdrcnt) {
		*prg2 = *prg1;
		sct1 = elf->sct;
		sct2 = tmp->sct;
		shdrcnt = elf->hdr32.shdrcnt;
		while(shdrcnt) {
			if ((prg1->offset == sct1->offset) &&
				(prg1->filesize == sct1->size)) {
				prg2->offset = sct2->offset;
				prg2->filesize = sct2->size;
			}
			sct1++;
			sct2++;
			shdrcnt--;
		}
		prg1++;
		prg2++;
		phdrcnt--;
	}

	if (fseek(fdp, 0, SEEK_SET) != 0) {
		printf("ELF header: seek error\n");
		goto erl_err3;
	}
	if (fwrite(&tmp->hdr, sizeof(tmp->hdr), 1, fdp) != 1) {
		printf("ELF header: write error\n");
		goto erl_err3;
	}
	if (fwrite(&tmp->hdr32, sizeof(tmp->hdr32), 1, fdp) != 1) {
		printf("ELF32 header: write error\n");
		goto erl_err3;
	}
	if (fwrite(tmp->prg, elf->hdr32.phdrent, elf->hdr32.phdrcnt, fdp)
													!= elf->hdr32.phdrcnt) {
		printf("ELF32 program header: write error\n");
		goto erl_err3;
	}
	if (fwrite(tmp->sct, elf->hdr32.shdrent, elf->hdr32.shdrcnt, fdp)
													!= elf->hdr32.shdrcnt) {
		printf("ELF32 section header: write error\n");
		goto erl_err3;
	}

	fclose(fdp);
	free(tmp);
	return(0);

erl_err3:
	fclose(fdp);
	remove(dstname);

erl_err2:
	free(tmp);

erl_err1:
	return(-1);
}

int monaelf(const char *srcname, const char *dstname) {

	FILE	*fp;
	ELF		orgelf;
	ELF		elf;

	if (dstname == NULL) {
		dstname = elftmp;
	}
	fp = fopen(srcname, "rb");
	if (fp == NULL) {
		printf("input file couldn't open\n");
		goto me_err1;
	}
	orgelf = loadelfhdr(fp);
	if (orgelf == NULL) {
		goto me_err2;
	}
	elf = elfcollect(orgelf);
	free(orgelf);
	if (elf == NULL) {
		goto me_err2;
	}
	if (elfrelocate(fp, elf, dstname) != 0) {
		goto me_err3;
	}
	free(elf);
	fclose(fp);

	if (dstname == elftmp) {
		filecopy(elftmp, srcname);
		remove(elftmp);
	}
	return(0);

me_err3:
	free(elf);

me_err2:
	fclose(fp);

me_err1:
	return(-1);
}

