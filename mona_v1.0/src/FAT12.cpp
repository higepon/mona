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


// for test on Windows
#ifdef FS_TEST

#include<stdio.h>
#include<stdlib.h>

// for Mona
#else

#include<global.h>
#include<operator.h>

#define printf g_console->printf

#endif

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

const int FAT12::NORMAL_STATE       = 0;
const int FAT12::BPB_ERROR          = 1;
const int FAT12::NOT_FAT12_ERROR    = 2;
const int FAT12::FAT_READ_ERROR     = 3;
const int FAT12::NOT_DIR_ERROR      = 4;
const int FAT12::DRIVER_READ_ERROR  = 5;
const int FAT12::FILE_EXIST_ERROR   = 6;
const int FAT12::DRIVER_WRITE_ERROR = 7;
const int FAT12::PATH_LENGTH        = 512;
const char FAT12::PATH_SEP          = '\\';

const int FAT12::READ_MODE  = 0;
const int FAT12::WRITE_MODE = 1;

/*!
  \brief initilize

  \author HigePon
  \date   create:2003/04/10 update:
*/
FAT12::FAT12(DiskDriver* driver) {

    driver_ = driver;
    errNum_ = NORMAL_STATE;
    fat_    = (byte*)0;
    currentDirecotry_ = 0;
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

/*!
  \brief initilize FAT12Driver

  \author HigePon
  \Date   create:2003/04/10 update:
*/
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

    printf("after is FAT12\n");

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

    if (!(bpb_.bytesPerSector) || bpb_.sectorPerCluster) {

        printf("FAT12: 0 %d, %d", bpb_.bytesPerSector, bpb_.sectorPerCluster);
        return false;
    }

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

    if (!(driver_->read(1, buf_))) return false;

    for (int i = 0; i < 512; i++) {

        printf("%d", buf_[i]);
    }

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

/*!
  \brief set fat at cluster

  \param cluster cluster
  \param fat fat
  \return true

  \author HigePon
  \date   create:2003/04/10 update:
*/
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

    int lba = clusterToLba(currentDirecotry_);

    /* root directory has no "." or "..", but they are necessary */
    if (currentDirecotry_ == 0 && !strcmp(path, ".")) return true;
    if (currentDirecotry_ == 0 && !strcmp(path, "..")) return true;

    /* read */
    if (!(driver_->read(lba, buf_))) return false;
    memcpy(entries_, buf_, sizeof(DirectoryEntry) * 16);

    for (int j = 0; j < 16; j++) {

        /* free */
        if (entries_[j].filename[0] == 0xe5) continue;

        /* no other entries_ */
        if (entries_[j].filename[0] == 0x00) break;

#ifdef FAT12_DEBUG
        printf("currentDirecotry_ = %d cdr entry %s:", currentDirecotry_, path);
        printf("&&[");
        for (int k = 0; k < 8; k++) printf("%c", entries_[j].filename[k]);
        printf("]&&");
#endif

        /* not directory */
        if (!(entries_[j].attribute & ATTR_DIRECTORY)) continue;

        /* change directory ok */
        if (compareName((char*)(entries_[j].filename), path)) {

            currentDirecotry_ = entries_[j].cluster;
            strcpy(currentPath_, path);
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
  \brief convert cluster to LBA

  \param cluster cluster

  \return LBA

  \author HigePon
  \date   create:2003/04/10 update:
*/
int FAT12::clusterToLba(int cluster) {

    if (cluster < 2) return rootEntryStart_;

    int lba = ((cluster - 2) * bpb_.sectorPerCluster) + firstDataSector_;
    return lba;
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

    /* tokenize */
    strcpy(buf, filename);
    file = strtok(buf, ".");
    ext  = strtok(NULL, ".");

    if (!changeDirectory(path)) return false;

    if (!readEntry()) return false;

    /* find file to open */
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

            currentEntry_   = &(entries_[j]);
            currentCluster_ = currentEntry_->cluster;
            fileSize_       = currentEntry_->filesize;
            isOpen_         = true;
            readHasNext_    = true;
            firstWrite_     = true;
            openMode_       = mode;
            if (mode == WRITE_MODE) currentEntry_->filesize = 0;
            return true;
        }
    }

    /* file not found */
    currentDirecotry_ = currentDirecotry;
    return false;
}

/*!
  \brief file close

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:
*/
bool FAT12::close() {

    isOpen_ = false;

    if (openMode_ == WRITE_MODE) {

        writeEntry();

        if (!writeFAT()) {
            readFAT(false);
            return false;
        }
    }
    return true;
}

/*!
  \brief read

  \param buffer read buffer

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:
*/
bool FAT12::read(byte* buffer) {

    /* has no next */
    if (!readHasNext_) return false;

    /* read */
    int lba = clusterToLba(currentCluster_);
    if (!(driver_->read(lba, buf_))) return false;
    memcpy(buffer, buf_, 512);
    fileSize_ -= 512;

    /* check fat & size */
    if ((currentCluster_ = getFATAt(currentCluster_)) > 0xff8 || fileSize_ <= 0) {
        readHasNext_ = false;
    }
    return true;
}

/*!
  \brief check read has next


  \return true/false hasnext/has no next

  \author HigePon
  \date   create:2003/05/03 update:
*/
bool FAT12::readHasNext() const {

    return readHasNext_;
}

/*!
  \brief create file at current directory

  \param name filename
  \param ext  fileextension

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:2003/05/102
*/
bool FAT12::createFlie(const char* name, const char* ext) {

    /* read current directory */
    if (!readEntry()) return false;

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

        /* directory */
        if (entries_[j].attribute & ATTR_DIRECTORY) continue;

        /* file found */
        if (compareName((char*)(entries_[j].filename), name)
            && compareName((char*)(entries_[j].extension), ext)) {
            errNum_ = FILE_EXIST_ERROR;
            return false;
        }
    }

    /* no empty entry */
    if (freeIndex == -1)  return false;

    /* find cluster */
    int cluster = map_->find();
    if (cluster == -1) return false;

    /* file attribute */
    for (int k = 0; k < 8; k++) entries_[freeIndex].filename[k]  = ' ';
    for (int k = 0; k < 3; k++) entries_[freeIndex].extension[k] = ' ';
    for (int k = 0; k < 8 && name[k] != '\0'; k++) entries_[freeIndex].filename[k]  = name[k];
    for (int k = 0; k < 3 && name[k] != '\0'; k++) entries_[freeIndex].extension[k] = ext[k];
    entries_[freeIndex].filesize  = 0;
    entries_[freeIndex].cluster   = cluster;
    entries_[freeIndex].attribute = ATTR_ARCHIVE;

    if (!writeEntry()) return false;

    /* regist FAT */
    setFATAt(cluster, getFATAt(1));
    if (!writeFAT()) {
        readFAT(false);
        return false;
    }
    return true;
}

/*!
  \brief write FAT

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:
*/
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

/*!
  \brief write to file. file should be open.
  \buffer buffer to write
  \size   buffer size.  0 <= size <=512

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:
*/
bool FAT12::write(byte* buffer, int size) {

    if (!isOpen_ || size <= 0 || size > 512) return false;

    /* save current Cluster */
    int cluster = currentCluster_;

    int nextCluster = getFATAt(currentCluster_);

    if (firstWrite_) {
        firstWrite_ = false;

    } else if (nextCluster > 0xff8) {

        int next;

        /* allocate next cluster */
        if ((next = map_->find()) == -1) return false;

        /* set allocated cluster to FAT */
        setFATAt(currentCluster_, next);
        setFATAt(next           , getFATAt(1));

        /* change current */
        currentCluster_ = next;
    } else {
        currentCluster_ = nextCluster;
    }

    /* size check */
    memset(buffer + size, 0, 512 - size);
    (currentEntry_->filesize) += size;

    /* write buffer to Disk */
    int lba = clusterToLba(currentCluster_);

#ifdef FAT12_DEBUG
    printf("FAT12::write called lba=%d", lba);
#endif

    if (!(driver_->write(lba, buffer))) {
        currentCluster_ = cluster;
        errNum_ = DRIVER_READ_ERROR;
        return false;
    }
    return true;
}

/*!
  \brief write to file. file should be open. write size is 512.

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:
*/
bool FAT12::write(byte* buffer) {

    return write(buffer, 512);
}

/*!
  \brief write directory entry

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/03 update:
*/
bool FAT12::writeEntry() {

    byte buf[512];
    int lba = clusterToLba(currentDirecotry_);

    memcpy(buf, entries_, sizeof(DirectoryEntry) * 16);
    if (!(driver_->write(lba, buf))) {
        errNum_ = DRIVER_WRITE_ERROR;
        return false;
    }
    return true;
}

/*!
  \brief read directory entry

  \return true/false OK/NG

  \author HigePon
  \date   create:2003/05/10 update:
*/
bool FAT12::readEntry() {

    byte buf[512];
    int lba = clusterToLba(currentDirecotry_);

    if (!(driver_->read(lba, buf))) {
        errNum_ = DRIVER_READ_ERROR;
        return false;
    }

    memcpy(entries_, buf, sizeof(DirectoryEntry) * 16);
    return true;
}

bool FAT12::rename(const char* from, const char* to) {return true;}
bool FAT12::remove(const char* file) {return true;}
bool FAT12::removeDirecotry(const char* name) {return true;}
bool FAT12::makeDirectory(const char* name) {return true;}
