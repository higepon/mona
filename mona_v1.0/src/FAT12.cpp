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

#include<stdio.h>
#include<stdlib.h>
//#include<global.h>

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

const int FAT12::BPB_ERROR          = 1;
const int FAT12::NOT_FAT12_ERROR    = 2;
const int FAT12::FAT_READ_ERROR     = 3;
const int FAT12::NOT_DIR_ERROR      = 4;
const int FAT12::DRIVER_READ_ERROR  = 5;
const int FAT12::FILE_EXIST_ERROR   = 6;
const int FAT12::DRIVER_WRITE_ERROR = 7;
const int FAT12::PATH_LENGTH        = 512;
const char FAT12::PATH_SEP          = '\\';

const int FAT12::READ_MODE = 0;

/*!
  \brief initilize

  \author HigePon
  \date   create:2003/04/10 update:
*/
FAT12::FAT12(DiskDriver* driver) {

    driver_ = driver;
    errNum_ = 0;
    currentDirecotry_ = 0;
    fat_ = (byte*)0;
    return;
}

/*!
  \brief destroy

  \author HigePon
  \Date   create:2003/04/10 update:
*/
FAT12::~FAT12() {

    free(fat_);
    delete(map_);
    return;
}

bool FAT12::initilize() {

    /* read and set BPB */
    if (!readBPB()) {
        errNum_ = BPB_ERROR;
        return false;
    }

    /* specify file system */
    if (!isFAT12()) {
        errNum_ = NOT_FAT12_ERROR;
        return false;
    }

    /* read fat */
    if (!readFAT(true)) {
        errNum_ = FAT_READ_ERROR;
        return false;
    }

    /* cluster map */
    int mapSize = 512 * bpb_.fatSize16 / 3 + 1;
    map_ = new BitMap(mapSize);
    for (int j = 0; j < mapSize; j++) {

        if (getFATAt(j)) map_->mark(j);
    }

    /* set parameters depend on bpb */
    rootDirSectors_  = ((bpb_.rootEntryCount * 32) + (bpb_.bytesPerSector - 1)) / bpb_.bytesPerSector;
    firstDataSector_ = bpb_.reservedSectorCount + bpb_.numberFATs * bpb_.fatSize16 + rootDirSectors_;
    rootEntryStart_  = bpb_.reservedSectorCount + bpb_.fatSize16 * bpb_.numberFATs;

    return true;
}

/*!
  \brief specify file system.

  \return true/false FAT12/other

  \author HigePon
  \Date   create:2003/04/17 update:
*/
bool FAT12::isFAT12() {

    int rootDirSectors;
    int totalSector;
    int dataSector;
    int countOfClusters;

    rootDirSectors = ((bpb_.rootEntryCount * 32) + (bpb_.bytesPerSector - 1))
        / bpb_.bytesPerSector;

    totalSector = (bpb_.totalSector16 != 0) ? bpb_.totalSector16 : bpb_.totalSector32;
    dataSector = totalSector - (bpb_.reservedSectorCount + (bpb_.numberFATs * bpb_.fatSize16) + rootDirSectors);
    countOfClusters = dataSector / bpb_.sectorPerCluster;

    /* FAT12 */
    if (countOfClusters < 4085) return true;

    /* FAT16, FAT32 or ??? */
    return false;
}

/*!
  \brief read BPB and parse

  \return true/false OK/NG

  \author HigePon
  \Date   create:2003/04/17 update:
*/
bool FAT12::readBPB() {

    byte* p = buf_;

    if (!(driver_->read(0, buf_))) return false;

    p += 3;
    memcpy(bpb_.oemName, p, 8);
    p += 8;
    memcpy(&(bpb_.bytesPerSector), p, 2);
    p += 2;
    bpb_.sectorPerCluster = *p;
    p += 1;
    memcpy(&(bpb_.reservedSectorCount), p, 2);
    p += 2;
    bpb_.numberFATs = *p;
    p += 1;
    memcpy(&(bpb_.rootEntryCount), p, 2);
    p += 2;
    memcpy(&(bpb_.totalSector16), p, 2);
    p += 2;
    bpb_.media = *p;
    p += 1;
    memcpy(&(bpb_.fatSize16), p, 2);
    p += 2;
    memcpy(&(bpb_.sectorPerTrack), p, 2);
    p += 2;
    memcpy(&(bpb_.numberOfHeaders), p, 2);
    p += 2;
    memcpy(&(bpb_.hiddenSector), p, 4);
    p += 4;
    memcpy(&(bpb_.totalSector32), p, 4);
    p += 4;
    bpb_.driveNumber = *p;
    p += 1;
    bpb_.reserved1 = *p;
    p += 1;
    bpb_.bootSignature = *p;
    p += 1;
    memcpy(&(bpb_.volumeId), p, 4);
    p += 4;
    memcpy(&(bpb_.volumeLabel), p, 11);
    p += 11;
    memcpy(&(bpb_.fileSysType), p, 8);
    return true;
}

/*!
  \brief get ErrorNo

  \return error number

  \author HigePon
  \Date   create:2003/04/17 update:
*/
int FAT12::getErrorNo() const {
    return errNum_;
}

/*!
  \brief get fat at cluster

  \param cluster cluster
  \return fat

  \author HigePon
  \date   create:2003/04/10 update:
*/
word FAT12::getFATAt(int cluster) const {

    word result;
    int index = cluster * 12 / 8;

    if (cluster % 2) {

        result = ((fat_[index] & 0xf0) >> 4) | (fat_[index + 1] << 4);
    } else {

        result = (fat_[index]) | ((fat_[index + 1] & 0xf) << 8);
    }

    return result;
}

bool FAT12::setFATAt(int cluster, word fat) {

    int index = cluster * 12 / 8;

    if (cluster % 2) {

        fat_[index]     = (fat_[index] & 0x0f) | ((fat & 0x0f) << 4);
        fat_[index + 1] = fat >> 4;
    } else {

        fat_[index] = fat & 0xff;
        fat_[index + 1] = (fat_[index + 1] & 0xf0) | ((fat & 0xf00) >> 8);
    }
    return true;
}


/*!
  \brief change directory

  \param path absolutepath
  \return true/false OK/NG

  \author HigePon
  \date   create:2003/04/10 update:
*/
bool FAT12::changeDirectory(const char* path) {

    int  currentDirectory;
    char buf[PATH_LENGTH];
    char sep[] = {PATH_SEP, '\0'};
    char* rpath;

    /* for strtok */
    strcpy(buf, path);

    /* save current */
    currentDirectory = currentDirecotry_;

    for (int i = 0; i < 512; i++) {

        /* get token */
        char* p = (i == 0) ? buf : NULL;
        if (!(rpath = strtok(p, sep))) break;

        printf("\n--try to cdr to %s--", rpath);

        if (!changeDirectoryRelative(rpath)) {

            errNum_           = NOT_DIR_ERROR;
            currentDirecotry_ = currentDirectory;
            return false;
        }
    }

    /* done changedirectory */
    return true;
}

/*!
  \brief change directory relative

  \param path realative path
  \return true/false OK/NG

  \author HigePon
  \date   create:2003/04/10 update:
*/
bool FAT12::changeDirectoryRelative(const char* path) {

    int lbp = clusterToLbp(currentDirecotry_);

    /* root directory has no "." or "..", but they are necessary */
    if (currentDirecotry_ == 0 && !strcmp(path, ".")) return true;
    if (currentDirecotry_ == 0 && !strcmp(path, "..")) return true;

    /* read */
    if (!(driver_->read(lbp, buf_))) return false;
    memcpy(entries_, buf_, sizeof(DirectoryEntry) * 16);

    for (int j = 0; j < 16; j++) {

        /* free */
        if (entries_[j].filename[0] == 0xe5) continue;

        /* no other entries_ */
        if (entries_[j].filename[0] == 0x00) break;

        /* not directory */
        if (!(entries_[j].attribute & ATTR_DIRECTORY)) continue;

        for (int k = 0; k < 8; k++) printf("%c", (char)entries_[j].filename[k]);

        /* change directory ok */
        if (compareName((char*)(entries_[j].filename), path)) {

            currentDirecotry_ = entries_[j].cluster;
            strcpy(currentPath_, path);
            printf("\n--directory %s found\n", path);
            printf("\n--current cluster = %d\n", currentDirecotry_);
            return true;
        }
    }
    return false;
}

/*!
  \brief compare file name

  \param name1 not teminated by '\0'
  \param name2 terminated by '\0'

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/04/10 update:
*/
bool FAT12::compareName(const char* name1, const char* name2) const {

    for (int i = 0; i < 9; i++) {

        if (name2[i] == '\0') return true;

        if (name1[i] != name2[i]) {
            return false;
        }
    }
    return false;
}

/*!
  \brief convert cluster to LBP

  \param cluster cluster

  \return LBP

  \author HigePon
  \date   create:2003/04/10 update:
*/
int FAT12::clusterToLbp(int cluster) {

    if (cluster < 2) return rootEntryStart_;

    int lbp = ((cluster - 2) * bpb_.sectorPerCluster) + firstDataSector_;
    return lbp;
}

/*!
  \brief open file

  \param path path
  \param filename file name
  \param mode read/write see FAT12#READ_MODE
  \return true/false OK/NG

  \author HigePon
  \date   create:2003/04/10 update:
*/
bool FAT12::open(const char* path, const char* filename, int mode) {

    char  buf[16];
    char* file;
    char* ext;

    /* save current directory */
    int currentDirecotry = currentDirecotry;

    /* toknize */
    strcpy(buf, filename);
    file = strtok(buf, ".");
    ext  = strtok(NULL, ".");

    /* change direcotory */
    if (!changeDirectory(path)) return false;

    /* find file in entries_ */
    int lbp = clusterToLbp(currentDirecotry_);

    /* read current directory */
    if (!(driver_->read(lbp, buf_))) {
        currentDirecotry_ = currentDirecotry;
        return false;
    }
    memcpy(entries_, buf_, sizeof(DirectoryEntry) * 16);

    for (int j = 0; j < 16; j++) {

        /* free */
        if (entries_[j].filename[0] == 0xe5) continue;

        /* no other entries_ */
        if (entries_[j].filename[0] == 0x00) break;

        /* directory */
        if (entries_[j].attribute & ATTR_DIRECTORY) continue;

        /* file found */
        if (compareName((char*)(entries_[j].filename), file)
            && compareName((char*)(entries_[j].extension), ext)) {

            currentCluster_ = entries_[j].cluster;
            fileSize_       = entries_[j].filesize;
            isOpen_         = true;
            readHasNext_    = true;
            firstWrite_     = true;
            currentEntry_ = &(entries_[j]);
            printf("flie %s found\n", filename);
            return true;
        }
    }
    currentDirecotry_ = currentDirecotry;
    return false;
}

bool FAT12::close() {
    isOpen_ = false;
    return true;
}

bool FAT12::read(byte* buffer) {

    /* has no next */
    if (!readHasNext_) return false;

    /* read */
    int lbp = clusterToLbp(currentCluster_);
    if (!(driver_->read(lbp, buf_))) return false;
    memcpy(buffer, buf_, 512);

    /* check fat */
    if ((currentCluster_ = getFATAt(currentCluster_)) > 0xff8) {
        readHasNext_ = false;
    }
    return true;
}

bool FAT12::readHasNext() const {

    return readHasNext_;
}

bool FAT12::createFlie(const char* name, const char* ext) {

    int lbp = clusterToLbp(currentDirecotry_);

    /* read current directory */
    if (!(driver_->read(lbp, buf_))) {
        errNum_ = DRIVER_READ_ERROR;
        return false;
    }
    memcpy(entries_, buf_, sizeof(DirectoryEntry) * 16);

    /* find free entry */
    int freeIndex = -1;
    for (int j = 0; j < 16; j++) {

        /* free */
        if (entries_[j].filename[0] == 0xe5) {
            freeIndex = j;
            continue;
        }

        /* no other entries_ */
        if (entries_[j].filename[0] == 0x00) break;

        /* debug */
        printf("\n");
        for (int i = 0; i < 8; i++) printf("%c", (char)entries_[j].filename[i]);
        printf("\n attribute %d",entries_[j].attribute);
        printf("\n ftime[%d:%d:%d]", entries_[j].ftime.sec, entries_[j].ftime.min,  entries_[j].ftime.hour);
        printf("\n fdate[%d:%d:%d]", entries_[j].fdate.day, entries_[j].fdate.month,  entries_[j].fdate.year);


        /* directory */
        if (entries_[j].attribute & ATTR_DIRECTORY) continue;

        /* file found */
        if (compareName((char*)(entries_[j].filename), name)
            && compareName((char*)(entries_[j].extension), ext)) {
            errNum_ = FILE_EXIST_ERROR;
            return false;
        }
    }

    if (freeIndex == -1) {
        return false;
    }

    int cluster = map_->find();

    if (cluster == -1) return false;

    for (int k = 0; k < 8; k++) entries_[freeIndex].filename[k]  = ' ';
    for (int k = 0; k < 3; k++) entries_[freeIndex].extension[k] = ' ';
    for (int k = 0; k < 8 && name[k] != '\0'; k++) entries_[freeIndex].filename[k]  = name[k];
    for (int k = 0; k < 3 && name[k] != '\0'; k++) entries_[freeIndex].extension[k] = ext[k];
    entries_[freeIndex].filesize  = 0;
    entries_[freeIndex].cluster   = cluster;
    entries_[freeIndex].attribute = 0x20;

    printf("\n--createFile:cluster=%d", cluster);

    setFATAt(cluster, getFATAt(1));
    printf("\n--wrtten fat is %x", getFATAt(cluster));

    /* write current directory */
    memcpy(buf_, entries_, sizeof(DirectoryEntry) * 16);
    if (!(driver_->write(lbp, buf_))) {
        errNum_ = DRIVER_READ_ERROR;
        return -1;
    }

    if (!writeFAT()) {
        readFAT(false);
        return false;
    }

    return true;
}

bool FAT12::writeFAT() {

    for (int i = 0; i < bpb_.fatSize16; i++) {

        if (!(driver_->write(fatStart_ + i, fat_ + i * 512))) return false;
    }
    return true;
}

/*!
  \brief read FAT

  \param allocate true:allocate memory/false:don't allocate memory
  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:
*/
bool FAT12::readFAT(bool allocate) {

    fatStart_ = bpb_.reservedSectorCount;

    if (allocate && !(fat_ = (byte*)malloc(512 * bpb_.fatSize16))) return false;

    for (int i = 0; i < bpb_.fatSize16; i++) {

        if (!(driver_->read(fatStart_ + i, fat_ + i * 512))) return false;
    }
    return true;
}

bool FAT12::write(byte* buffer, int size) {

    if (size <= 0 || size > 512) return false;

    int cluster = currentCluster_;

    if (!isOpen_) return false;

    if (firstWrite_) {
        firstWrite_ = false;
    } else {

        int nextCluster = getFATAt(cluster);

        printf("[[nextCluster=%d]]", nextCluster);

        if (nextCluster > 0xff8) {

            int next = map_->find();
            if (next == -1) return false;

            setFATAt(cluster, next);
            printf("cluseter** %d, %d", cluster, next);
            printf("conf %d$$", getFATAt(44));
            setFATAt(next   , getFATAt(1));
            printf("cluster** %d, %d", next, getFATAt(1));
            cluster         = next;
            currentCluster_ = cluster;
        } else {
            currentCluster_= nextCluster;
        }
    }

    int lbp = clusterToLbp(cluster);

    byte inbuf[512];
    memset(inbuf, 0, 512);
    memcpy(inbuf, buffer, size);

    (currentEntry_->filesize) += size;
    printf("@@write@@");
    for (int k = 0; k < 8; k++) printf("%c", (char)(currentEntry_->filename[k]));

    writeEntry();

    if (!(driver_->write(lbp, inbuf))) {
        errNum_ = DRIVER_READ_ERROR;
        return false;
    }

    if (!writeFAT()) {
        readFAT(false);
        return false;
    }

    return true;
}

bool FAT12::write(byte* buffer) {

    return write(buffer, 512);
}

bool FAT12::writeEntry() {

    byte buf[512];
    int lbp = clusterToLbp(currentDirecotry_);

    memcpy(buf, entries_, sizeof(DirectoryEntry) * 16);
    if (!(driver_->write(lbp, buf))) {
        errNum_ = DRIVER_READ_ERROR;
        return false;
    }
    return true;
}

bool FAT12::rename(const char* from, const char* to) {return true;}
bool FAT12::remove(const char* file) {return true;}
bool FAT12::removeDirecotry(const char* name) {return true;}
bool FAT12::makeDirectory(const char* name) {return true;}
