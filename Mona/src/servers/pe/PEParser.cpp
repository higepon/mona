#include <monapi/string.h>
#include "PEParser.h"

PEParser::PEParser() :
	data(NULL), size(0), file(NULL), standard(NULL), specific(NULL),
	directories(NULL), sections(NULL), imports(NULL), importsCount(0), imageSize(0)
{
}

bool PEParser::Parse(uint8_t* data, uint32_t size)
{
	while (this->addrs_p.size() > 0) this->addrs_p.removeAt(0);
	while (this->addrs_v.size() > 0) this->addrs_v.removeAt(0);
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
	
	this->data = data;
	this->size = size;
	
	if (!this->ReadSections()) return false;
	if (!this->ReadImportTables()) return false;
	
	return true;
}

bool PEParser::ReadSections()
{
	this->imageSize = 0;
	for (int i = 0; i < this->file->NumberOfSections; i++)
	{
		SectionHeaders* shdr = &this->sections[i];
		if (this->size < shdr->PointerToRawData + shdr->VirtualSize) return false;
		
		uint32_t addr = shdr->VirtualAddress + shdr->SizeOfRawData;
		if (this->imageSize < addr) this->imageSize = addr;
		
		this->addrs_p.add(shdr->PointerToRawData);
		this->addrs_v.add(shdr->VirtualAddress);
	}
	return true;
}

bool PEParser::ReadImportTables()
{
	this->imports = NULL;
	this->importsCount = 0;
	uint32_t addr = (uint32_t)(this->directories->ImportTable & 0xffffffff);
	uint32_t size = (uint32_t)(this->directories->ImportTable >> 32);
	if (addr == 0 || size == 0) return true;
	
	int paddr = this->ConvertToPhysical(addr);
	if (this->size < paddr + size) return false;
	
	this->imports = (ImportTable*)&this->data[paddr];
	while (this->imports[this->importsCount].ImportLookupTable != 0) this->importsCount++;
	
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

uint32_t PEParser::ConvertToPhysical(uint32_t virt)
{
	int p = -1;
	uint32_t max = 0;
	int len = this->addrs_v.size();
	for (int i = 0; i < len; i++)
	{
		uint32_t v = this->addrs_v.get(i);
		if (max < v && v <= virt)
		{
			p = i;
			max = v;
		}
	}
	return (p >= 0) ? virt - max + this->addrs_p.get(p) : virt;
}

const char* PEParser::GetImportTableName(int index)
{
	ImportTable* it = this->GetImportTable(index);
	if (it == NULL) return NULL;
	
	uint32_t paddr = this->ConvertToPhysical(it->Name);
	if (this->size < paddr) return NULL;
	
	return (const char*)&this->data[paddr];
}
