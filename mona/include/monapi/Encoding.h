/*
 *   Copyright (c) 2010  Higepon(Taro Minowa)  <higepon@users.sourceforge.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _MONAPI_ENCODING_
#define _MONAPI_ENCODING_

namespace MonAPI {

class Encoding
{
public:
    static int ucs4ToUtf8(uint32_t u, uint8_t* buf)
    {
        // UTF8-1
        if (u < 0x80) {
            buf[0] = (uint8_t)u;
            return 1;
            // UTF8-2
        } else if (u < 0x7ff) {
            buf[0] = 0xc0 | ((u >> 6) & 0x1f);
            buf[1] = 0x80 | (u & 0x3f);
            return 2;
            // UTF8-3
        } else if (u < 0xffff) {
            buf[0] = 0xe0 | ((u >> 12) & 0xf);
            buf[1] = 0x80 | ((u >> 6) & 0x3f);
            buf[2] = 0x80 | (u & 0x3f);
            return 3;
            // UTF8-4
        } else if (u <= 0x10ffff) {
            buf[0] = 0xf0 | ((u >> 18) & 0x7);
            buf[1] = 0x80 | ((u >> 12) & 0x3f);
            buf[2] = 0x80 | ((u >> 6) & 0x3f);
            buf[3] = 0x80 | (u & 0x3f);
            return 4;
        } else {
            buf[0] = 0xff;
            buf[1] = 0xfd;
            return 2;
        }
    }
};
}; // namespace MonAPI

#endif // _MONAPI_ENCODING_
