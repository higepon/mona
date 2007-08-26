#include "audio_driver.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/Message.h>
#include <ivorbisfile.h>


/* FILE I/O functions (replace this) */
static size_t
exx_read(void* p,size_t s,size_t n,void *fp){
    return fread(p,s,n,(FILE*)fp);
}

static
int
exx_seek64(void* fp,ogg_int64_t l,int w){
    return fseek((FILE*)fp,l,w);
}

static
int
exx_close(void *fp){
    fclose((FILE*)fp);
    return 0;
}

static
long
exx_tell(void *fp){
    return ftell((FILE*)fp);
}


/* main routine */

int
main2(int ac,char **av){
    _printf("hoge");
    int i;
    int bs_ptr = 0; /* not used, but updated by ov_read */
    FILE* fp;
#define BUFLEN 8192
    char buf[BUFLEN];
    OggVorbis_File ovf;
    ov_callbacks exx = {
        exx_read,exx_seek64,exx_close,exx_tell
    };

//    if(ac != 2) return -1;
    _printf("open %s...", "A.OGG");
    fp = fopen("/APPS/A.OGG","rb");
    if(!fp) {
        _printf("failed.\n");
        return -2;
    }else{
        _printf("succeeded.\n");
    }

//    _printf("[[[%d]]]\n", fread(buf, 1, 8192,fp));

    i = ov_open_callbacks(fp,&ovf,NULL,0,exx);

    switch(i){
        case 0:
            break;
        case OV_EREAD:
            _printf("OV_EREAD");
            break;
        case OV_ENOTVORBIS:
            _printf("OV_ENOTVORBIS");
            break;
        case OV_EVERSION:
            _printf("OV_EVERSION");
            break;

        case OV_EBADHEADER:
            _printf("OV_EBA");
            break;

        case OV_EFAULT:
            _printf("OV_EFAULT");
            break;
        default:
            _printf("-2");
            return -2;
    }

    while(1){
        i = ov_read(&ovf,buf,BUFLEN,&bs_ptr);
        if(i<0){
            switch(i){
                case OV_HOLE:
                case OV_EBADLINK:
                    continue;
            }
        }else if(!i){
            break;
        }
//        _printf("read %d byte(s)\n",i);
    }
}



int is_stopped = 0;

error_t stopped(void *ref)
{
    is_stopped = 1;
    return OK;
}

error_t render(void* ref, void* buffer, size_t size, size_t *wrote)
{
    short *p = (short*)buffer;
    static float phase = 0.0;
    static int counter = 0;
    float samplingRate = 44100;
    float sinewaveFrequency = 440;
    float freq = sinewaveFrequency * 2 * M_PI / samplingRate;
//  puts(__func__);
//  printf("p = %x\n", p);
//  if( counter >= 44100*2*3) return NG;
    for(unsigned int i = 0 ; i < size/4u ; i++ )
    {
        short wave = (short)(3000.0*sin(phase));
        *p++ = wave;
        *p++ = wave;
        phase += freq;
    }
//  counter += size;
//  *wrote = size;
    return OK;
}

error_t frender(void *ref, void *buffer ,size_t size, size_t *wrote)
{
    FILE *fp = (FILE*)ref;
    *wrote = fread(buffer, size, 1, fp);
    return feof(fp) != 0 ? NG : OK;
}

error_t cmrender(void *ref, void *buffer ,size_t size, size_t *wrote)
{
    monapi_cmemoryinfo *cmi = (monapi_cmemoryinfo*)ref;
    static unsigned int pos = 0;
    size_t write = size;
    pos += size;
    if( cmi->Size - pos < size ) write = cmi->Size - pos;
    memcpy(buffer, cmi->Data+pos, write);
    *wrote = write;
    if( cmi->Size - pos < size ) return NG;
    return OK;
}

int main(int argc, char** argv)
{
    return main2(argc, argv);
    handle_t dev;
    struct audio_data_format format;
    struct audio_driver *driver;
    monapi_cmemoryinfo *cmi;
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    cmi = monapi_file_read_all("/APPS/TEST.RAW");
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    FILE *fp;
    fp = fopen("/APPS/TEST.RAW", "r");
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    if( fp == NULL ) return 1;
    setbuf(fp, NULL);
    _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
    format.sample_rate = 44100;
    format.bits = 16;
    format.channels = 2;

    printf("callback: %x\n", &render);

    driver = audio_driver_factory("es1370");
    puts("got a driver");
    if( driver == NULL ) return 1;

    dev = driver->driver_new();
    if( dev == NULL )
    {
        puts("Couldn't open the device.");
        return 1;
    }
    driver->driver_set_format(dev, &format);
    size_t blocksize = driver->driver_get_block_size(dev);
    char *buf = (char*)malloc(blocksize);
    size_t dummy;
    int r;
//  driver->driver_set_render_callback(dev, &render, dev);
//  driver->driver_set_render_callback(dev, &frender, fp);
//  driver->driver_set_render_callback(dev, &cmrender, cmi);
    while(1)
    {
        render(NULL, buf, blocksize, &dummy);
        if( driver->driver_write_block(dev, buf) == 0 ) break;
    }

    driver->driver_start(dev);
    puts("Start");
//  while(is_stopped==0) syscall_mthread_yield_message();
    while(1)
    {
        int isng = OK;
//      render(NULL, buf, blocksize, &dummy);
        isng = frender(fp, buf, blocksize, &dummy);
        WRITE:
        r = driver->driver_write_block(dev, buf);
        if( r == 0 ) goto WRITE;
        if( isng == NG ) break;
    }
    puts("Stopped");

    fclose(fp);
    driver->driver_stop(dev);
    driver->driver_delete(dev);

    exit(0);

    return 0;
}
