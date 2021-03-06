/*
Copyright (c) 2008 Shotaro Tsuji

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <monapi/messages.h>
#include <monapi.h>
#include <sys/error.h>
#include "mona_file.h"

using namespace MonAPI;

int __mlibc_mona_file_is_valid(void *f, int fid)
{
	//puts(__func__);
	return fid < M_OK ? 0 : 1;
}

int __mlibc_mona_file_open(void *f, const char *file, int flags)
{
    uint32_t fid = 0;

    fid = monapi_file_open(file, 0);
    if( fid < M_OK && flags & F_CREATE )
        fid = monapi_file_open(file, FILE_CREATE);
    return (int)fid;
}

int __mlibc_mona_file_close(void *f, int fid)
{
	return monapi_file_close(fid) != M_OK ? 0 : 1;
}

int __mlibc_mona_file_read(void *self, void *buf, size_t size)
{
    FILE *f = NULL;
    uint32_t fid = 0;
    unsigned char *p = (unsigned char*)buf;
    int readsize = 0;
    int i = 0;
    f = (FILE*)self;
    fid = f->file;
    scoped_ptr<SharedMemory> shm(monapi_file_read(fid, (uint32_t)size));
    if( shm.get() == NULL )
    {
        return -1;
    }
    readsize = (int)shm->size();
    memcpy(p, shm->data(), readsize);
    monapi_file_seek(fid, readsize, SEEK_CUR);
    return readsize;
}

int __mlibc_mona_file_write(void *self, void *buf, size_t size)
{
    uint32_t result = 0;
    FILE *f = NULL;

    f = (FILE*)self;

    MonAPI::SharedMemory shm(size);
    if (shm.map(true) != M_OK) {
        return -1;
    }
    memcpy(shm.data(), buf, shm.size());

    result = monapi_file_write((uint32_t)f->file, shm, shm.size());

    monapi_file_seek((uint32_t)f->file, (uint32_t)size+f->offset, SEEK_SET);
    return (int)result;
}

int __mlibc_mona_file_seek(void *self, int pos, int whence)
{
    MONAPI_BOOL result = 0;
    FILE *f = (FILE*)self;

    result = monapi_file_seek((uint32_t)f->file, (uint32_t)pos, (uint32_t)whence);
    if( result < 0 )
    {
        return -1;
    }

    return 0;
}

struct __mlibc_file_operators_ __file_ops =
{
	__mlibc_mona_file_open,
	__mlibc_mona_file_close,
	__mlibc_mona_file_read,
	__mlibc_mona_file_write,
	__mlibc_mona_file_seek,
	__mlibc_mona_file_is_valid
};
