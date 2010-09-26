#ifndef _MONA_DEPRECATED_
#define _MONA_DEPRECATED_

// enum
// {
//     FS_NO_ERROR            = 0x00,
//     FS_INIT_ERROR          = 0x01,
//     FS_GET_ROOT_ERROR      = 0x02,
//     FS_GET_DIR_ERROR       = 0x03,
//     FS_DIR_NOT_EXIST_ERROR = 0x04,
//     FS_ALREADY_OPEN_ERROR  = 0x05,
//     FS_FILE_OPEN_ERROR     = 0x06,
//     FS_FILE_NOT_FOUND      = 0x06,
//     FS_FILE_IS_NOT_OPEN    = 0x07,
//     FS_FILE_EXIST          = 0x08,
//     FS_FILE_CREATE_ERROR   = 0x09,
//     MUTEX_TIMEOUT          = -1,
// };

enum
{
    HOGE = 4000,
    IDM_OBJECT_NOT_FOUND , // Error should be negative
    IDM_SECURITY_ERROR   ,
    IDM_INVALID_TYPE
};

#endif
