#include "z.h"
#include <global.h>

void *calloc(size_t nmemb, size_t size)
{
        char *p;
        unsigned int i;

        size = size * nmemb;

        p = (char*)malloc(size);
        if (NULL == p)
                return NULL;

        for (i = 0; i < size; i++)
                p[i] = 0;

        return p;
}

int encode (input_stream *is, output_stream *os)
{
        z_stream zst;
        int st, flag = Z_NO_FLUSH;

        zst.zalloc = Z_NULL;
        zst.zfree = Z_NULL;
        zst.opaque = Z_NULL;
        if (deflateInit(&zst, Z_DEFAULT_COMPRESSION) != Z_OK)
                return -1;

        zst.next_in = is->bf;
        zst.avail_in = 0;
        zst.next_out = os->bf;
        zst.avail_out = os->sz;

        for (;;) {
                if (zst.avail_in == 0) {
                        zst.next_in = is->bf;
                        zst.avail_in = is->read(is, is->sz);
                        if (zst.avail_in <= 0)
                                flag = Z_FINISH;
                }

                st = deflate(&zst, flag);
                if (st == Z_STREAM_END)
                        break;
                if (st != Z_OK)
                        return -1;

                if (zst.avail_out == 0) {
                        os->write(os, os->sz);
                        zst.next_out = os->bf;
                        zst.avail_out = os->sz;
                }
        }

        os->write(os, os->sz - zst.avail_out);

        if (deflateEnd(&zst) != Z_OK)
                return -1;

        return 0;
}

int decode (input_stream *is, output_stream *os)
{
    g_console->printf("[2.0]");
        z_stream zst;
        int st, flag = Z_NO_FLUSH;

        zst.zalloc = Z_NULL;
        zst.zfree = Z_NULL;
        zst.opaque = Z_NULL;
        if (inflateInit(&zst) != Z_OK)
                return -1;
    g_console->printf("[2.1]");

        zst.next_in = is->bf;
        zst.avail_in = 0;
        zst.next_out = os->bf;
        zst.avail_out = os->sz;

    g_console->printf("[2.2]");

        for (;;) {
    g_console->printf("[2.3]");
                if (zst.avail_in == 0) {

                        zst.next_in = is->bf;

                        zst.avail_in = is->read(is, is->sz);
                        if (zst.avail_in <= 0)
                                flag = Z_FINISH;
                }
    g_console->printf("[2.4]");
    g_console->printf("[2.4.1][%x][%x][%x][%x]", (dword)(&zst), (dword)(zst.next_in), (dword)(zst.avail_in), (dword)(zst.next_out), (dword)(zst.avail_out));
                st = inflate(&zst, Z_NO_FLUSH);
    g_console->printf("[2.4.2]");
                if (st == Z_STREAM_END)
                        break;

    g_console->printf("[2.5]");
                if (st != Z_OK)
                        return -1;

    g_console->printf("[2.6]");

                if (zst.avail_out == 0) {
                        os->write(os, os->sz);
                        zst.next_out = os->bf;
                        zst.avail_out = os->sz;
                }
    g_console->printf("[2.7]");
        }

        os->write(os, os->sz - zst.avail_out);

        if (inflateEnd(&zst) != Z_OK)
                return -1;

        return 0;
}
