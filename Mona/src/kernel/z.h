#ifndef _Z_H_
#define _Z_H_

#include "zlib.h"
#include "string.h"
#include "operator.h"

typedef struct input_stream input_stream;

struct input_stream {
    unsigned char *bf;
    int sz;
    void *data;
    int (*read)(input_stream *p, int sz);
};

typedef struct output_stream output_stream;

struct output_stream {
    unsigned char *bf;
    int sz;
    void *data;
    int (*write)(output_stream *p, int sz);
};

int encode (input_stream *is, output_stream *os);
int decode (input_stream *is, output_stream *os);

#ifdef __cplusplus
extern "C" {
#endif

void *calloc(size_t nmemb, size_t size);

#ifdef __cplusplus
}
#endif

#endif // _Z_H_ //
