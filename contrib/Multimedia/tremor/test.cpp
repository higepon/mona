#include <math.h>
#include <stdio.h>
#include <string.h>
#include <monapi/syscall.h>
#include <monapi/messages.h>
#include <monapi/Message.h>
#include <ivorbisfile.h>

static uint8_t pcmout[4096];

int main(int ac,char **av) {
    OggVorbis_File vf;
    int eof = 0;
    int current_section;


    FILE* fp = fopen("/APPS/A.OGG", "rb");
    if (NULL == fp) {
        fprintf(stderr, "file open error\n");
        return -1;
    }

    if (ov_open(fp, &vf, NULL, 0) < 0) {
        fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
        return -1;
    }

#if 0
    char **ptr=ov_comment(&vf,-1)->user_comments;
    vorbis_info *vi=ov_info(&vf,-1);
    while(*ptr){
        fprintf(stderr, "%s\n",*ptr);
        ++ptr;
    }
    fprintf(stderr, "\nBitstream is %d channel, %dHz\n", vi->channels, vi->rate);
    fprintf(stderr, "\nDecoded length: %d samples\n",
            (long)ov_pcm_total(&vf, -1));
    fprintf(stderr, "Encoded by: %s\n\n", ov_comment(&vf, -1)->vendor);
#endif


//    while(!eof){
//        long ret = ov_read(&vf, pcmout, sizeof(pcmout), &current_section);
    long ret = ov_read(&vf, pcmout, 16, &current_section);
        if (ret == 0) {
            /* EOF */
            eof=1;
        } else if (ret < 0) {
            /* error in the stream.  Not a problem, just reporting it in
               case we (the app) cares.  In this case, we don't. */
            //      printf("%s %s:%d\n", __func__, __FILE__, __LINE__);fflush(stdout);// debug
        } else {
            /* we don't bother dealing with sample rate changes, etc, but
               you'll have to*/
            char buf[32];
            int i;
            for (i = 0; i < 16; i++) {
                sprintf(buf, "0x%02x ", pcmout[i]);
                printf(buf);
            }
        }
//    }
    return 0;
}
