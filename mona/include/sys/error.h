/*!
    \file  error.h
    \brief error defineition

    Copyright (c) 2004 HigePon
    WITHOUT ANY WARRANTY

    \author  HigePon
    \version $Revision$
    \date   create:2004/03/10 update:$Date$
*/

#ifndef _MONA_ERROR_
#define _MONA_ERROR_

// 

// http://sena--web.hp.infoseek.co.jp/oldbeos/BeBookJ/The%20Support%20Kit/ErrorCodes.html



// deprecated start

enum
{
    FS_NO_ERROR            = 0x00,
    FS_INIT_ERROR          = 0x01,
    FS_GET_ROOT_ERROR      = 0x02,
    FS_GET_DIR_ERROR       = 0x03,
    FS_DIR_NOT_EXIST_ERROR = 0x04,
    FS_ALREADY_OPEN_ERROR  = 0x05,
    FS_FILE_OPEN_ERROR     = 0x06,
    FS_FILE_NOT_FOUND      = 0x06,
    FS_FILE_IS_NOT_OPEN    = 0x07,
    FS_FILE_EXIST          = 0x08,
    FS_FILE_CREATE_ERROR   = 0x09,
    MUTEX_TIMEOUT          = -1,
};

// IDManager Errors
//   Since id allcated by IDManager greater than zero,
//   error numbers are defined as negative number.
enum
{                              // A normal value (which means no error) should be positive.
    M_OK                 = 0,  // OK is always zero
    M_XX = -5000,
    M_BAD_MUTEX_ID       ,
    M_TIMED_OUT,
    IDM_OBJECT_NOT_FOUND , // Error should be negative
    IDM_SECURITY_ERROR   ,
    IDM_INVALID_TYPE     
};


#endif
