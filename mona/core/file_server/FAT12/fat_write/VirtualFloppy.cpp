#include "VirtualFloppy.h"

VirtualFloppy::VirtualFloppy(FILE* fp)
{
    this->fp = fp;
}

VirtualFloppy::~VirtualFloppy()
{
}

int VirtualFloppy::open()
{
    return 0;
}

int VirtualFloppy::close()
{
    return 0;
}

int VirtualFloppy::read (uint32_t lba, void* buf, int size)
{
        fseek(this->fp, lba * 512, SEEK_SET);

        int result = fread(buf, size, 1, fp);
        return result;
}

int VirtualFloppy::write(uint32_t lba, void* buf, int size)
{
        fseek(fp, lba * 512, SEEK_SET);

        int result = fwrite(buf, size, 1, fp);
        return result;
}

int VirtualFloppy::ioctl(void* p)
{
    return 0;
}
