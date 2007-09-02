#include "audio_driver.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/Message.h>
#include <ivorbisfile.h>


// /* FILE I/O functions (replace this) */
// static size_t
// exx_read(void* p,size_t s,size_t n,void *fp){
//     return fread(p,s,n,(FILE*)fp);
// }

// static
// int
// exx_seek64(void* fp,ogg_int64_t l,int w){
//     return fseek((FILE*)fp,l,w);
// }

// static
// int
// exx_close(void *fp){
//     fclose((FILE*)fp);
//     return 0;
// }

// static
// long
// exx_tell(void *fp){
//     return ftell((FILE*)fp);
// }

extern "C" int64_t __divdi3(int64_t a, int64_t b);




/* main routine */
static uint8_t pcmout[4096];
int main2(int ac,char **av) {
    OggVorbis_File vf;
    int eof=0;
    int current_section;

    printf("-1/2 = %d\n", __divdi3(-1, 2));

    FILE* fp = fopen("/APPS/A.OGG", "rb");
    if (NULL == fp) {
        fprintf(stderr, "file open error\n");
        return -1;
    }

    if (ov_open(fp, &vf, NULL, 0) < 0) {
        fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
        return -1;
    }

//     char **ptr=ov_comment(&vf,-1)->user_comments;
//     vorbis_info *vi=ov_info(&vf,-1);
//     while(*ptr){
//         fprintf(stderr, "%s\n",*ptr);
//         ++ptr;
//     }
//     fprintf(stderr, "\nBitstream is %d channel, %ldHz\n", vi->channels, vi->rate);
//     fprintf(stderr, "\nDecoded length: %ld samples\n",
//             (long)ov_pcm_total(&vf, -1));
//     fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);





    //    while(!eof){
      long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
      if (ret == 0) {
        /* EOF */
        eof=1;
      } else if (ret < 0) {
        /* error in the stream.  Not a problem, just reporting it in
           case we (the app) cares.  In this case, we don't. */
          printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
      } else {
        /* we don't bother dealing with sample rate changes, etc, but
           you'll have to*/
        printf("ret=%d\n", ret);
        int i;
        for (i = 10; i < 20; i++) {
            int j = 0;
            for (j = 0; j < 16; j++) {
                printf("%d ", pcmout[i * 16 + j]);
            }
            printf("\n");
        }
      }
      //    }
    return 0;

//   OggVorbis_File vf;
//   int eof = 0;
//   int current_section;

//   memset(pcmout, 0, 4096);
//     printf("-1/2 = %d\n", __divdi3(-1, 2));
//   FILE* fp = fopen("/APPS/A.OGG","rb");
//   if (NULL == fp) {
//       _printf("fopen error\n");
//       return -1;
//   }


//   int ret1 = 0;
//   if ((ret1 = ov_open(fp, &vf, NULL, 0)) < 0) {
//       fprintf(stderr,"ret=%dInput does not appear to be an Ogg bitstream.\n", ret1);
//       exit(1);
//   }

// //     char **ptr=ov_comment(&vf,-1)->user_comments;
// //     vorbis_info *vi=ov_info(&vf,-1);
// //     while(*ptr){
// //       _printf("%s\n",*ptr);
// //       ++ptr;
// //     }
// //     _printf("\nBitstream is %d channel, %dHz\n",vi->channels,vi->rate);
// //     _printf("\nDecoded length: %d samples\n",
// //             (long)ov_pcm_total(&vf,-1));
// //     _printf("Encoded by: %s\n\n",ov_comment(&vf,-1)->vendor);

// //   fread(pcmout, 1, 4096, fp);
// //         int i;
// //         for (i = 0; i < 10; i++) {
// //             int j = 0;
// //             for (j = 0; j < 16; j++) {
// //                 fprintf(stdout, "%x ", 0xff & (pcmout[i * 16 + j]));
// //             }
// //             printf("\n");
// //         }



//     //    while(!eof){
//       long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
//       if (ret == 0) {
//         /* EOF */
//         eof=1;
//       } else if (ret < 0) {
//         /* error in the stream.  Not a problem, just reporting it in
//            case we (the app) cares.  In this case, we don't. */
//           printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
//       } else {
//         /* we don't bother dealing with sample rate changes, etc, but
//            you'll have to*/
//         printf("ret=%d\n", ret);
//         int i;
//         for (i = 10; i < 20; i++) {
//             int j = 0;
//             for (j = 0; j < 16; j++) {
//                 fprintf(stdout, "%d ", pcmout[i * 16 + j]);
//             }
//             printf("\n");
//         }
//       }
//       //    }


//     _printf("hoge");
//     int i;
//     int bs_ptr = 0; /* not used, but updated by ov_read */
//     FILE* fp;
// #define BUFLEN 8192
//     char buf[BUFLEN];
//     OggVorbis_File ovf;
//     ov_callbacks exx = {
//         exx_read,exx_seek64,exx_close,exx_tell
//     };

// //    if(ac != 2) return -1;
//     _printf("open %s...", "A.OGG");
//     fp = fopen("/APPS/A.OGG","rb");
//     if(!fp) {
//         _printf("failed.\n");
//         return -2;
//     }else{
//         _printf("succeeded.\n");
//     }

// //    _printf("[[[%d]]]\n", fread(buf, 1, 8192,fp));

//     i = ov_open_callbacks(fp,&ovf,NULL,0,exx);

//     switch(i){
//         case 0:
//             break;
//         case OV_EREAD:
//             _printf("OV_EREAD");
//             break;
//         case OV_ENOTVORBIS:
//             _printf("OV_ENOTVORBIS");
//             break;
//         case OV_EVERSION:
//             _printf("OV_EVERSION");
//             break;

//         case OV_EBADHEADER:
//             _printf("OV_EBA");
//             break;

//         case OV_EFAULT:
//             _printf("OV_EFAULT");
//             break;
//         default:
//             _printf("-2");
//             return -2;
//     }

//     while(1){
//         i = ov_read(&ovf,buf,BUFLEN,&bs_ptr);
//         if(i<0){
//             switch(i){
//                 case OV_HOLE:
//                 case OV_EBADLINK:
//                     continue;
//             }
//         }else if(!i){
//             break;
//         }
// //        _printf("read %d byte(s)\n",i);
//     }
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
//     handle_t dev;
//     struct audio_data_format format;
//     struct audio_driver *driver;
//     monapi_cmemoryinfo *cmi;
//     _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//     cmi = monapi_file_read_all("/APPS/TEST.RAW");
//     _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//     FILE *fp;
//     fp = fopen("/APPS/TEST.RAW", "r");
//     _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//     if( fp == NULL ) return 1;
//     setbuf(fp, NULL);
//     _printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
//     format.sample_rate = 44100;
//     format.bits = 16;
//     format.channels = 2;

//     printf("callback: %x\n", &render);

//     driver = audio_driver_factory("es1370");
//     puts("got a driver");
//     if( driver == NULL ) return 1;

//     dev = driver->driver_new();
//     if( dev == NULL )
//     {
//         puts("Couldn't open the device.");
//         return 1;
//     }
//     driver->driver_set_format(dev, &format);
//     size_t blocksize = driver->driver_get_block_size(dev);
//     char *buf = (char*)malloc(blocksize);
//     size_t dummy;
//     int r;
// //  driver->driver_set_render_callback(dev, &render, dev);
// //  driver->driver_set_render_callback(dev, &frender, fp);
// //  driver->driver_set_render_callback(dev, &cmrender, cmi);
//     while(1)
//     {
//         render(NULL, buf, blocksize, &dummy);
//         if( driver->driver_write_block(dev, buf) == 0 ) break;
//     }

//     driver->driver_start(dev);
//     puts("Start");
// //  while(is_stopped==0) syscall_mthread_yield_message();
//     while(1)
//     {
//         int isng = OK;
// //      render(NULL, buf, blocksize, &dummy);
//         isng = frender(fp, buf, blocksize, &dummy);
//         WRITE:
//         r = driver->driver_write_block(dev, buf);
//         if( r == 0 ) goto WRITE;
//         if( isng == NG ) break;
//     }
//     puts("Stopped");

//     fclose(fp);
//     driver->driver_stop(dev);
//     driver->driver_delete(dev);

//     exit(0);

    return 0;
}
