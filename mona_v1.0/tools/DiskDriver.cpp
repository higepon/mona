#include "DiskDriver.h"

DiskDriver::DiskDriver(const char* file, long size) {

    file_.open(file, std::ios::in|std::ios::binary);

}

DiskDriver::~DiskDriver() {

    file_.close();

}

bool DiskDriver::read(char* buf, int sector) {

    file_.seekg(0);
    file_.seekg(sector * 512);
    file_.read(buf, 512);

    return true;
}

bool DiskDriver::write(char* buf, int sector) {

    return true;
}
