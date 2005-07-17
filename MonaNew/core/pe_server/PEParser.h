#ifndef __PE_SERVER_PEPARSER_H__
#define __PE_SERVER_PEPARSER_H__

#include <sys/HList.h>
#include "pe.h"

class PEParser
{
private:
	uint8_t* data;
	uint32_t size;
	PEFileHeader* file;
	PEHeaderStandardFields* standard;
	PEHeaderWindowsNTSpecificFields* specific;
	PEHeaderDataDirectories* directories;
	SectionHeaders* sections;
	ImportTable* imports;
	int importsCount;
	ExportTable* exp;
	uint32_t imageSize;
	uint32_t address;
	HList<uint32_t> addrs_p;
	HList<uint32_t> addrs_v;

public:
	PEParser();
	bool Parse(uint8_t* data, uint32_t size);
	bool Load(uint8_t* image);
	bool Relocate(uint8_t* image, uint32_t address);
	bool Link(uint8_t* image, int index, PEParser* parser);

	uint32_t ConvertToPhysical(uint32_t virt);
	const char* GetImportTableName(int index);
	uint32_t GetExportAddress(int index);
	const char* GetExportName(int index);
	int GetExportOrdinal(const char* name);

	inline uint32_t get_ImageSize() { return this->imageSize; }
	inline uint32_t get_EntryPoint() { return this->data == NULL ? 0 : this->specific->ImageBase + this->standard->EntryPointRVA; }
	inline int get_ImportTableCount() { return this->importsCount; }
	inline ImportTable* GetImportTable(int index) { return index < this->importsCount ? &this->imports[index] : NULL; }

private:
	bool ReadSections();
	bool ReadImportTables();
	bool ReadExportTable();
};

#endif  // __PE_SERVER_PEPARSER_H__
