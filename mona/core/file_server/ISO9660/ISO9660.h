/*!
    \file  ISO9660.h
    \brief ISO9660

    Copyright (c) 2006 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision: 3123 $
    \date   create:2004/11/06 update:$Date: 2006-04-15 13:55:21 +0900 (土, 15  4月 2006) $
*/

#ifndef __ISO_9660__
#define __ISO_9660__

#include "sys/types.h"
#include "FileDate.h"
#include <string>
#include <vector>

#define ISO_POSITION(from, to) (to - from + 1)

namespace iso9660
{
/*----------------------------------------------------------------------
    BaseVolumeDescriptor
----------------------------------------------------------------------*/
typedef struct BaseVolumeDescriptor
{
    uint8_t type;
    char id  [ISO_POSITION(2, 6)];
    uint8_t version;
    char data[ISO_POSITION(8, 2048)];
};

/*----------------------------------------------------------------------
    PrimaryVolumeDescriptor
----------------------------------------------------------------------*/
typedef struct PrimaryVolumeDescriptor
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
    uint16_t logical_block_size_l;
    uint16_t logical_block_size_b;
    uint32_t path_table_size_l;
    uint32_t path_table_size_b;
    uint32_t type_l_path_table;
    uint32_t opt_type_l_path_table;
    uint32_t type_m_path_table;
    uint32_t opt_type_m_path_table;
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
typedef struct PathTableEntry
{
    uint8_t length;
    uint8_t ext_attr_length;
    uint32_t extent;
    uint16_t parentDirectory;
    char name[0];
};
#pragma pack(0)

/*----------------------------------------------------------------------
    DirectoryEntry
----------------------------------------------------------------------*/
#pragma pack(1)
typedef struct DirectoryEntry
{
    uint8_t length;
    uint8_t ext_attr_length;
    uint32_t extent_l;
    uint32_t extent_b;
    uint32_t size_l;
    uint32_t size_b;
    char date[ISO_POSITION (19, 25)];
    unsigned existence      : 1;
    unsigned directory      : 1;
    unsigned associetedFile : 1;
    unsigned record         : 1;
    unsigned protection     : 1;
    unsigned reserved       : 2;
    unsigned lastRecord     : 1;
    uint8_t file_unit_size;
    uint8_t interleave;
    uint16_t volume_sequence_number_l;
    uint16_t volume_sequence_number_b;
    uint8_t name_len;
    char name[0];
};
#pragma pack(0)

/*----------------------------------------------------------------------
    Attribute
----------------------------------------------------------------------*/
typedef struct Attribute
{
    uint32_t id;
    uint32_t parentID;
    uint32_t extent;
    uint32_t size;
};

/*----------------------------------------------------------------------
    Entry
----------------------------------------------------------------------*/
class Entry;
typedef std::vector<Entry*> EntryList;
class Entry
{
public:
    Entry() : hasDetail(false), isDirectory(false) {}
    ~Entry() {}

public:
    Attribute attribute;
    Entry* parent;
    EntryList children;
    bool hasDetail;
    bool isDirectory;
    std::string name;
    FileDate createDate;
    FileDate modifiedDate;
};

}; // namespace iso9660
#endif
