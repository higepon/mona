#include <monapi/string.h>
#include "PEParser.h"

PEParser::PEParser() :
	data(NULL), size(0), file(NULL), standard(NULL), specific(NULL),
	directories(NULL), sections(NULL), imageSize(0)
{
}

bool PEParser::Parse(uint8_t* data, uint32_t size)
{
	this->data = NULL;
	if (data == NULL) return false;
	
	if (size < 0x40) return false;
	uint32_t offset = *(uint32_t*)&data[0x3c];
	
	if (size < offset + 4) return false;
	uint32_t signature = *(uint32_t*)&data[offset];
	if (signature != 0x4550) return false;
	offset += 4;
	
	if (size < offset + sizeof(PEFileHeader)) return false;
	this->file = (PEFileHeader*)&data[offset];
	offset += sizeof(PEFileHeader);
	
	if (size < offset + sizeof(PEHeaderStandardFields)) return false;
	this->standard = (PEHeaderStandardFields*)&data[offset];
	offset += sizeof(PEHeaderStandardFields);
	
	if (size < offset + sizeof(PEHeaderWindowsNTSpecificFields)) return false;
	this->specific = (PEHeaderWindowsNTSpecificFields*)&data[offset];
	offset += sizeof(PEHeaderWindowsNTSpecificFields);
	
	if (size < offset + sizeof(PEHeaderDataDirectories)) return false;
	this->directories = (PEHeaderDataDirectories*)&data[offset];
	offset += sizeof(PEHeaderDataDirectories);
	
	int sectionSize = sizeof(SectionHeaders) * this->file->NumberOfSections;
	if (size < offset + sectionSize) return false;
	this->sections = (SectionHeaders*)&data[offset];
	offset += sectionSize;
	
	this->imageSize = 0;
	for (int i = 0; i < this->file->NumberOfSections; i++)
	{
		SectionHeaders* shdr = &this->sections[i];
		if (size < shdr->PointerToRawData + shdr->VirtualSize) return false;
		
		uint32_t addr = shdr->VirtualAddress + shdr->VirtualSize;
		if (this->imageSize < addr) this->imageSize = addr;
	}
	
	this->data = data;
	this->size = size;
	return true;
}

bool PEParser::Load(uint8_t* image)
{
	if (this->data == NULL) return false;
	
	memset(image, 0, this->imageSize);
	for (int i = 0; i < this->file->NumberOfSections; i++)
	{
		SectionHeaders* shdr = &this->sections[i];
		memcpy(&image[shdr->VirtualAddress], &this->data[shdr->PointerToRawData], shdr->VirtualSize);
	}
	return true;
}
