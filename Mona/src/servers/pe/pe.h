#ifndef __PE_SERVER_PE_H__
#define __PE_SERVER_PE_H__

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef struct
{
	uint16_t Machine, NumberOfSections;
	uint32_t TimeDateStamp, PointerToSymbolTable, NumberOfSymbols;
	uint16_t OptionalHeaderSize, Characteristics;
} PEFileHeader;

typedef struct
{
	uint16_t Magic;
	uint8_t LMajor, LMinor;
	uint32_t CodeSize, InitializedDataSize, UninitializedDataSize;
	uint32_t EntryPointRVA, BaseOfCode, BaseOfData;
} PEHeaderStandardFields;

typedef struct
{
	uint32_t ImageBase, SectionAlignment, FileAlignment;
	uint16_t OSMajor, OSMinor, UserMajor, UserMinor, SubSysMajor, SubSysMinor;
	uint32_t Reserved, ImageSize, HeaderSize, FileChecksum;
	uint16_t SubSystem, DLLFlags;
	uint32_t StackReserveSize, StackCommitSize, HeapReserveSize, HeapCommitSize;
	uint32_t LoaderFlags, NumberOfDataDirectories;
} PEHeaderWindowsNTSpecificFields;

typedef struct
{
	uint64_t ExportTable, ImportTable, ResourceTable, ExceptionTable;
	uint64_t CertificateTable, BaseRelocationTable, Debug;
	uint64_t Copyright, GlobalPtr, TLSTable, LoadConfigTable, BoundImport;
	uint64_t IAT, DelayImportDescriptor, CLIHeader, Reserved;
} PEHeaderDataDirectories;

typedef struct
{
	uint64_t Name;
	uint32_t VirtualSize, VirtualAddress, SizeOfRawData;
	uint32_t PointerToRawData, PointerToRelocations, PointerToLinenumbers;
	uint16_t NumberOfRelocations, NumberOfLinenumbers;
	uint32_t Characteristics;
} SectionHeaders;

typedef struct
{
	uint32_t ImportLookupTable, DateTimeStamp, ForwarderChain, Name, ImportAddressTable;
} ImportTable;

#endif  // __PE_SERVER_PE_H__
