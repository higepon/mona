#ifndef __PE_SERVER_PEPARSER_H__
#define __PE_SERVER_PEPARSER_H__

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
	uint32_t imageSize;

public:
	PEParser();
	bool Parse(uint8_t* data, uint32_t size);
	inline uint32_t get_ImageSize() { return this->imageSize; }
	inline uint32_t get_EntryPoint() { return this->data == NULL ? 0 : this->specific->ImageBase + this->standard->EntryPointRVA; }
	bool Load(uint8_t* image);
};

#endif  // __PE_SERVER_PEPARSER_H__
