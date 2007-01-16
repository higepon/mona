//#include <monapi/syscall.h>
#include <monapi/string.h>
#include "PEParser.h"

PEParser::PEParser() :
    data(NULL), size(0), file(NULL), standard(NULL), specific(NULL),
    directories(NULL), sections(NULL), imports(NULL), importsCount(0), exp(NULL),
    imageSize(0), address(0)
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

    if (!this->ReadSections() || !this->ReadImportTables() || !this->ReadExportTable()) return false;

    return true;
}

bool PEParser::ReadSections()
{
    this->imageSize = 0;
    for (int i = 0; i < this->file->NumberOfSections; i++)
    {
        SectionHeaders* shdr = &this->sections[i];
        if (shdr->PointerToRawData == 0) continue;

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

    uint32_t paddr = this->ConvertToPhysical(addr);
    if (this->size < paddr + size) return false;

    this->imports = (ImportTable*)&this->data[paddr];
    while (this->imports[this->importsCount].ImportLookupTable != 0) this->importsCount++;

    return true;
}

bool PEParser::ReadExportTable()
{
    this->exp = NULL;
    uint32_t addr = (uint32_t)(this->directories->ExportTable & 0xffffffff);
    uint32_t size = (uint32_t)(this->directories->ExportTable >> 32);
    if (addr == 0 || size == 0) return true;

    uint32_t paddr = this->ConvertToPhysical(addr);
    if (this->size < paddr + size) return false;

    this->exp = (ExportTable*)&this->data[paddr];
    int len = (int)this->exp->AddressTableEntries;
    for (int i = 0; i < len; i++)
    {
        int idx = this->exp->OrdinalBase + i;
        uint32_t addr = this->GetExportAddress(idx);
        if (addr == 0) return false;
        //const char* name = this->GetExportName(idx);
        //printf(name == NULL ? "- %x[%d]\n" : "- %x[%d] %s\n", addr, idx, name);
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

uint32_t PEParser::GetExportAddress(int index)
{
    if (this->exp == NULL) return 0;

    index -= this->exp->OrdinalBase;
    int len = (int)this->exp->AddressTableEntries;
    if (index < 0 || len <= index) return 0;

    uint32_t addr = this->exp->ExportAddressTable + index * 4;
    uint32_t paddr = this->ConvertToPhysical(addr);
    if (this->size < paddr + 4) return 0;

    return *(uint32_t*)&this->data[paddr];
}

const char* PEParser::GetExportName(int index)
{
    if (this->exp == NULL) return NULL;

    index -= this->exp->OrdinalBase;
    uint32_t addr = this->exp->OrdinalTable;
    uint32_t paddr = this->ConvertToPhysical(addr);
    int len = this->exp->NumberOfNamePointers;
    if (this->size < paddr + len * 2) return NULL;

    for (int i = 0; i < len; i++)
    {
        int v = *(uint16_t*)&this->data[paddr + i * 2];
        if (v == index)
        {
            uint32_t name_addr = this->exp->NamePointer;
            uint32_t name_paddr = this->ConvertToPhysical(name_addr);
            name_paddr += i * 4;
            if (this->size < name_paddr + 4) return NULL;

            uint32_t ret_addr = *(uint32_t*)&this->data[name_paddr];
            uint32_t ret_paddr = this->ConvertToPhysical(ret_addr);
            if (this->size < ret_paddr) return NULL;

            return (const char*)&this->data[ret_paddr];
        }
    }
    return NULL;
}

int PEParser::GetExportOrdinal(const char* name)
{
    if (this->exp == NULL) return -1;

    uint32_t addr = this->exp->NamePointer;
    uint32_t paddr = this->ConvertToPhysical(addr);
    uint32_t paddr_end = paddr + this->exp->NumberOfNamePointers * 4;
    if (this->size < paddr_end) return -1;

    int num = -1;
    for (int i = 0; paddr < paddr_end; paddr += 4, i++)
    {
        uint32_t name_addr = *(uint32_t*)&this->data[paddr];
        uint32_t name_paddr = this->ConvertToPhysical(name_addr);
        if (this->size < name_paddr) return -1;

        const char* n = (const char*)&this->data[name_paddr];
        if (strcmp(n, name) == 0)
        {
            num = i;
            break;
        }
    }
    if (num == -1) return -1;

    uint32_t ordinal_addr = this->exp->OrdinalTable + num * 2;
    uint32_t ordinal_paddr = this->ConvertToPhysical(ordinal_addr);
    if (this->size < ordinal_paddr + 2) return -1;

    return *(uint16_t*)&this->data[ordinal_paddr] + this->exp->OrdinalBase;
}

bool PEParser::Load(uint8_t* image)
{
    if (this->data == NULL) return false;

    this->address = 0;
    memset(image, 0, this->imageSize);
    for (int i = 0; i < this->file->NumberOfSections; i++)
    {
        SectionHeaders* shdr = &this->sections[i];
        if (shdr->PointerToRawData == 0) continue;

        memcpy(&image[shdr->VirtualAddress], &this->data[shdr->PointerToRawData], shdr->VirtualSize);
    }
    return true;
}

#include <monalibc.h>
bool PEParser::Relocate(uint8_t* image, uint32_t address)
{
    if (image == NULL) return false;
    this->address = address;
    uint32_t addr = (uint32_t)(this->directories->BaseRelocationTable & 0xffffffff);
    uint32_t size = (uint32_t)(this->directories->BaseRelocationTable >> 32);
    if (addr == 0 || size == 0) return true;

    uint32_t addr_end = addr + size;

    if (this->imageSize < addr_end) return false;

    while (addr < addr_end)
    {
        uint32_t page_addr = *(uint32_t*)&image[addr];
        uint32_t page_size = *(uint32_t*)&image[addr + 4];

        uint32_t addr_page_end = addr + page_size;
        addr += 8;
        if (this->imageSize < addr_page_end) return false;

        //int fixups = (page_size - 8) / 2;
        //printf("Virtual Address: %x Chunk size %d Number of fixups %d\n", page_addr, page_size, fixups);
        while (addr < addr_page_end)
        {
            uint16_t fixup = *(uint16_t*)&image[addr];
            addr += 2;
            int type = (int)(fixup >> 12);
            uint32_t offset = fixup & 0x0fff;
            //printf("reloc offset %x [%x] %d\n", offset, page_addr + offset, type);
            if (type == 3)  // HIGHLOW
            {
                uint32_t* target = (uint32_t*)&image[page_addr + offset];
                *target -= this->specific->ImageBase;
                *target += address;
            }
            else if (type != 0)  // not ABSOLUTE
            {
                return false;
            }
        }
    }
    return true;
}

#define DEBUG_LINK 0

bool PEParser::Link(uint8_t* image, int index, PEParser* parser)
{
// <<<<<<< .working
// 	if (parser == NULL) return false;
	
// 	ImportTable* it = this->GetImportTable(index);
// 	if (it == NULL) return false;
	
// 	uint32_t addr1 = it->ImportLookupTable;
// 	uint32_t addr2 = it->ImportAddressTable;
// 	if (addr1 == 0 || addr2 == 0) return false;
	
// 	for (;; addr1 += 4, addr2 += 4)
// 	{
// 		if (addr1 >= this->imageSize || addr2 >= this->imageSize) return false;
		
// 		uint32_t* ptr1 = (uint32_t*)&image[addr1];
// 		uint32_t* ptr2 = (uint32_t*)&image[addr2];
// 		if (*ptr1 == 0 || *ptr2 == 0) break;
		
// 		int ordinal = -1;
// 		uint32_t* ptr3 = (uint32_t*)&image[*ptr1];
// 		if ((*ptr3 & 0x80000000) != 0)
// 		{
// 			// Ordinal Number
// 			ordinal = (int)((*ptr3) & 0x7fffffff);
// 		}
// 		else
// 		{
// 			// Hint/Name Table RVA
// 			int hint = (*(uint16_t*)ptr3) + 1;
// 			const char* name = (const char*)&image[*ptr1 + 2];
// 			if (!strcmp(name, parser->GetExportName(hint)))
// 			{
// 				ordinal = hint;
// 				//printf("* [%d]%s\n", ordinal, name);
// 			}
// 			else
// 			{
// 				ordinal = parser->GetExportOrdinal(name);
// 				//printf("* [%d->%d]%s\n", hint, ordinal, name);
// 			}
// 		}
// 		if (ordinal == -1) return false;
		
// 		uint32_t exp_addr = parser->GetExportAddress(ordinal);
// 		if (exp_addr == 0) return false;
		
// 		*ptr2 = exp_addr + parser->address;
// 		//printf("  address = %x\n", *ptr2);
// 	}
	
// 	return true;
// =======
#if DEBUG_LINK
    _printf("Link start table=%d\n", index);
#endif
    if (parser == NULL) return false;
    ImportTable* it = this->GetImportTable(index);

#if DEBUG_LINK
    _printf("ImportLookupTable =%x\n DateTimeStamp=%x\nForwarderChain=%x\nName=%x\nImportAddressTable=%x\n",
            it->ImportLookupTable,
            it->DateTimeStamp,
            it->ForwarderChain,
            it->Name,
            it->ImportAddressTable);
#endif

    if (it == NULL) return false;
    uint32_t addr        = it->ImportAddressTable;
    uint32_t lookupAddrr = it->ImportLookupTable;

    if (addr == 0) return false;

#if DEBUG_LINK
    uint32_t* p = (uint32_t*)&image[lookupAddrr];
    for (int i = 0; i < 128; i++)
    {
        _printf("%c", *((char*)(&image[*p + 2 ] + i)));
    }
#endif

    for (; addr < this->imageSize; addr += 4, lookupAddrr += 4)
    {
        uint32_t* lookupPtr = (uint32_t*)&image[lookupAddrr];
        uint32_t* ptr       = (uint32_t*)&image[addr];

        if (*lookupPtr == 0) break;

        uint32_t fixup = 0;
        std::map<uint32_t, uint32_t>::iterator it = this->fixups.find(*lookupPtr);
        if (it == this->fixups.end())
            this->fixups.insert(std::pair<uint32_t, uint32_t>(*lookupPtr, addr));
        else
            fixup = *ptr - this->specific->ImageBase - it->second;

        int ordinal = -1;
        const char* name = (const char*)&image[(*lookupPtr) + 2];
        if ((*lookupPtr & 0x80000000) != 0)
        {
            // Ordinal Number
            ordinal = (int)(*lookupPtr & 0x7fffffff);
        }
        else
        {
            // Hint/Name Table RVA
            ordinal = parser->GetExportOrdinal(name);
        }
        if (ordinal == -1)
        {
            _printf("link error %s\n", name);
            return false;
        }
        uint32_t exp_addr = parser->GetExportAddress(ordinal);
        const char* exp_name = parser->GetExportName(ordinal);
        if (name != NULL && (exp_name == NULL || strcmp(name, exp_name) != 0)) return false;
        if (exp_addr == 0) return false;
        *ptr = exp_addr + parser->address + fixup;
    }
    return true;
//>>>>>>> .merge-right.r3945
}
