/*!
    \file  iso9660.h
    \brief iso9660

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/11/06 update:$Date$
*/

#ifndef _ISO_9660_
#define _ISO_9660_

#include <sys/types.h>
#include <monapi/Assert.h>
#include <sys/HList.h>
#include <fat_write/IStorageDevice.h>
#include <monapi/CString.h>
#include "CFile.h"

#define ISO_POSITION(from, to) (to - from + 1)

/*----------------------------------------------------------------------
    ISOBaseVolumeDescriptor
----------------------------------------------------------------------*/
typedef struct ISOBaseVolumeDescriptor
{
    byte type;
    char id  [ISO_POSITION(2, 6)];
    byte version;
    char data[ISO_POSITION(8, 2048)];
};

/*----------------------------------------------------------------------
    ISOPrimaryVolumeDescriptor
----------------------------------------------------------------------*/
typedef struct ISOPrimaryVolumeDescriptor
{
    char type                  [ISO_POSITION (  1,   1)];
    char id                    [ISO_POSITION (  2,   6)];
    char version               [ISO_POSITION (  7,   7)];
    char unused1               [ISO_POSITION (  8,   8)];
    char system_id             [ISO_POSITION (  9,  40)];
    char volume_id             [ISO_POSITION ( 41,  72)];
    char unused2               [ISO_POSITION ( 73,  80)];
    char volume_space_size     [ISO_POSITION ( 81,  88)];
    char unused3               [ISO_POSITION ( 89, 120)];
    char volume_set_size       [ISO_POSITION (121, 124)];
    char volume_sequence_number[ISO_POSITION (125, 128)];
    word logical_block_size_l;
    word logical_block_size_b;
    dword path_table_size_l;
    dword path_table_size_b;
    dword type_l_path_table;
    dword opt_type_l_path_table;
    dword type_m_path_table;
    dword opt_type_m_path_table;
    char root_directory_record [ISO_POSITION (157, 190)];
    char volume_set_id         [ISO_POSITION (191, 318)];
    char publisher_id          [ISO_POSITION (319, 446)];
    char preparer_id           [ISO_POSITION (447, 574)];
    char application_id        [ISO_POSITION (575, 702)];
    char copyright_file_id     [ISO_POSITION (703, 739)];
    char abstract_file_id      [ISO_POSITION (740, 776)];
    char bibliographic_file_id [ISO_POSITION (777, 813)];
    char creation_date         [ISO_POSITION (814, 830)];
    char modification_date     [ISO_POSITION (831, 847)];
    char expiration_date       [ISO_POSITION (848, 864)];
    char effective_date        [ISO_POSITION (865, 881)];
    char file_structure_version[ISO_POSITION (882, 882)];
    char unused4               [ISO_POSITION (883, 883)];
    char application_data      [ISO_POSITION (884, 1395)];
    char unused5               [ISO_POSITION (1396, 2048)];
};

/*----------------------------------------------------------------------
    ISOPathTableEntry
----------------------------------------------------------------------*/
#pragma pack(2)
typedef struct ISOPathTableEntry
{
    byte length;
    byte ext_attr_length;
    dword extent;
    word parentDirectory;
    char name[0];
};
#pragma pack(0)

/*----------------------------------------------------------------------
    ISODirectoryEntry
----------------------------------------------------------------------*/
#pragma pack(1)
typedef struct ISODirectoryEntry
{
    byte length;
    byte ext_attr_length;
    dword extent_l;
    dword extent_b;
    dword size_l;
    dword size_b;
    char date[ISO_POSITION (19, 25)];
    unsigned existence      : 1;
    unsigned directory      : 1;
    unsigned associetedFile : 1;
    unsigned record         : 1;
    unsigned protection     : 1;
    unsigned reserved       : 2;
    unsigned lastRecord     : 1;
    byte file_unit_size;
    byte interleave;
    word volume_sequence_number_l;
    word volume_sequence_number_b;
    byte name_len;
    char name[0];
};
#pragma pack(0)

/*----------------------------------------------------------------------
    DirectoryEntry
----------------------------------------------------------------------*/
typedef struct DirectoryEntry
{
    DirectoryEntry() : hasDetail(false) {}

    MonAPI::CString name;
    DirectoryEntry* parent;
    HList<DirectoryEntry*> children;
    bool hasDetail;
    bool isDirectory;
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;

    /* iso */
    dword id;
    dword parentID;
    dword extent;
    dword size;
};

/*----------------------------------------------------------------------
    ISO9660
----------------------------------------------------------------------*/
class ISO9660
{
public:
    ISO9660(IStorageDevice* device, const MonAPI::CString& mountPath);
    virtual ~ISO9660();

public:
    bool Initialize();
    int GetLastError() const {return this->lastError;}
    bool Read(dword lba, byte* buffer, dword size);
    ISO::File* GetFile(const MonAPI::CString& path);
    _A<MonAPI::CString> GetFileSystemEntries(const MonAPI::CString& path);

    // for debug
    void PrintDirectory(DirectoryEntry* entry);

public:
    enum
    {
        MEMORY_ALLOCATE_ERROR       = 0x01,
        VOLUME_DESCRIPTOR_NOT_FOUND,
        DIRECTORY_NOT_FOUND,
        SECTOR_SIZE = 2048,
        READ_ERROR
    };

private:
    enum
    {
        ISO_PRIMARY_VOLUME_DESCRIPTOR = 1,
        ISO_END_VOLUME_DESCRIPTOR     = 255
    };

private:
    bool ReadVolumeDescriptor();
    bool SetDirectoryCache();
    byte* ReadPathTableIntoBuffer();
    void CreateDirectoryListFromPathTable(HList<DirectoryEntry*>* list, byte* buffer);
    void SetDirectoryRelation(HList<DirectoryEntry*>* list, DirectoryEntry* directory);
    void SetDetailInformation(DirectoryEntry* to, ISODirectoryEntry* from);
    bool SetDetailInformation(DirectoryEntry* entry);
    DirectoryEntry* FindDirectoryEntry(DirectoryEntry* root, const MonAPI::CString& path);
    DirectoryEntry* FindFileEntry(DirectoryEntry* directory, const MonAPI::CString& fileName);
    void DeleteDirectoryEntry(DirectoryEntry* entry);
    MonAPI::CString GetProperName(const MonAPI::CString& name);

private:
    IStorageDevice* device;
    MonAPI::CString mountPath;
    DirectoryEntry* rootDirectory;
    ISOPrimaryVolumeDescriptor pdescriptor;
    bool primaryVolumeDescriptorFound;

    int lastError;
};

/*----------------------------------------------------------------------
    ISO9660File
----------------------------------------------------------------------*/
class ISO9660File : public ISO::File
{
public:
    ISO9660File(ISO9660* iso, DirectoryEntry* fileEntry);
    virtual ~ISO9660File();

public:
    virtual dword Read(void* buffer, dword size);
    virtual bool Seek(int position, int flag);
    virtual dword GetSize() const {return fileEntry->size;}
    virtual const char* GetName() {return fileEntry->name;}

private:
    ISO9660* iso;
    DirectoryEntry* fileEntry;
    dword pointer;

};

#endif
