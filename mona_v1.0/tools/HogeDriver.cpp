#include "types.h"
#include "HogeDriver.h"

HogeDriver::HogeDriver(const char* file, long size) {

    file_.open(file, std::ios::in|std::ios::binary);

}

HogeDriver::~HogeDriver() {

    file_.close();

}

bool HogeDriver::read(int lba, byte* buf) {

    file_.seekg(0);
    file_.seekg((lba) * 512);
    file_.read((char*)buf, 512);

    return true;
}

bool HogeDriver::write(int lba, byte* buf) {

    return true;
}
