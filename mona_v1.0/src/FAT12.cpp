/*!
    \file  FAT12.cpp
    \brief class FAT12 File System

    Copyright (c) 2003 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2003/04/10 update:$Date$
*/

#include<FAT12.h>
#include<string.h>

/*!
    \brief initilize

    \author HigePon
    \date   create:2003/04/10 update:
*/
FAT12::FAT12(DiskDriver* driver) {

    driver_ = driver;
    return;
}

/*!
    \brief destroy

    \author HigePon
    \date   create:2003/04/10 update:
*/
FAT12::~FAT12() {


    return;
}

bool FAT12::initilize() {

    if (!setBPB()) return false;

    return true;
}

bool FAT12::setBPB() {

    byte* p = buf_;

    if (!(driver_->read(0, buf_))) return false;

    p += 3;
    memcpy(bpb_.oemName, p, 8);
    p += 8;
    memcpy(&(bpb_.sizeOfSector), p, 2);
    p += 2;
    bpb_.sectorPerCluster = *p;
    p += 1;
    memcpy(&(bpb_.reservedSector), p, 2);
    p += 2;
    bpb_.NumberOfFat = *p;
    p += 1;
    memcpy(&(bpb_.rootEntries), p, 2);
    p += 2;
    memcpy(&(bpb_.totalSector), p, 2);
    p += 2;
    bpb_.media = *p;
    p += 1;
    memcpy(&(bpb_.fatSize), p, 2);
    p += 2;
    memcpy(&(bpb_.sectorPerTrack), p, 2);
    p += 2;
    memcpy(&(bpb_.numberOfHeads), p, 2);
    p += 2;
    memcpy(&(bpb_.hiddenSector), p, 4);
    p += 4;
    memcpy(&(bpb_.totalBigSector), p, 4);
    p += 4;
    bpb_.driveId = *p;
    p += 1;
    bpb_.dirtyFlag = *p;
    p += 1;
    bpb_.extendBootSign = *p;
    p += 1;
    memcpy(&(bpb_.serialNo), p, 4);
    p += 4;
    memcpy(&(bpb_.volume), p, 11);
    p += 11;
    memcpy(&(bpb_.type), p, 8);
    return true;
}
