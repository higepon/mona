#include "types.h"
#include "HogeDriver.h"
#include <stdio.h>

HogeDriver::HogeDriver(const char* file, long size) {

    file_.open(file, std::ios::out|std::ios::in|std::ios::binary);

}

HogeDriver::~HogeDriver() {

    file_.close();

}

bool HogeDriver::read(dword lba, byte* buf) {

    file_.seekg(0);
    file_.seekg((lba - 1) * 512);
    file_.read((char*)buf, 512);

    return true;
}

bool HogeDriver::write(dword lba, byte* buf) {

    file_.seekg(0);
    file_.seekg((lba - 1) * 512);
    file_.write((char*)buf, 512);
    return true;
}
