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
    FS_FILE_IS_NOT_OPEN    = 0x07
};


#endif
