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
//#include<global.h>

#define ATTR_READ_ONLY  0x01
#define ATTR_HIDDEN     0x02
#define ATTR_SYSTEM     0x04
#define ATTR_VOLUME_ID  0x08
#define ATTR_DIRECTORY  0x10
#define ATTR_ARCHIVE    0x20

const int FAT12::BPB_ERROR         = 1;
const int FAT12::NOT_FAT12_ERROR   = 2;
const int FAT12::FAT_READ_ERROR    = 3;
const int FAT12::NOT_DIR_ERROR     = 4;
const int FAT12::DRIVER_READ_ERROR = 5;
const int FAT12::PATH_LENGTH       = 512;
const char FAT12::PATH_SEP         = '\\';

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
    return;
}

/*!
  \brief destroy

  \author HigePon
  \Date   create:2003/04/10 update:
*/
FAT12::~FAT12() {

    return;
}

bool FAT12::initilize() {

    /* read and set BPB */
    if (!setBPB()) {
        errNum_ = BPB_ERROR;
        return false;
    }

    /* specify file system */
    if (!isFAT12()) {
        errNum_ = NOT_FAT12_ERROR;
        return false;
    }

    /* read fat */
    fatStart_ = bpb_.reservedSectorCount;
    if (!(driver_->read(fatStart_, fat_))) {
        errNum_ = FAT_READ_ERROR;
        return false;
    }

    /* set parameters depend on bpb */
    rootDirSectors_  = ((bpb_.rootEntryCount * 32) + (bpb_.bytesPerSector - 1)) / bpb_.bytesPerSector;
    firstDataSector_ = bpb_.reservedSectorCount + bpb_.numberFats * bpb_.fatSize16 + rootDirSectors_;
    rootEntryStart_  = bpb_.reservedSectorCount + bpb_.fatSize16 * bpb_.numberFats;


    for (int i = 0; i < 20; i++) printf("[%d:%x]", i, getFATAt(i));

    printf("fat[");
    for (int k = 0; k < 20; k++) printf("%x", fat_[k]);



    int rootEntryStart = bpb_.reservedSectorCount
        + bpb_.fatSize16 * bpb_.numberFats;

    if (!(driver_->read(rootEntryStart, buf_))) return false;

    printf("rootEntryStart = %d\n", rootEntryStart);

    DirectoryEntry entry[50];

    memcpy(entry, buf_, sizeof(DirectoryEntry) * 20);

    for (int j = 0; j < 15; j++) {

        if (entry[j].filename[0] == 0xe5 || entry[j].filename[0] == 0x00) continue;

        printf("[");
        for (int i = 0; i < 8; i++) printf("%c", (char)(entry[j].filename[i]));
        printf("]");
        printf("[");

        if (entry[j].attribute & ATTR_DIRECTORY) printf("DIR");
        else {
            for (int i = 0; i < 3; i++) printf("%c", (char)(entry[j].extension[i]));
        }
        printf("]");
        printf("size[%d]=%d ", j, entry[j].filesize);
        printf("%d/%d/%d ", entry[j].fdate.year + 1980, entry[j].fdate.month, entry[j].fdate.day);
        printf("%d:%d:%d ", entry[j].ftime.hour, entry[j].ftime.min, (entry[j].ftime.sec) * 2);
        printf("cluster = %d\n", entry[j].cluster);
    }


    int rootDirSectors = ((bpb_.rootEntryCount * 32) + (bpb_.bytesPerSector - 1)) / bpb_.bytesPerSector;
    int firstDataSector = bpb_.reservedSectorCount + bpb_.numberFats * bpb_.fatSize16 + rootDirSectors;

    int cluster = 5;
    int size = entry[10].filesize;

    printf("size of entry[10]=%d", entry[10].filesize);

//      do {

//          int lbp = ((cluster - 2) * bpb_.sectorPerCluster) + firstDataSector;

//          if (!(driver_->read(lbp, buf_))) return false;

//          for (int k = 0; k < 512 && size > 0; k++, size--) {
//              printf("%c", (char)buf_[k]);
//          }

//          cluster = getFATAt(cluster);
//          printf("cluster = %d \n", cluster);

//      } while (0xff8 > cluster);

    printf("sizeof directryentry 32 = %d", sizeof(DirectoryEntry));

    int lbp = ((4 - 2) * bpb_.sectorPerCluster) + firstDataSector;

    if (!(driver_->read(lbp, buf_))) return false;

    memcpy(entry, buf_, sizeof(DirectoryEntry) * 50);

    for (int j = 0; j < 50; j++) {

        /* free */
        if (entry[j].filename[0] == 0xe5) continue;

        if (entry[j].filename[0] == 0x00) break;

        printf("[");
        for (int i = 0; i < 8; i++) printf("%c", (char)(entry[j].filename[i]));
        printf("]");
        printf("[");

        if (entry[j].attribute & ATTR_DIRECTORY) printf("DIR");
        else {
            for (int i = 0; i < 3; i++) printf("%c", (char)(entry[j].extension[i]));
        }
        printf("]");
        printf("size[%d]=%d ", j, entry[j].filesize);
        printf("%d/%d/%d ", entry[j].fdate.year + 1980, entry[j].fdate.month, entry[j].fdate.day);
        printf("%d:%d:%d ", entry[j].ftime.hour, entry[j].ftime.min, (entry[j].ftime.sec) * 2);
        printf("cluster = %d\n", entry[j].cluster);
    }

    cluster = 15;
    size = entry[2].filesize;

    printf("size of entry[2]=%d", entry[2].filesize);

//      do {

//          int lbp = ((cluster - 2) * bpb_.sectorPerCluster) + firstDataSector;

//          if (!(driver_->read(lbp, buf_))) return false;

//          for (int k = 0; k < 512 && size > 0; k++, size--) {
//              printf("%c", (char)buf_[k]);
//          }

//          cluster = getFATAt(cluster);
//          printf("cluster = %d \n", cluster);

//      } while (0xff8 > cluster);

    char buf[] = "yyyymmdd";
    printf("@@@%s@@", strtok(buf, "/"));
    printf("@@@%s@@", strtok(NULL, "/"));

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
    dataSector = totalSector - (bpb_.reservedSectorCount + (bpb_.numberFats * bpb_.fatSize16) + rootDirSectors);
    countOfClusters = dataSector / bpb_.sectorPerCluster;

    /* FAT12 */
    if (countOfClusters < 4085) return true;

    /* FAT16, FAT32 or ??? */
    return false;
}

/*!
  \brief reade BPB and parse

  \return true/false OK/NG

  \author HigePon
  \Date   create:2003/04/17 update:
*/
bool FAT12::setBPB() {

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
    bpb_.numberFats = *p;
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

        printf("try to cdr to %s", rpath);

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

    DirectoryEntry entries[16];
    int lbp = clusterToLbp(currentDirecotry_);

    /* root directory has no "." or "..", but they are necesary */
    if (currentDirecotry_ == 0 && !strcmp(path, ".")) return true;
    if (currentDirecotry_ == 0 && !strcmp(path, "..")) return true;

    /* read */
    if (!(driver_->read(lbp, buf_))) return false;
    memcpy(entries, buf_, sizeof(DirectoryEntry) * 16);

    for (int j = 0; j < 16; j++) {

        /* free */
        if (entries[j].filename[0] == 0xe5) continue;

        /* no other entries */
        if (entries[j].filename[0] == 0x00) break;

        /* not directory */
        if (!(entries[j].attribute & ATTR_DIRECTORY)) continue;

        for (int k = 0; k < 8; k++) printf("%c", (char)entries[j].filename[k]);

        /* change directory ok */
        if (compareName((char*)(entries[j].filename), path)) {

            currentDirecotry_ = entries[j].cluster;
            strcpy(currentPath_, path);
            printf("directory %s found\n", path);
            printf("current cluster = %d\n", currentDirecotry_);
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

    /* find file in entries */
    DirectoryEntry entries[16];
    int lbp = clusterToLbp(currentDirecotry_);

    /* read current directory */
    if (!(driver_->read(lbp, buf_))) {
        currentDirecotry_ = currentDirecotry;
        return false;
    }
    memcpy(entries, buf_, sizeof(DirectoryEntry) * 16);

    for (int j = 0; j < 16; j++) {

        /* free */
        if (entries[j].filename[0] == 0xe5) continue;

        /* no other entries */
        if (entries[j].filename[0] == 0x00) break;

        /* directory */
        if (entries[j].attribute & ATTR_DIRECTORY) continue;

        /* file found */
        if (compareName((char*)(entries[j].filename), file)
            && compareName((char*)(entries[j].extension), ext)) {

            currentCluster_ = entries[j].cluster;
	    fileSize_       = entries[j].filesize;
            isOpen_         = true;
	    readHasNext_    = true;
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

bool FAT12::createFlie(const char* name) {return true;}

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

bool FAT12::write(const char* file, byte* buffer) {return true;}
bool FAT12::rename(const char* from, const char* to) {return true;}
bool FAT12::remove(const char* file) {return true;}
bool FAT12::removeDirecotry(const char* name) {return true;}
bool FAT12::makeDirectory(const char* name) {return true;}
