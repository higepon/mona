#include "types.h"
#include "DiskDriver.h"

DiskDriver::DiskDriver(const char* file, long size) {

    file_.open(file, std::ios::in|std::ios::binary);

}

DiskDriver::~DiskDriver() {

    file_.close();

}

bool DiskDriver::read(int lba, char* buf) {

    file_.seekg(0);
    file_.seekg((lba + 1) * 512);
    file_.read(buf, 512);

    return true;
}

bool DiskDriver::write(int lba, char* buf) {

    return true;
}
