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

#ifdef MONA
#include <monapi.h>
#include <monapi/CString.h>
#include <fat_write/IStorageDevice.h>
#else
#include "types.h"
#include "HList.h"
#include "CString.h"
#endif
#include <monapi/Assert.h>



#include "File.h"

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
    ISO9660Attribute
----------------------------------------------------------------------*/
typedef struct ISO9660Attribute
{
    dword id;
    dword parentID;
    dword extent;
    dword size;
};

#endif
