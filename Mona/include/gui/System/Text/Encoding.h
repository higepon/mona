/*!
    \file  Encoding.h
    \brief  Encoding エンコーディングクラス 定義ヘッダ

    This software is in the public domain.
    There are no restrictions on any sort of usage of this software.

    \author   
    \version $Revision$
    \date   create: update:$Date$
*/

#ifndef __SYSTEM_TEXT_ENCODING_H__
#define __SYSTEM_TEXT_ENCODING_H__

#include <monapi/Array.h>

typedef unsigned int wchar;

namespace System { namespace Text
{
    /*! \class Encoding
    \brief Encoding
    */
    class Encoding
    {
    public:
        /// UTF-8 -> UCS-4 (string)
        static _A<wchar> ToWCharArray(const char* text, int length = -1);
        /// UTF-8 -> UCS-4 (char)
        static wchar ToWChar(const char* text, int max);
        /// get UCS-4 string length from UTF-8 string
        static int GetLength(const char* text, int length = -1);
        /// get UTF-8 character length
        static int GetCharLength(const char* text, int max);
    };
}}

#endif  // __SYSTEM_TEXT_ENCODING_H__
