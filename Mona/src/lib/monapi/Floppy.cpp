/*----------------------------------------------------------------------
    Floppy
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Floppy::Floppy(int device) {
}

Floppy::~Floppy() {
}

int Floppy::open() {
    return syscall_fdc_open();
}

int Floppy::close() {
    return syscall_fdc_close();
}

int Floppy::read(dword lba,  byte* buf, dword blocknum) {;
    return syscall_fdc_read(lba, buf, blocknum);
}

int Floppy::write(dword lba, byte* buf, dword blocknum) {
    return syscall_fdc_write(lba, buf, blocknum);
}

bool Floppy::diskChanged() {
    return syscall_fdc_disk_changed();
}

int Floppy::ioctl(void* p) {

    /* not supported */
    return 0;
}

}
