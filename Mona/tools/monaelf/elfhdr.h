
typedef struct {
	char	sig[4];
	UINT8	bits;
	UINT8	endian;
	UINT8	headver;
	UINT8	padding[9];
} ELFHDR;

typedef struct {
	UINT16	type;
	UINT16	arch;
	UINT32	filever;
	UINT32	entrypoint;
	UINT32	phdrpos;
	UINT32	shdrpos;
	UINT32	flag;
	UINT16	hdrsize;
	UINT16	phdrent;
	UINT16	phdrcnt;
	UINT16	shdrent;
	UINT16	shdrcnt;
	UINT16	strsec;
} ELFHDR32;

typedef struct {
	UINT32	type;
	UINT32	offset;
	UINT32	virtualaddr;
	UINT32	physaddr;
	UINT32	filesize;
	UINT32	memorysize;
	UINT32	flags;
	UINT32	align;
} ELFPRG;

typedef struct {
	UINT32	name;
	UINT32	type;
	UINT32	flags;
	UINT32	address;
	UINT32	offset;
	UINT32	size;
	UINT32	link;
	UINT32	info;
	UINT32	align;
	UINT32	entsize;
} ELFSCT;

